"""
LaserWidgets.py

wsPython widgets for my laser projector's UI.
Copyright (c) 2008 Micah Dowty

   Permission is hereby granted, free of charge, to any person
   obtaining a copy of this software and associated documentation
   files (the "Software"), to deal in the Software without
   restriction, including without limitation the rights to use, copy,
   modify, merge, publish, distribute, sublicense, and/or sell copies
   of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
   BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
   ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.

"""

from __future__ import division

import wx

from LaserObjects import *
import BluetoothConduit
import VectorMachine

__all__ = ['ValueLabel', 'ValueSlider', 'ValueSpinner', 'ValueSlider2D',
           'ScatterPlot2D', 'PollingBTConnector', 'CalibrationLabel',
           'BTConnectButton', 'BTPolledValues', 'VMPlot2D', 'ValueGrid']


class ValueLabel(wx.StaticText):
    """A StaticText widget which monitors an AdjustableValue.
       """
    def __init__(self, parent, value, name=None, formatter="%s"):
        wx.StaticText.__init__(self, parent)
        self.value = value
        self.name = name
        self.formatter = formatter
        self.value.observe(self._valueChanged)

    def _valueChanged(self, v):
        if v is None:
            vStr = "---"
        else:
            vStr = self.formatter % v

        if self.name:
            vStr = "%s: %s" % (self.name, vStr)

        self.SetLabel(vStr)


class ValueSlider(wx.Slider):
    """A slider widget which is connected to an AdjustableValue.
       Our sliders are always floating-point, represented to WX
       as an arbitrary integer with the specified resolution.
       """
    def __init__(self, parent, value, size=None, resolution=1000):
        wx.Slider.__init__(self, parent, size=size, minValue=0, maxValue=resolution)
        self.value = value
        self.resolution = resolution
        self.value.observe(self._valueChanged)
        self.Bind(wx.EVT_SLIDER, self._onEvent)

    def _valueChanged(self, v):
        if v is None:
            self.Disable()
        else:
            self.SetValue(int(self.value.getUnitValue() * self.resolution + 0.5))
            self.Enable()

    def _onEvent(self, event):
        self.value.setUnitValue(self.GetValue() / self.resolution,
                                self._valueChanged)


class ValueSpinner(wx.SpinCtrl):
    """A spin control widget which is connected to an AdjustableValue."""
    def __init__(self, parent, value):
        wx.SpinCtrl.__init__(self, parent)
        self.SetRange(value.min, value.max)

        self.value = value
        self.value.observe(self._valueChanged)
        self.Bind(wx.EVT_SPIN, self._onEvent)
        self.Bind(wx.EVT_TEXT_ENTER, self._onEvent)

    def _valueChanged(self, v):
        if v is None:
            self.Disable()
        else:
            self.SetValue(v)
            self.Enable()

    def _onEvent(self, event):
        self.value.set(self.GetValue(), self._valueChanged)


class ValueSlider2D(wx.PyControl):
    """A two-dimensional slider, connected to two AdjustableValues.
       This is a custom widget which draws a box containing a thumb
       that's draggable in both dimensions.
       """
    snapDistance = 10

    def __init__(self, parent, xValue, yValue, size=(128, 128), thumbRadius=5,
                 topDown=False, readOnly=False, snapToDiagonal=False):
        wx.PyControl.__init__(self, parent, size=size)

        self.thumbX = None
        self.thumbY = None
        self.thumbRadius = thumbRadius
        self.topDown = topDown
        self.readOnly = readOnly
        self.size = size
        self.snapToDiagonal = snapToDiagonal

        self.SetBackgroundStyle(wx.BG_STYLE_CUSTOM)
        self.Bind(wx.EVT_PAINT, self.onPaint)
        self.Bind(wx.EVT_SIZE, self.onSize)

        if not readOnly:
            self.Bind(wx.EVT_MOTION, self.onMouseEvent)
            self.Bind(wx.EVT_LEFT_DOWN, self.onMouseEvent)
            self.Bind(wx.EVT_LEFT_DCLICK, self.onMouseEvent)
            self.Bind(wx.EVT_LEFT_UP, self.onMouseEvent)

        self.xValue = xValue
        self.yValue = yValue
        xValue.observe(self._valueChanged)
        yValue.observe(self._valueChanged)

        self.bgBrush = wx.Brush(wx.SystemSettings.GetColour(wx.SYS_COLOUR_BTNFACE))
        self.thumbBrush = wx.Brush(wx.SystemSettings.GetColour(wx.SYS_COLOUR_3DHILIGHT))
        self.thumbPen = wx.Pen(wx.SystemSettings.GetColour(wx.SYS_COLOUR_3DSHADOW))

    def DoGetBestSize(self):
        return self.size

    def _valueChanged(self, v):
        if self.xValue.value is None or self.yValue.value is None:
            self.Disable()
        else:
            self.Enable()
        self.Refresh()

    def onSize(self, event):
        self.Refresh()

    def onPaint(self, event):
        dc = wx.AutoBufferedPaintDC(self)
        width, height = self.GetClientSize()

        dc.Background = self.bgBrush
        dc.Clear()
        dc.Brush = self.thumbBrush
        dc.Pen = self.thumbPen

        if self.snapToDiagonal:
            # Draw the diagonal line
            if self.topDown:
                dc.DrawLine(0, 0, width, height)
            else:
                dc.DrawLine(0, height, width, 0)

        try:
            self.thumbX = int(self.xValue.getUnitValue() * width + 0.5)
            self.thumbY = int(self.yValue.getUnitValue() * height + 0.5)
            if not self.topDown:
                self.thumbY = height - self.thumbY

        except TypeError:
            pass
        else:
            dc.CrossHair(self.thumbX, self.thumbY)
            dc.DrawCircle(self.thumbX, self.thumbY, self.thumbRadius)

        event.Skip()

    def onMouseEvent(self, event):
        if event.LeftDown():
            self.CaptureMouse()

        if event.LeftUp() and self.HasCapture():
            self.ReleaseMouse()

        if event.LeftIsDown():
            x, y = event.GetPosition()
            width, height = self.GetClientSize()

            if not self.topDown:
                y = height - y

            if self.snapToDiagonal:
                if abs(x - y) < self.snapDistance:
                    x = y = (x + y) / 2

            self.xValue.setUnitValue(x / width)
            self.yValue.setUnitValue(y / height)


class ScatterPlot2D(wx.PyControl):
    """A two-dimensional scatter plot, showing a fixed number of recent values.
       """
    def __init__(self, parent, xValue, yValue, size=(128,128), historyLen=64, topDown=False):
        wx.PyControl.__init__(self, parent, size=size)

        self.topDown = topDown
        self.size = size
        self.historyLen = historyLen
        self.reset()
        
        self.SetBackgroundStyle(wx.BG_STYLE_CUSTOM)
        self.Bind(wx.EVT_PAINT, self.onPaint)
        self.Bind(wx.EVT_SIZE, self.onSize)

        self.xValue = xValue
        self.yValue = yValue

        # Only actually observe one of our values, so we
        # get called once per point rather than once per
        # component.
        yValue.observe(self._valueChanged)

        self.bgBrush = wx.Brush(wx.SystemSettings.GetColour(wx.SYS_COLOUR_BTNFACE))
        self.crosshairPen = wx.Pen(wx.SystemSettings.GetColour(wx.SYS_COLOUR_3DSHADOW))
        self.historyPen = wx.Pen(wx.SystemSettings.GetColour(wx.SYS_COLOUR_BTNTEXT))

    def DoGetBestSize(self):
        return self.size

    def reset(self):
        self.history = [(-1, -1)] * self.historyLen
        self.head = 0

    def _valueChanged(self, v):
        # XXX: We can't rescale our points once they're captured.
        width, height = self.GetClientSize()

        try:
            x = int(self.xValue.getUnitValue() * width + 0.5)
            y = int(self.yValue.getUnitValue() * height + 0.5)
        except TypeError:
            # Disabled
            self.reset()
        else:
            if not self.topDown:
                y = height - y

            self.head = (self.head + 1) % self.historyLen
            self.history[self.head] = (x, y)

        self.Refresh()

    def onSize(self, event):
        self.Refresh()

    def onPaint(self, event):
        dc = wx.AutoBufferedPaintDC(self)

        dc.Background = self.bgBrush
        dc.Clear()

        dc.Pen = self.crosshairPen
        dc.CrossHair(*self.history[self.head])

        dc.Pen = self.historyPen
        dc.DrawPointList(self.history)

        event.Skip()


class PollingBTConnector(BTConnector):
    """This is a BTConnector subclass that can poll hardware for new
       values periodically. It's part of LaserWidgets rather than
       LaserObjects because it relies on the wxPython main loop.

       The extra 'pollInterval' argument specifies how many seconds to
       wait between receiving one value and requesting the next.  If
       pollDelay is zero, we ask for values once per round-trip to the
       hardware.
       """
    def __init__(self, value, bt, regionName, offset=0,
                 format="<I", pollInterval=0):
        BTConnector.__init__(self, value, bt, regionName, offset, format)
        self.pollInterval = pollInterval

    def onRead(self, data):
        BTConnector.onRead(self, data)
        if self.pollInterval > 0:
            wx.CallAfter(wx.CallLater, int(self.pollInterval * 1000 + 0.5), self.onConnect)
        else:
            self.onConnect()


class CalibrationLabel(wx.StaticText):
    """A StaticText widget that displays raw calibration values."""
    def __init__(self, parent, calibration):
        wx.StaticText.__init__(self, parent)
        calibration.observe(self._valueChanged)

    def _valueChanged(self, v):
        if v is None:
            self.SetLabel("(No calibration data)")
        else:
            keys = v.keys()
            keys.sort()
            self.SetLabel("Calibration: " + ' '.join(["%s=%s" % (key, v[key]) for key in keys]))


class BTConnectButton(wx.Button):
    """A UI button which allows connecting/disconnecting a BluetoothConduit,
       and which displays the current connection status.
       """
    connectLabel = "Connect"
    connectingLabel = "Connecting..."
    disconnectLabel = "Disconnect"
    disconnectingLabel = "Connecting..."

    def __init__(self, parent, bt):
        self.bt = bt
        wx.Button.__init__(self, parent, label=self.connectLabel)
        self.Bind(wx.EVT_BUTTON, self.onClicked)
        self.bt.onConnect.append(self.onConnect)
        self.bt.onDisconnect.append(self.onDisconnect)

    def onClicked(self, event):
        # XXX: Connection currently blocks the UI event loop

        if self.bt.isConnected:
            self.SetLabel(self.disconnectingLabel)
            self.Disable()
            try:
                self.bt.disconnect()
            except:
                self.onConnect()
                raise
        else:
            self.SetLabel(self.connectingLabel)
            self.Disable()
            try:
                self.bt.connect()
            except:
                self.onDisconnect()
                raise

    def onConnect(self):
        self.SetLabel(self.disconnectLabel)
        self.Enable()

    def onDisconnect(self):
        self.SetLabel(self.connectLabel)
        self.Enable()


class BTPolledValues:
    """A collection of standard AdjustableValues for data that
       we want to poll from the laser projector. The act of creating
       this object will cause us to poll several values any time the
       projector is connected, consuming additional bandwidth.
       """
    def __init__(self, bt, calibration):
        self.bt = bt
        self.calibration = calibration
        
        # Per-axis values.

        self.x = BTPolledValuesAxis(self, 'x', 0)
        self.y = BTPolledValuesAxis(self, 'y', 1)
        

class BTPolledValuesAxis:
    """The internal per-axis object that makes up half of a BTPolledValues.
       """
    def __init__(self, parent, axisName, axisNumber):
        bt = parent.bt
        cal = parent.calibration

        # Temperature sensor

        self.thermRaw = AdjustableValue()
        PollingBTConnector(self.thermRaw, bt, "therm", axisNumber, pollInterval=2.0)

        self.thermDegF = ThermValue(self.thermRaw)
        self.thermDegC = ThermValue(self.thermRaw, toFahrenheit=False)

        # Current position

        self.posRaw = AdjustableValue()
        PollingBTConnector(self.posRaw, bt, "prox_" + axisName, 0)

        self.pos = CalibratedPositionValue(self.posRaw, cal, axisName)


class VMPlot2D(wx.PyControl):
    """Interpret and visualize the results of a VectorMachine instruction stream."""
    def __init__(self, parent, instructions=None,
                 size=(384, 384), pointSkip=10):

        wx.PyControl.__init__(self, parent, size=size)

        self.instructions = instructions or []
        self.size = size
        self.pointSkip = pointSkip
        
        self.SetBackgroundStyle(wx.BG_STYLE_CUSTOM)
        self.Bind(wx.EVT_PAINT, self.onPaint)
        self.Bind(wx.EVT_SIZE, self.onSize)

        # Black color scheme. I'd prefer something a little modern,
        # but it's important to get good contrast between the
        # interpolated points and the background, and to be able to
        # discern laser points from hidden points.

        self.bgBrush = wx.Brush((0, 0, 0))
        self.crosshairPen = wx.Pen((64, 64, 64))
        self.controlPen = wx.Pen((255, 255, 255))
        self.laserPen = wx.Pen((255, 0, 0))
        self.hiddenPen = wx.Pen((100, 100, 100))
        self.textColor = (200, 200, 200)
        
    def DoGetBestSize(self):
        return self.size

    def onSize(self, event):
        self.Refresh()

    def setInstructions(self, instructions):
        self.instructions = instructions
        wx.CallAfter(self.Refresh)

    def onPaint(self, event):
        inst = self.instructions
        points = VectorMachine.interpret(inst, self.pointSkip)
        nPoints = VectorMachine.countSamples(inst)

        dc = wx.AutoBufferedPaintDC(self)
        width, height = self.GetClientSize()
        cx = width / 2
        cy = height / 2

        dc.Background = self.bgBrush
        dc.Clear()

        dc.Pen = self.crosshairPen
        dc.CrossHair(width/2, height/2)

        for x, y, le, i in points:
            x += cx
            y += cy

            if le:
                dc.Pen = self.laserPen
            else:
                dc.Pen = self.hiddenPen

            dc.DrawPoint(x,y)

            if i == 0:
                # Control point
                dc.Pen = self.controlPen
                dc.DrawCircle(x, y, 1)

        dc.TextForeground = self.textColor
        dc.DrawText(
            "%d instructions, %d points. %.1f FPS" % (
                len(inst), nPoints, VectorMachine.SAMPLE_HZ / nPoints),
            5, 5)

        event.Skip()


class ValueGrid(wx.FlexGridSizer):
    """A grid widget which displays a table of named adjustable values.
       The grid is created from a list of (name, obj) tuples.
       """
    def __init__(self, parent, items):
        wx.FlexGridSizer.__init__(self, rows=len(items), cols=3)
        self.AddGrowableCol(1, 1)

        for name, obj in items:
            self.Add(wx.StaticText(parent, label=name+":"), 0, wx.ALIGN_CENTER_VERTICAL)
            self.Add(ValueSlider(parent, obj), 1, wx.EXPAND | wx.ALL, 2)
            self.Add(ValueSpinner(parent, obj))


class LaserController(wx.BoxSizer):
    """A widget that holds common laser adjustment controls, and
       manages the connection with the laser hardware.
       """
    def __init__(self, parent):
        wx.BoxSizer.__init__(self, wx.VERTICAL)

        self.serializer = ValueSerializer()
        self.bt = BluetoothConduit.BluetoothConduit()

        self.adj = BTAdjustableValues(self.bt, self.serializer)
        self.polled = BTPolledValues(self.bt, self.adj.calibration)

	self.buttonRow = wx.BoxSizer(wx.HORIZONTAL)
        self.buttonRow.Add(BTConnectButton(parent, self.bt), 0, wx.ALL, 2)
	self.Add(self.buttonRow, 0, wx.ALL, 2)

	loadSettingsBtn = wx.Button(parent, label="Load settings...")
	loadSettingsBtn.Bind(wx.EVT_BUTTON, self.onLoadSettings)
	self.buttonRow.Add(loadSettingsBtn, 0, wx.ALL, 2)

	saveSettingsBtn = wx.Button(parent, label="Save settings...")
	saveSettingsBtn.Bind(wx.EVT_BUTTON, self.onSaveSettings)
	self.buttonRow.Add(saveSettingsBtn, 0, wx.ALL, 2)

        self.Add(CalibrationLabel(parent, self.adj.calibration), 0, wx.ALL, 2)

	thermRow = wx.BoxSizer(wx.HORIZONTAL)
	thermRow.Add(ValueLabel(parent, self.polled.x.thermDegF, "X Axis Temperature", u"%.1f \u00B0F"))
	thermRow.Add((35, -1))
	thermRow.Add(ValueLabel(parent, self.polled.y.thermDegF, "Y Axis Temperature", u"%.1f \u00B0F"))
	self.Add(thermRow, 0, wx.ALL, 2)

	self.Add(ValueGrid(parent, (
                    ("Laser power", ScaledValue(self.adj.laserPower, 1000)),
                    )), 0, wx.EXPAND | wx.ALL, 2)

	grid = wx.FlexGridSizer(rows=2, cols=6)
	grid.Add(wx.StaticText(parent, label="Proportional gain"))
	grid.Add(wx.StaticText(parent, label="Integral gain"))
	grid.Add(wx.StaticText(parent, label="Derivative gain"))
	grid.Add(wx.StaticText(parent, label="Center"))
	grid.Add(wx.StaticText(parent, label="Scale"))
	grid.Add(wx.StaticText(parent, label="Monitor"))
	grid.Add(ValueSlider2D(parent, self.adj.x.vcmPGain,self.adj.y.vcmPGain, snapToDiagonal=True), 1, wx.EXPAND)
	grid.Add(ValueSlider2D(parent, self.adj.x.vcmIGain, self.adj.y.vcmIGain, snapToDiagonal=True), 1, wx.EXPAND)
	grid.Add(ValueSlider2D(parent, self.adj.x.vcmDGain, self.adj.y.vcmDGain, snapToDiagonal=True), 1, wx.EXPAND)
        grid.Add(ValueSlider2D(parent, self.adj.x.vcmCenter, self.adj.y.vcmCenter, topDown=True), 1, wx.EXPAND)
        grid.Add(ValueSlider2D(parent, self.adj.x.vcmScale, self.adj.y.vcmScale), 1, wx.EXPAND)
        grid.Add(ScatterPlot2D(parent, self.polled.x.pos, self.polled.y.pos, topDown=True), 1, wx.EXPAND)
        self.Add(grid, 0, wx.EXPAND | wx.ALL, 2)

        filterRow = wx.BoxSizer(wx.HORIZONTAL)
        filterRow.Add(wx.StaticText(parent, label="Filter exponent: "), 0, wx.ALIGN_CENTER_VERTICAL)
	filterRow.Add((10, -1))
        filterRow.Add(wx.StaticText(parent, label="X="), 0, wx.ALIGN_CENTER_VERTICAL)
        filterRow.Add(ValueSpinner(parent, self.adj.x.proxFilter))
	filterRow.Add((10, -1))
        filterRow.Add(wx.StaticText(parent, label="Y="), 0, wx.ALIGN_CENTER_VERTICAL)
        filterRow.Add(ValueSpinner(parent, self.adj.y.proxFilter))
        self.Add(filterRow, 0, wx.EXPAND | wx.ALL, 2)

    def close(self):
        if self.bt.isConnected:
            self.bt.disconnect()

    def onLoadSettings(self, event):
        dialog = wx.FileDialog(None, "Load settings", defaultDir="../data",
                               wildcard="*.laserSet", style=wx.FD_OPEN)
        if dialog.ShowModal() == wx.ID_OK:
            self.serializer.load(open(dialog.GetPath()))

    def onSaveSettings(self, event):
        dialog = wx.FileDialog(None, "Save settings", defaultDir="../data",
                               wildcard="*.laserSet", style=wx.FD_SAVE | wx.FD_OVERWRITE_PROMPT)
        if dialog.ShowModal() == wx.ID_OK:
            self.serializer.save(open(dialog.GetPath(), 'w'))


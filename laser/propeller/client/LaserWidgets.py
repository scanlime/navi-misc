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

__all__ = ['ValueLabel', 'ValueSlider', 'ValueSpinner', 'ValueSlider2D',
           'ScatterPlot2D', 'PollingBTConnector']


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
    def __init__(self, parent, xValue, yValue, size=(200,200), thumbRadius=5,
                 topDown=False, readOnly=False):
        wx.PyControl.__init__(self, parent, size=size)

        self.thumbX = None
        self.thumbY = None
        self.thumbRadius = thumbRadius
        self.topDown = topDown
        self.readOnly = readOnly
        self.size = size

        self.SetBackgroundStyle(wx.BG_STYLE_CUSTOM)
        self.Bind(wx.EVT_PAINT, self.onPaint)
        self.Bind(wx.EVT_SIZE, self.onSize)

        if not readOnly:
            self.Bind(wx.EVT_MOTION, self.onMouseEvent)
            self.Bind(wx.EVT_LEFT_DOWN, self.onMouseEvent)
            self.Bind(wx.EVT_LEFT_DCLICK, self.onMouseEvent)

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

        try:
            self.thumbX = int(self.xValue.getUnitValue() * width + 0.5)
            self.thumbY = int(self.yValue.getUnitValue() * height + 0.5)
            if not self.topDown:
                self.thumbY = height - self.thumbY

        except TypeError:
            pass
        else:
            dc.Brush = self.thumbBrush
            dc.Pen = self.thumbPen

            dc.CrossHair(self.thumbX, self.thumbY)
            dc.DrawCircle(self.thumbX, self.thumbY, self.thumbRadius)

        event.Skip()

    def onMouseEvent(self, event):
        if event.LeftIsDown():
            x, y = event.GetPosition()
            width, height = self.GetClientSize()

            if not self.topDown:
                y = height - y

            self.xValue.setUnitValue(x / width)
            self.yValue.setUnitValue(y / height)


class ScatterPlot2D(wx.PyControl):
    """A two-dimensional scatter plot, showing a fixed number of recent values.
       """
    def __init__(self, parent, xValue, yValue, size=(128,128), historyLen=128, topDown=False):
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
        xValue.observe(self._valueChanged)
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

        dc.Pen = self.historyPen
        dc.DrawPointList(self.history)

        dc.Pen = self.crosshairPen
        dc.CrossHair(*self.history[self.head])

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
            wx.CallLater(int(self.pollInterval * 1000 + 0.5), self.onConnect)
        else:
            self.onConnect()

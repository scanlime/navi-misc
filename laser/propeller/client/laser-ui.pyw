"""
laser-ui.py

WxPython GUI for controlling the LaserProp.
Copyright (c) 2008 Micah Dowty

   Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation
   files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy,
   modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software
   is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE 
   WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
   COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

"""

from BluetoothConduit import BluetoothConduit
import wx, struct


class ConnectButton(wx.Button):
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


class FlushTimer(wx.Timer):
    """A Timer which flushes events in the BluetoothConduit at regular intervals."""
    def __init__(self, bt, msInterval=10):
        self.bt = bt
        self.msInterval = msInterval
        wx.Timer.__init__(self)

        self.Bind(wx.EVT_TIMER, self.onTimerEvent)

        bt.onConnect.append(self.onConnect)
        bt.onDisconnect.append(self.onDisconnect)
        if bt.isConnected:
            self.onConnect()

    def onConnect(self):
        self.Start(self.msInterval)

    def onDisconnect(self):
        self.Stop()

    def onTimerEvent(self, event):
        self.bt.flush()


class TweakConduit:
    """The TweakConduit object links one or more wxPython widgets to
       a 32-bit value within a BluetoothConduit memory region.
       Supports sliders and spin button widgets.
       """
    def __init__(self, bt, regionName, offset=0, widgets=None):
        self.bt = bt
        self.regionName = regionName
        self.offset = offset
        self.widgets = widgets or []
        self.value = None

        self.bt.addCallbacks(self.onConnect, self.onDisconnect)

        for w in self.widgets:
            for e in wx.EVT_SPIN, wx.EVT_SLIDER, wx.EVT_TEXT_ENTER:
                w.Bind(e, self.onWidgetEvent)

    def onConnect(self):
        # Read the current value
        self.bt.setRegionByName(self.regionName)
        self.bt.seek(self.offset)
        self.bt.read(1, self.onRead)

    def onDisconnect(self):
        for w in self.widgets:
            w.Disable()
        self.value = None

    def onRead(self, data):
        self.value = struct.unpack("<I", data)[0]
        for w in self.widgets:
            w.SetValue(self.value)
            w.Enable()

    def onWidgetEvent(self, event):
        w = event.GetEventObject()
        value = w.GetValue()
        if value == self.value:
            return
        else:
            self.value = value    

        # Update all other widgets
        for other in self.widgets:
            if other is not w:
                other.SetValue(value)
        
        # Update the hardware
        self.bt.setRegionByName(self.regionName)
        self.bt.seek(self.offset)
        self.bt.write(struct.pack("<I", self.value))


class TweakGrid(wx.FlexGridSizer):
    """A tabular grid of tweakable values, each with both a slider
       and a spin button. The rows in the grid are specified using
       a list of (regionName, offset, label, min, max) tuples.
       """
    def __init__(self, parent, bt, specification):
        wx.FlexGridSizer.__init__(self, len(specification), 3, 2, 2)
        
        for regionName, offset, label, minValue, maxValue in specification:
            textWidget = wx.StaticText(parent, label = label + ": ")
            slider = wx.Slider(parent, minValue=minValue, maxValue=maxValue, size=(600,-1))
            spinner = wx.SpinCtrl(parent)
            spinner.SetRange(0, 0x7FFFFFFF)
            TweakConduit(bt, regionName, offset, [slider, spinner])
            self.Add(textWidget)
            self.Add(slider)
            self.Add(spinner)


class MonitorLabel(wx.StaticText):
    """A StaticText widget which monitors a 32-bit value within
       a BluetoothConduit memory region.
       """
    def __init__(self, parent, bt, regionName, offset=0, formatter=str):
        wx.StaticText.__init__(self, parent)
        self.formatter = formatter
        self.bt = bt
        self.regionName = regionName
        self.offset = offset
        self.bt.addCallbacks(self.onConnect, self.onDisconnect)

    def onConnect(self):
        self.bt.setRegionByName(self.regionName)
        self.bt.seek(self.offset)
        self.bt.read(1, self.onRead)

    def onRead(self, value):
        self.onConnect()
        self.SetLabel(self.formatter(struct.unpack("<I", value)[0]))

    def onDisconnect(self):
        self.SetLabel("---")


class MouseTracker(wx.Panel):
    def __init__(self, parent, bt, regionName, size=(512,512)):
        self.bt = bt
        self.regionName = regionName
        self.buffer = []
        self.center = (size[0]/2, size[1]/2)

        wx.Panel.__init__(self, parent, size=size)
        self.SetCursor(wx.StockCursor(wx.CURSOR_CROSS))
        
        self.Bind(wx.EVT_LEFT_UP, self.onMouseEvent)
        self.Bind(wx.EVT_LEFT_DOWN, self.onMouseEvent)
        self.Bind(wx.EVT_MOTION, self.onMouseEvent)
        self.Bind(wx.EVT_PAINT, self.onPaint)

    def onMouseEvent(self, event):
        if event.LeftDown():
            self.buffer = []

        if event.LeftIsDown():
            pos = event.GetPosition()
            self.buffer.append(pos)

            if self.bt.isConnected:
                self.bt.setRegionByName(self.regionName)
                self.bt.seek(len(self.buffer))
                self.bt.write(struct.pack("<hh", pos.x - self.center[0], pos.y - self.center[1]))
                self.bt.seek(0)
                self.bt.write(struct.pack("<I", len(self.buffer)))

            self.Refresh()

    def onPaint(self, event):
        dc = wx.PaintDC(self)
        dc.Clear()
        dc.CrossHair(*self.center)
        if self.buffer:
            dc.DrawLines(self.buffer)
        event.Skip()


class MainWindow(wx.Frame):
    def __init__(self, parent=None, id=wx.ID_ANY, title="LaserProp Test"):
        wx.Frame.__init__(self, parent, id, title)
        vbox = wx.BoxSizer(wx.VERTICAL)

        self.bt = BluetoothConduit()
        self.Bind(wx.EVT_CLOSE, self.onClose)

        self.timer = FlushTimer(self.bt)
        vbox.Add(ConnectButton(self, self.bt))

        vbox.Add(TweakGrid(self, self.bt, [
            ("params", 0, "P[x]", 0, 100000000),
            ("params", 1, "P[y]", 0, 100000000),
            ("params", 12, "D[x]", 0, 100000000),
            ("params", 13, "D[y]", 0, 100000000),
            ("params", 2, "DP[x]", 0, 100000000),
            ("params", 3, "DP[y]", 0, 100000000),
            ("params", 4, "center[x]", 0, 500000),
            ("params", 5, "center[y]", 0, 500000),
            ("params", 6, "minPos[x]", 0, 100000),
            ("params", 7, "minPos[y]", 0, 100000),
            ("params", 8, "unstickGain", 0, 100000000),
            ("params", 9, "scale[x]", 0, 1000),
            ("params", 10, "scale[y]", 0, 1000),
            ("params", 11, "reps", 0, 50),
            ]))

        vbox.Add(MonitorLabel(self, self.bt, "pos_x", 0, lambda v: "pos_x: %d" % v))
        vbox.Add(MonitorLabel(self, self.bt, "pos_y", 0, lambda v: "pos_y: %d" % v))

        vbox.Add(MouseTracker(self, self.bt, "pattern"))

        self.SetSizer(vbox)
        self.SetAutoLayout(1)
        vbox.Fit(self)

        self.Show()

    def onClose(self, event):
        if self.bt.isConnected:
            self.bt.disconnect()
        self.Destroy()


if __name__ == "__main__":
    app = wx.App()
    frame = MainWindow()
    app.MainLoop()

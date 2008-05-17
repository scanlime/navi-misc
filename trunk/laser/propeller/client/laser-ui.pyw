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
import wx, struct, math


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
            w.SetValue(min(0x7FFFFFFF, self.value))
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
        self.SetLabel(self.formatter(struct.unpack("<i", value)[0]))

    def onDisconnect(self):
        self.SetLabel("---")


class VectorMachine:
    REG_SP = 0
    REG_S = 1
    REG_DS = 2
    REG_DDS = 3

    SP_NOP = 0
    SP_JUMP = 1
    SP_CLEAR = 2
    SP_INC = 3
    SP_RESET = 4

    FIXED_POINT_BITS = 16

    def __init__(self, bt, cmdRegion="vm_cmd", memRegion="vector_mem"):
        self.bt = bt
        self.cmdRegion = cmdRegion
        self.memRegion = memRegion

    def cmd(self, instr):
        self.bt.setRegionByName(self.cmdRegion)
        self.bt.write(instr)

    def pack(self, reg=REG_S, le=0, scnt=0, exp=0, y=0, x=0, offset=0):
        x &= 0x1FF
        y &= 0x1FF
        return struct.pack("<I", (reg << 30) | (le << 29) |
                           (scnt << 22) | (exp << 18) | (y << 9) | x | offset)

    def write(self, addr, data):
        self.bt.setRegionByName(self.memRegion)
        self.bt.seek(addr)
        self.bt.write(data)

    def jump(self, offset):
        """Issue a jump instruction to the provided address."""
        self.cmd(self.pack(reg=self.REG_SP, exp=self.SP_JUMP,
                           offset=offset))

    def stop(self):
        """Turn the laser off, and lock the instruction pointer
           at offset zero in an infinite loop.
           """
        self.cmd(self.pack(reg=self.REG_SP, exp=self.SP_RESET))

    def start(self):
        """Write a no-op instruction into offset zero, and continue
           execution there.
           """
        self.stop()
        self.write(0, self.pack(reg=self.REG_SP, exp=self.SP_NOP))

    def uploadVectors(self, vectors, scnt):
        """Upload a whole vector program, sending only the parts
           that the remote VectorMachine doesn't already have.
           """
        self.stop()
        buf = []

        for x, y in vectors:
            # Break up large sample counts into multiple instructions
            s = scnt
            while s > 0:
                iscnt = min(127, s)
                s -= iscnt            
        
                buf.append(self.pack(le=1, scnt=iscnt, exp=15,
                                     x=int(x), y=int(y)))

        # Loop
        buf.append(self.pack(reg=self.REG_SP, exp=self.SP_JUMP, offset=1))

        self.write(1, ''.join(buf))
        self.start()

def vecSub(a, b):
    return (a[0] - b[0], a[1] - b[1])

def vecAdd(a, b):
    return (a[0] + b[0], a[1] + b[1])

def vecMag(a):
    return math.sqrt(a[0] ** 2 + a[1] ** 2)

def vecMul(a, b):
    return (a[0] * b, a[1] * b)

def vecNorm(a):
    return vecMul(a, 1.0 / vecMag(a))

def vecDot(a, b):
    return a[0] * b[0] + a[1] * b[1]

def iterPathSampler(points, dist):
    """Given one line segment path, resample that path and yield
       new points every 'dist' units.
       """
    prev = None

    # X is an accumulator that increments every time we pass a
    # pixel on the input line. When it crosses 'dist', we generate
    # an output sample and reset it to zero.
    x = 0

    for cur in points:
        if prev is None:
            prev = cur
            yield prev
            continue

        v = vecSub(cur, prev)
        vmag = vecMag(v)

        # Walk from the previous input sample to the current one,
        # emitting output samples as we go.

        remaining = vmag

        while True:
            travel = dist - x
            if remaining < travel:
                break

            prev = vecAdd(prev, vecMul(v, travel / vmag))
            x = 0
            remaining -= travel
            yield prev

        prev = cur
        x += remaining


class DrawingWidget(wx.Panel):
    def __init__(self, parent, bt, size=(512,512)):
        self.bt = bt
        self.vm = VectorMachine(self.bt)

        self.path = []
        self.resampled = []

        wx.Panel.__init__(self, parent)
        hbox = wx.BoxSizer(wx.HORIZONTAL)

        self.dist = wx.Slider(self, minValue=1, maxValue=100, value=10,
                              style=wx.SL_VERTICAL)
        self.dist.Bind(wx.EVT_SLIDER, self.tesselate)
        hbox.Add(self.dist, 0, wx.EXPAND)

        self.scnt = wx.Slider(self, minValue=1, maxValue=512, value=256,
                              style=wx.SL_VERTICAL)
        self.scnt.Bind(wx.EVT_SLIDER, self.tesselate)
        hbox.Add(self.scnt, 0, wx.EXPAND)

        self.canvas = wx.Panel(self, size=size)
        self.canvas.SetCursor(wx.StockCursor(wx.CURSOR_CROSS))
        self.canvas.SetBackgroundStyle(wx.BG_STYLE_CUSTOM)
        self.canvas.Bind(wx.EVT_LEFT_UP, self.onMouseEvent)
        self.canvas.Bind(wx.EVT_RIGHT_UP, self.onMouseEvent)
        self.canvas.Bind(wx.EVT_LEFT_DOWN, self.onMouseEvent)
        self.canvas.Bind(wx.EVT_MOTION, self.onMouseEvent)
        self.canvas.Bind(wx.EVT_PAINT, self.onPaint)
        self.canvas.Bind(wx.EVT_SIZE, self.onSize)
        hbox.Add(self.canvas, 1, wx.EXPAND)
        self.onSize()

        self.SetSizer(hbox)
        self.SetAutoLayout(1)

    def onSize(self, event=None):
        self.canvas.Refresh()

    def onMouseEvent(self, event):
        pos = event.GetPosition()
        center = (self.Size.x / 2, self.Size.y / 2)
        pos = vecSub(pos, center)

        if event.LeftDown():
            self.path = []
            self.lastPos = pos
            self.tesselate()

        if event.LeftUp() or event.RightUp():
            if self.bt.isConnected:
                self.vm.uploadVectors(self.resampled, self.scnt.GetValue())

        if event.LeftIsDown():
            self.path.append(pos)
            self.tesselate()

    def tesselate(self, event=None):
        self.resampled = list(iterPathSampler(self.path + self.path[:1],
                                              self.dist.GetValue()))
        self.Refresh()

    def onPaint(self, event):
        dc = wx.AutoBufferedPaintDC(self.canvas)
        dc.Clear()
        center = (self.Size.x / 2, self.Size.y / 2)

        dc.Pen = wx.Pen('#aaaaaa')
        dc.CrossHair(*center)

        dc.Pen = wx.Pen('#444444')
        dc.DrawLines(self.path, *center)

        dc.Pen = wx.Pen('#800000')
        for point in self.resampled:
            dc.DrawCircle(point[0] + center[0], point[1] + center[1], 2)

        event.Skip()


class MainWindow(wx.Frame):
    def __init__(self, parent=None, id=wx.ID_ANY, title="LaserProp Test"):
        wx.Frame.__init__(self, parent, id, title)
        vbox = wx.BoxSizer(wx.VERTICAL)

        self.bt = BluetoothConduit()
        self.Bind(wx.EVT_CLOSE, self.onClose)

        self.timer = FlushTimer(self.bt)
        vbox.Add(ConnectButton(self, self.bt), 0, wx.ALL, 4)

        tweakables = []
        for axis in 'xy':
            region = 'params_' + axis
            tweakables.extend([
                (region, 0, axis + ") P", 0, 0x7FFFFFFF),
                (region, 1, axis + ") I", 0, 1000000),
                (region, 2, axis + ") D", 0, 50000000),
                (region, 3, axis + ") Center", 0, 1000000),
                (region, 4, axis + ") Scale", 0, 2000),
                ])

        vbox.Add(TweakGrid(self, self.bt, tweakables), 0, wx.ALL, 4)

        for label in (
            MonitorLabel(self, self.bt, "pos_x", 0, lambda v: "pos_x: %d" % v),
            MonitorLabel(self, self.bt, "pos_y", 0, lambda v: "pos_y: %d" % v),
            MonitorLabel(self, self.bt, "params_x", 1, lambda v: "x.i: %d" % v),
            ):
            vbox.Add(label, 0, wx.ALL, 4)

        vbox.Add(DrawingWidget(self, self.bt), 1, wx.ALL | wx.EXPAND)

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

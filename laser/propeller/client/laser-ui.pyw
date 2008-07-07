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

from __future__ import division

from BluetoothConduit import *
from LaserObjects import *
from LaserWidgets import *

import wx, struct, math


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

    def __init__(self, bt, cmdRegion="vm", memRegion="vector_mem"):
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

    def uploadVectors(self, vectors, scnt, cb, limit=256):
        self.stop()
        buf = []

        for x, y, le in vectors:
            # Break up large sample counts into multiple instructions
            s = scnt
            while s > 0 and len(buf) < limit:
                iscnt = min(127, s)
                s -= iscnt            
        
                buf.append(self.pack(le=le, scnt=iscnt, exp=15,
                                     x=int(x), y=int(y)))
            
            if len(buf) >= limit:
                break

        # Loop
        # Note the nonzero scnt: we need this loop to generate samples,
        # otherwise the VM's timer will overflow if we send it an empty image.

        buf.append(self.pack(reg=self.REG_SP, scnt=1, exp=self.SP_JUMP, offset=1))

        print "Pattern length: %d" % len(buf)
        self.write(1, ''.join(buf))
        self.start()

        def finish(_):
            print "Finished writing pattern"
            cb()
        self.bt.read(1, finish)


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
       new points every 'dist' units. Each point is an (x, y, payload)
       tuple, where 'payload' is an arbitrary Python object which is
       preserved in the resampled output. Each output sample carries
       the payload of the preceeding input sample.
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

        v = vecSub(cur[:2], prev[:2])
        vmag = vecMag(v)

        # Walk from the previous input sample to the current one,
        # emitting output samples as we go.

        remaining = vmag

        while True:
            travel = dist - x
            if remaining < travel:
                break

            prev = vecAdd(prev[:2], vecMul(v, travel / vmag)) + prev[2:]
            x = 0
            remaining -= travel
            yield prev

        prev = cur
        x += remaining


class DrawingWidget(wx.Panel):
    def __init__(self, parent, bt, size=(512,512)):
        self.bt = bt
        self.vm = VectorMachine(self.bt)

        self._needUpload = False
        self._uploadPending = False
        self.path = []
        self.resampled = []

        wx.Panel.__init__(self, parent)
        hbox = wx.BoxSizer(wx.HORIZONTAL)

        self.dist = wx.Slider(self, minValue=1, maxValue=100, value=10,
                              style=wx.SL_VERTICAL)
        self.dist.Bind(wx.EVT_SLIDER, self.tesselate)
        hbox.Add(self.dist, 0, wx.EXPAND)

        self.scnt = wx.Slider(self, minValue=1, maxValue=128, value=64,
                              style=wx.SL_VERTICAL)
        self.scnt.Bind(wx.EVT_SLIDER, self.tesselate)
        hbox.Add(self.scnt, 0, wx.EXPAND)

        self.canvas = wx.Panel(self, size=size)
        self.canvas.SetCursor(wx.StockCursor(wx.CURSOR_CROSS))
        self.canvas.SetBackgroundStyle(wx.BG_STYLE_CUSTOM)
        self.canvas.Bind(wx.EVT_LEFT_UP, self.onMouseEvent)
        self.canvas.Bind(wx.EVT_RIGHT_UP, self.onMouseEvent)
        self.canvas.Bind(wx.EVT_LEFT_DOWN, self.onMouseEvent)
        self.canvas.Bind(wx.EVT_RIGHT_DOWN, self.onMouseEvent)
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
        pos = (min(255, max(-255, pos[0])),
               min(255, max(-255, pos[1])))
   
        if event.RightDown():
            self.path = []
            self.lastPos = pos
            self.tesselate()

        if event.RightIsDown():
            self.path.append((pos[0], pos[1], event.LeftIsDown()))
            self.tesselate()

    def tesselate(self, event=None):
        self.resampled = list(iterPathSampler(self.path + self.path[:1],
                                              self.dist.GetValue()))
        self.Refresh()
        self._needUpload = True
        self._upload()

    def _upload(self):
        if not self.bt.isConnected:
            self._uploadPending = False
            return

        if self._needUpload and not self._uploadPending:
            def done():
                self._uploadPending = False
                self._upload()

            self._needUpload = False
            self._uploadPending = True
            self.vm.uploadVectors(self.resampled, self.scnt.GetValue(), done)

    def onPaint(self, event):
        dc = wx.AutoBufferedPaintDC(self.canvas)
        dc.Clear()
        center = (self.Size.x / 2, self.Size.y / 2)

        dc.Pen = wx.Pen('#aaaaaa')
        dc.CrossHair(*center)

        dc.Pen = wx.Pen('#444444')
        prevX = None
        prevY = None
        prevLE = None
        for x, y, le in self.path:
            if prevLE:
                dc.DrawLine(prevX + center[0], prevY + center[1],
                            x + center[0], y + center[1])
            prevX = x
            prevY = y
            prevLE = le

        for x, y, le in self.resampled:
            if le:
                dc.Pen = wx.Pen('#800000')
            else:
                dc.Pen = wx.Pen('#aaaaaa')

            dc.DrawCircle(x + center[0], y + center[1], 2)

        event.Skip()


class MainWindow(wx.Dialog):                 
    def __init__(self, parent=None, id=wx.ID_ANY, title="LaserProp Test"):
        wx.Dialog.__init__(self, parent, id, title)
        vbox = wx.BoxSizer(wx.VERTICAL)

        self.bt = BluetoothConduit()
        self.adj = BTAdjustableValues(self.bt)
        self.polled = BTPolledValues(self.bt, self.adj.calibration)
        self.timer = BTFlushTimer(self.bt)
        self.Bind(wx.EVT_CLOSE, self.onClose)

        vbox.Add(BTConnectButton(self, self.bt), 0, wx.ALL, 2)

        vbox.Add(CalibrationLabel(self, self.adj.calibration), 0, wx.ALL, 2)

	thermRow = wx.BoxSizer(wx.HORIZONTAL)
	thermRow.Add(ValueLabel(self, self.polled.x.thermDegF, "X Axis Temperature", u"%.1f \u00B0F"))
	thermRow.Add((35, -1))
	thermRow.Add(ValueLabel(self, self.polled.y.thermDegF, "Y Axis Temperature", u"%.1f \u00B0F"))
	vbox.Add(thermRow, 0, wx.ALL, 2)

	laserRow = wx.BoxSizer(wx.HORIZONTAL)
	laserRow.Add(wx.StaticText(self, label="Laser power:"), 0, wx.ALIGN_CENTER_VERTICAL)
	laserRow.Add(ValueSlider(self, self.adj.laserPower), 1, wx.EXPAND | wx.ALL, 2)
	laserRow.Add(ValueSpinner(self, ScaledValue(self.adj.laserPower, 1000)))
	vbox.Add(laserRow, 0, wx.EXPAND | wx.ALL, 2)

	grid = wx.FlexGridSizer(rows=2, cols=6)
	grid.Add(wx.StaticText(self, label="Proportional gain"))
	grid.Add(wx.StaticText(self, label="Integral gain"))
	grid.Add(wx.StaticText(self, label="Derivative gain"))
	grid.Add(wx.StaticText(self, label="Center"))
	grid.Add(wx.StaticText(self, label="Scale"))
	grid.Add(wx.StaticText(self, label="Monitor"))
	grid.Add(ValueSlider2D(self, self.adj.x.vcmPGain,self.adj.y.vcmPGain, snapToDiagonal=True), 1, wx.EXPAND)
	grid.Add(ValueSlider2D(self, self.adj.x.vcmIGain, self.adj.y.vcmIGain, snapToDiagonal=True), 1, wx.EXPAND)
	grid.Add(ValueSlider2D(self, self.adj.x.vcmDGain, self.adj.y.vcmDGain, snapToDiagonal=True), 1, wx.EXPAND)
        grid.Add(ValueSlider2D(self, self.adj.x.vcmCenter, self.adj.y.vcmCenter, topDown=True), 1, wx.EXPAND)
        grid.Add(ValueSlider2D(self, self.adj.x.vcmScale, self.adj.y.vcmScale, snapToDiagonal=True), 1, wx.EXPAND)
        grid.Add(ScatterPlot2D(self, self.polled.x.pos, self.polled.y.pos, topDown=True), 1, wx.EXPAND)
        vbox.Add(grid, 0, wx.EXPAND | wx.ALL, 2)

        filterRow = wx.BoxSizer(wx.HORIZONTAL)
        filterRow.Add(wx.StaticText(self, label="Filter exponent: "), 0, wx.ALIGN_CENTER_VERTICAL)
	filterRow.Add((10, -1))
        filterRow.Add(wx.StaticText(self, label="X="), 0, wx.ALIGN_CENTER_VERTICAL)
        filterRow.Add(ValueSpinner(self, self.adj.x.proxFilter))
	filterRow.Add((10, -1))
        filterRow.Add(wx.StaticText(self, label="Y="), 0, wx.ALIGN_CENTER_VERTICAL)
        filterRow.Add(ValueSpinner(self, self.adj.y.proxFilter))
        vbox.Add(filterRow, 0, wx.EXPAND | wx.ALL, 2)

        vbox.Add(DrawingWidget(self, self.bt), 1, wx.ALL | wx.EXPAND, 4)

        self.SetSizer(vbox)
        self.SetAutoLayout(1)
        vbox.Fit(self)

        self.Show()

    def onClose(self, event):
        if self.bt.isConnected:
            self.bt.disconnect()
        self.Destroy()


if __name__ == "__main__":
    app = wx.App(0)
    frame = MainWindow()
    app.MainLoop()

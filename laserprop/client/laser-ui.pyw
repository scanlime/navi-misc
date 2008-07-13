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
import VectorMachine
import SVGPath

import ILDA

import wx, struct, math, time

def loadKirby():
    # SVG path data extracted from Inkscape after joining all paths,
    # adding points (spacing of 10), and converting to lines.

    d="M 83.96875,172.15625 L 76.062532,177.13085 L 69.074451,183.32287 L 63.583636,190.85742 L 61.159828,199.78005 L 65.926164,207.24706 L 75.09375,208.375 L 84.912905,206.69043 L 94.595426,204.32697 L 104.13627,201.44677 L 113.4646,197.94184 L 122.34375,193.4375 L 113.85832,190.84347 L 105.6922,187.37198 L 97.934196,183.06489 L 90.667264,177.97347 L 83.96875,172.15625 L 83.96875,172.15625 M 59.75,93.6875 L 51.924459,99.327103 L 44.410595,105.37405 L 37.34826,111.94061 L 31.036953,119.2279 L 26.269295,127.58022 L 25.46875,137.03125 L 30.737017,143.84605 L 39.046627,146.6806 L 47.845599,147.33458 L 56.667173,146.91417 L 65.4375,145.875 L 62.185168,137.50527 L 59.843248,128.83563 L 58.436037,119.96819 L 57.96875,111 L 58.415132,102.29761 L 59.75,93.6875 L 59.75,93.6875 M 142.90625,26.0625 L 133.39535,26.589891 L 124.00269,28.176802 L 114.84847,30.812287 L 106.05033,34.463551 L 97.718972,39.080324 L 89.955674,44.600237 L 82.855787,50.950102 L 76.505895,58.050165 L 70.985863,65.813851 L 66.369239,74.145245 L 62.718238,82.943075 L 60.083101,92.096221 L 58.496173,101.48882 L 57.96875,111 L 58.496141,120.5109 L 60.083052,129.90356 L 62.718537,139.05778 L 66.369802,147.85592 L 70.986575,156.18728 L 76.506489,163.95058 L 82.856354,171.05047 L 89.956418,177.40036 L 97.720104,182.92039 L 106.0515,187.53701 L 114.84933,191.18801 L 124.00247,193.82315 L 133.39507,195.41008 L 142.90625,195.9375 L 145.59375,195.90625 L 151.04171,188.10115 L 158.11282,181.70449 L 166.16259,176.58394 L 174.85555,172.6458 L 183.96875,169.8125 L 193.46315,168.8188 L 202.625,171.34375 L 209.04807,164.2279 L 214.63345,156.43721 L 219.30669,148.06772 L 223.00269,139.22507 L 225.67136,130.01872 L 227.27832,120.57035 L 227.8125,111 L 227.28576,101.49118 L 225.70068,92.100165 L 223.06798,82.947139 L 219.42022,74.149739 L 214.80665,65.81747 L 209.29089,58.054306 L 202.94437,50.953388 L 195.8475,44.60255 L 188.0867,39.081678 L 179.75648,34.463717 L 170.96082,30.812404 L 161.80753,28.176655 L 152.41657,26.589873 L 142.90625,26.0625 L 142.90625,26.0625 M 147.48738,87.448395 L 146.23696,95.82089 L 143.14113,103.68054 L 136.62091,108.44676 L 130.69709,102.95798 L 128.53229,94.791561 L 128.26031,86.33088 L 129.51073,77.958391 L 132.60656,70.09875 L 139.12679,65.332523 L 145.05059,70.8213 L 147.21539,78.98772 L 147.48738,87.448395 L 147.48738,87.448395 M 133.52653,83.76262 L 128.27653,87.73137 L 128.57955,94.759348 L 130.15153,101.60637 L 132.43278,102.45012 L 137.34365,99.283709 L 139.05778,93.45012 L 138.03363,87.465032 L 133.52653,83.76262 L 133.52653,83.76262 M 101.78113,87.448395 L 100.53071,95.82089 L 97.43488,103.68054 L 90.914656,108.44676 L 84.990845,102.95798 L 82.826042,94.791561 L 82.554058,86.33088 L 83.804475,77.958391 L 86.90031,70.09875 L 93.420539,65.332523 L 99.344342,70.8213 L 101.50914,78.98772 L 101.78113,87.448395 L 101.78113,87.448395 M 87.82028,83.76262 L 82.57028,87.73137 L 82.873297,94.759348 L 84.44528,101.60637 L 86.72653,102.45012 L 91.637405,99.283709 L 93.35153,93.45012 L 92.32738,87.465032 L 87.82028,83.76262 L 87.82028,83.76262 M 140.84248,124.91929 L 132.39933,127.53426 L 123.95683,130.14903 L 115.51484,132.76364 L 107.07208,135.37849 L 98.62932,137.99335 L 104.68888,143.41348 L 111.49069,147.86646 L 119.02852,150.8761 L 127.08412,151.06742 L 134.19012,147.36292 L 138.84,140.75891 L 141.1275,132.99238 L 140.84248,124.91929 L 140.84248,124.91929 M 201.33356,148.83604 L 194.57518,143.04169 L 187.83893,137.2231 L 181.18737,131.30728 L 174.69774,125.21409 L 168.4869,118.83885 L 162.76675,112.02197 L 158.02168,104.50502 L 155.63126,96.000501 L 158.4184,87.74952 L 166.183,82.955826 L 175.3559,81.81202 L 184.2287,82.254347 L 193.00855,83.626256 L 201.62907,85.784449 L 210.07895,88.537978 L 218.37992,91.71516 M 201.33356,148.83604 L 194.57518,143.04169 L 187.83893,137.2231 L 181.18737,131.30728 L 174.69774,125.21409 L 168.4869,118.83885 L 162.76675,112.02197 L 158.02168,104.50502 L 155.63126,96.000501 L 158.4184,87.74952 L 166.183,82.955826 L 175.3559,81.81202 L 184.2287,82.254347 L 193.00855,83.626256 L 201.62907,85.784449 L 210.07895,88.537978 L 218.37992,91.71516 M 183.97064,169.81836 L 175.01393,172.59192 L 166.45982,176.42854 L 158.51633,181.40205 L 151.49361,187.6011 L 145.97067,195.15205 L 143.51696,204.09808 L 148.26637,211.60994 L 157.46034,212.75216 L 166.04043,211.32859 L 174.51466,209.35331 L 182.88642,206.98599 L 191.12626,204.19349 L 199.14907,200.82798 L 206.68385,196.49741 L 212.14526,189.93173 L 211.52614,181.76899 L 207.18537,174.77037 L 200.30488,170.24301 L 192.18168,168.77278 L 183.97064,169.81836 L 183.97064,169.81836"

    path = []
    cmd = None
    pos = None
    
    for token in d.split():
        if token in ('M', 'L'):
            cmd = token
        else:
            coord = map(float, token.split(','))
            if cmd == 'M':
                if pos:
                    path.append((pos[0], pos[1], False))       # Turn off laser
                for i in range(12):
                    path.append((coord[0], coord[1], False))   # Move laser

            path.append((coord[0], coord[1], True))    # Turn back on
            pos = coord

    return path


def vecSub((a, b), (c, d)):
    return (a-c, b-d)


class DrawingWidget(wx.Panel):
    def __init__(self, parent, vm, bt, prog, size=(512,512)):
        self.bt = bt
        self.vm = vm
        self.prog = prog

        self._needUpload = False
        self._uploadDeadline = None
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

            # XXX: ILDA test
	    #file = "../data/hello-world.ild"
            #for p in ILDA.readFirstFrame(open(file, 'rb')).iterPoints():
	    #	self.path.append((p.x * 255, -p.y * 255, not p.blanking))

	    # XXX: Kirby from Inkscape
            self.path = loadKirby()

            self.tesselate()

        #if event.RightIsDown():
        #    self.path.append((pos[0], pos[1], event.LeftIsDown()))
        #    self.tesselate()

    def tesselate(self, event=None):
        #self.resampled = list(iterPathSampler(self.path + self.path[:1],
        #                                      self.dist.GetValue()))
        self.resampled = self.path

        self.Refresh()
        self._needUpload = True
        self._upload()

    def _upload(self):
        if not self.bt.isConnected:
            self._uploadDeadline = False
            return

        if self._needUpload and (not self._uploadDeadline or time.time() > self._uploadDeadline):
            def done():
                print "Upload done"
                self._uploadDeadline = None
                self._upload()

            self._needUpload = False
            self._uploadDeadline = time.time() + 2

            scnt = self.scnt.GetValue()
            loop_hz = 40000.0
            print "Uploading %d resampled vectors. scnt=%d (%.1f PPS)" % (
                len(self.resampled), scnt, loop_hz / scnt)
            self.vm.runFrame(self.prog, done)

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

def bezier2(t, ((x0, y0), (x1, y1), (x2, y2))):
    # References:
    #   http://mathworld.wolfram.com/BezierCurve.html
    #   http://mathworld.wolfram.com/BernsteinPolynomial.html
    #
    # Quadratic Bezier curves use the 2nd degree Bernstein polynomials as
    # their basis functions:
    #
    #   B(0,2) = (1 - t)^2
    #   B(1,2) = 2 (1 - t) t
    #   B(2,2) = t^2

    # Intermediate values
    it = 1.0 - t
    it2 = it * it
    t2 = t * t

    # Basis functions
    b0 = it2
    b1 = 2 * it * t
    b2 = t2

    return (x0*b0 + x1*b1 + x2*b2,
            y0*b0 + y1*b1 + y2*b2)

def bezier2d(t, ((x0, y0), (x1, y1), (x2, y2))):
    # First derivative of bezier2(). Taking the derivatives of the
    # bernstein polynomials, we find:
    #
    #   B(0,2)  = (1 - t)^2
    #   B(0,2)  = 1 - 2t + t^2
    #   B(0,2)' = -2 + 2t
    #
    #   B(1,2)  = 2 (1 - t) t
    #   B(1,2)  = 2t - 2t^2
    #   B(1,2)' = 2 - 4t
    #
    #   B(2,2)  = t^2
    #   B(2,2)' = 2t

    # Basis functions
    b0 = -2 + 2*t
    b1 = 2 - 4*t
    b2 = 2*t

    return (x0*b0 + x1*b1 + x2*b2,
            y0*b0 + y1*b1 + y2*b2)


def bezier2dd(t, ((x0, y0), (x1, y1), (x2, y2))):
    # Second derivative of bezier2():
    #
    #   B(0,2)'  = -2 + 2t
    #   B(0,2)'' = 2
    #
    #   B(1,2)'  = 2 - 4t
    #   B(1,2)'' = -4
    #
    #   B(2,2)'  = 2t
    #   B(2,2)'' = 2

    return (x0*2 - x1*4 + x2*2,
            y0*2 - y1*4 + y2*2)


def bezier3(t, ((x0, y0), (x1, y1), (x2, y2), (x3, y3))):
    # References:
    #   http://mathworld.wolfram.com/BezierCurve.html
    #   http://mathworld.wolfram.com/BernsteinPolynomial.html
    #
    # Cubic Bezier curves use the 3rd degree Bernstein polynomials as
    # their basis functions:
    #
    #   B(0,3) = (1 - t)^3
    #   B(1,3) = 3 (1 - t)^2 t
    #   B(2,3) = 3 (1 - t) t^2
    #   B(3,3) = t^3

    # Intermediate values
    it = 1.0 - t
    it2 = it * it
    it3 = it2 * it
    t2 = t * t
    t3 = t2 * t

    # Basis functions
    b0 = it3
    b1 = 3 * it2 * t
    b2 = 3 * it * t2
    b3 = t3

    return (x0*b0 + x1*b1 + x2*b2 + x3*b3,
            y0*b0 + y1*b1 + y2*b2 + y3*b3)

def bezier3d(t, ((x0, y0), (x1, y1), (x2, y2), (x3, y3))):
    # First derivative of bezier3(). Taking the derivatives of the
    # bernstein polynomials, we find:
    #
    #   B(0,3)  = (1 - t)^3
    #   B(0,3)  = 1 - 3t + 3t^2 - t^3
    #   B'(0,3) = -3 + 6t - 3t^2
    #
    #   B(1,3)  = 3 (1 - t)^2 t
    #   B(1,3)  = 3t - 6t^2 + 3t^3
    #   B'(1,3) = 3 - 12t + 9t^2
    #
    #   B(2,3)  = 3 (1 - t) t^2
    #   B(2,3)  = 3t^2 - 3t^3
    #   B'(2,3) = 6t - 9t^2
    #
    #   B(3,3)  = t^3
    #   B'(3,3) = 3t^2

    # Intermediate values
    t2 = t * t
    t3 = t2 * t

    # Basis functions
    b0 = -3 + 6*t - 3*t2
    b1 = 3 - 12*t + 9*t2
    b2 = 6*t - 9*t2
    b3 = 3*t2

    return (x0*b0 + x1*b1 + x2*b2 + x3*b3,
            y0*b0 + y1*b1 + y2*b2 + y3*b3)

def bezier3dd(t, ((x0, y0), (x1, y1), (x2, y2), (x3, y3))):
    # Second derivative of bezier3():
    #
    #   B'(0,3)  = -3 + 6t - 3t^2
    #   B''(0,3) = 6 - 6t
    #
    #   B'(1,3)  = 3 - 12t + 9t^2
    #   B''(1,3) = -12 + 18t
    #
    #   B'(2,3)  = 6t - 9t^2
    #   B''(2,3) = 6 - 18t
    #
    #   B'(3,3)  = 3t^2
    #   B''(3,3) = 6t

    # Basis functions
    b0 = 6 - 6*t
    b1 = -12 + 18*t
    b2 = 6 - 18*t
    b3 = 6*t

    return (x0*b0 + x1*b1 + x2*b2 + x3*b3,
            y0*b0 + y1*b1 + y2*b2 + y3*b3)


def vmTest():
    """Test rig for interpolating paths into VectorMachine instructions."""



class MainWindow(wx.Dialog):                 
    def __init__(self, parent=None, id=wx.ID_ANY, title="LaserProp Test"):
        wx.Dialog.__init__(self, parent, id, title)
        vbox = wx.BoxSizer(wx.VERTICAL)

        self.bt = BluetoothConduit()
        self.adj = BTAdjustableValues(self.bt)
        self.polled = BTPolledValues(self.bt, self.adj.calibration)
        self.fb = VectorMachine.FrameBuffer(self.bt)
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

        self.speed = AdjustableValue(100, 1, 500)
        self.curvy = AdjustableValue(100, -400, 400)
        self.angle = AdjustableValue(0, -180, 180)
        self.speed.observe(self.redraw)
        self.curvy.observe(self.redraw)
        self.angle.observe(self.redraw)
	vbox.Add(ValueSlider(self, self.speed), 1, wx.EXPAND | wx.ALL, 2)
	vbox.Add(ValueSlider(self, self.curvy), 1, wx.EXPAND | wx.ALL, 2)
	vbox.Add(ValueSlider(self, self.angle), 1, wx.EXPAND | wx.ALL, 2)

        self.frame = 0
        #self.fb.addEmptyQueueCallback(self.animate)

        plotter = VMPlot2D(self)
        self.fb.observeFront(plotter.setInstructions)
        vbox.Add(plotter)

        self.SetSizer(vbox)
        self.SetAutoLayout(1)
        vbox.Fit(self)

        self.Show()

    def animate(self):
        self.frame += 1

        en = VectorMachine.Encoder()
        ts = SVGPath.TransformStack()
        ts.rotate((self.frame * 2.0,))
        s = int(self.speed.value)

        path = [
                ((100, 100), True),
                ((-100, 100), True),
                ((-100, -100), True),
                ((100, -100), True),
                ((100, 100), True),

                ((100, 100), False),
                ((80, 80), False),

                ((80, 80), True),
                ((-80, 80), True),
                ((-80, -80), True),
                ((80, -80), True),
                ((80, 80), True),

                ((80, 80), False),
                ((100, 100), False),
            ]

        en.setLaser(path[0][1])
        en.moveTo(*ts.transform(path[0][0]))

        for p in path[1:]:
            en.setLaser(p[1])
            en.hold(1)
            q = ts.transform(p[0])
            en.lineTo(q[0], q[1], s)

        en.hold(1)
        en.setLaser(False)
        en.hold(1)

        self.fb.replace(en.inst)
        

    def redraw(self, value):
        en = VectorMachine.Encoder()
        ts = SVGPath.TransformStack()

        ts.rotate((self.angle.value,))
        s = int(self.speed.value)
        c = int(self.curvy.value)

        bez3 = ( ts.transform((-100,0)), 
                 ts.transform((-100,c)),
                 ts.transform((100,100)),
                 ts.transform((100,0)) )

        # Draw the curve in hardware
        en.setLaser(True)
        en.moveTo(*(bez3[0] + (10,)))
        en.cCurveTo(*(bez3[1] + bez3[2] + bez3[3] + (s*2,)))

	en.setLaser(False)
        for x, y, speed in (
            (120, 0, s//2),
            (120, 100, s),
            (-120, 100, s*2),
            (-120, 0, s),
            (-100, 0, s//2),
            ):
            p = ts.transform((x, y))
            en.lineTo(p[0], p[1], speed)

        en.setLaser(False)
        en.hold(1)

        self.fb.replace(en.inst)

    def onClose(self, event):
        if self.bt.isConnected:
            self.bt.disconnect()
        self.Destroy()


if __name__ == "__main__":
    app = wx.App(0)
    frame = MainWindow()
    app.MainLoop()

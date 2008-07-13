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

import wx, struct, math, time


class MainWindow(wx.Dialog):                 
    def __init__(self, parent=None, id=wx.ID_ANY, title="LaserProp Test"):
        wx.Dialog.__init__(self, parent, id, title)
        vbox = wx.BoxSizer(wx.VERTICAL)

        self.bt = BluetoothConduit()
        self.adj = BTAdjustableValues(self.bt)
        self.polled = BTPolledValues(self.bt, self.adj.calibration)
        self.fb = VectorMachine.FrameBuffer(self.bt)
        self.Bind(wx.EVT_CLOSE, self.onClose)
        self.svgPaths = None

	buttonRow = wx.BoxSizer(wx.HORIZONTAL)
        buttonRow.Add(BTConnectButton(self, self.bt), 0, wx.ALL, 2)
        loadBtn = wx.Button(self, label="Load SVG...")
        loadBtn.Bind(wx.EVT_BUTTON, self.onLoadSVG)
        buttonRow.Add(loadBtn, 0, wx.ALL, 2)
	vbox.Add(buttonRow, 0, wx.ALL, 2)

        vbox.Add(CalibrationLabel(self, self.adj.calibration), 0, wx.ALL, 2)

	thermRow = wx.BoxSizer(wx.HORIZONTAL)
	thermRow.Add(ValueLabel(self, self.polled.x.thermDegF, "X Axis Temperature", u"%.1f \u00B0F"))
	thermRow.Add((35, -1))
	thermRow.Add(ValueLabel(self, self.polled.y.thermDegF, "Y Axis Temperature", u"%.1f \u00B0F"))
	vbox.Add(thermRow, 0, wx.ALL, 2)

	vbox.Add(ValueGrid(self, (
                    ("Laser power", ScaledValue(self.adj.laserPower, 1000)),
                    )), 0, wx.EXPAND | wx.ALL, 2)

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

        vmRow = wx.BoxSizer(wx.HORIZONTAL)

        plotter = VMPlot2D(self)
        self.fb.observeFront(plotter.setInstructions)
        vmRow.Add(plotter)

        self.beamParams = VectorMachine.BeamParams()
        self.beamParams.observeAll(self.redraw)
	vmRow.Add(ValueGrid(self, self.beamParams.items), 0, wx.EXPAND | wx.ALL, 2)
        
        vbox.Add(vmRow, 0, wx.EXPAND | wx.ALL, 2)

        self.SetSizer(vbox)
        self.SetAutoLayout(1)
        vbox.Fit(self)

        self.Show()

    def redraw(self):
        if not self.svgPaths:
            return

        en = VectorMachine.BeamAwareEncoder(self.beamParams)
        cx = 128
        cy = 128

        for path in self.svgPaths:
            for cmd in path.data:

                if cmd[0] == 'M':
                    en.moveTo(cmd[1]-cx, cmd[2]-cy)

                if cmd[0] == 'C':
                    q = SVGPath.cubicToQuadratic(en.en.s[0]+cx, en.en.s[1]+cy,
                                                 cmd[1], cmd[2],
                                                 cmd[3], cmd[4],
                                                 cmd[5], cmd[6])
                    for x1, y1, x2, y2 in q:
                        en.curveTo(x1-cx, y1-cy, x2-cx, y2-cy)

                if cmd[0] == 'L':
                    en.lineTo(cmd[1]-cx, cmd[2]-cy)

        en.close()
        self.fb.replace(en.inst)
    
    def onLoadSVG(self, event):
        dialog = wx.FileDialog(None, "Load an SVG file", defaultDir="../data", wildcard="*.svg")
        if dialog.ShowModal() == wx.ID_OK:
            self.svgPaths = SVGPath.parseSVG(dialog.GetPath()).paths
            self.redraw()

    def onClose(self, event):
        if self.bt.isConnected:
            self.bt.disconnect()
        self.Destroy()


if __name__ == "__main__":
    app = wx.App(0)
    frame = MainWindow()
    app.MainLoop()

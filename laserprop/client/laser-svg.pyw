#!/usr/bin/env python
"""
laser-svg.pyw

SVG viewer for the LaserProp projector.

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

import LaserWidgets
import VectorMachine
import SVGPath
import wx


class MainWindow(wx.Dialog):                 
    def __init__(self, parent=None, id=wx.ID_ANY, title="LaserProp SVG Loader"):
        wx.Dialog.__init__(self, parent, id, title)
        vbox = wx.BoxSizer(wx.VERTICAL)

        self.ctrl = LaserWidgets.LaserController(self)
        vbox.Add(self.ctrl, 0, wx.EXPAND)

        self.fb = VectorMachine.FrameBuffer(self.ctrl.bt)

        self.Bind(wx.EVT_CLOSE, self.onClose)
        self.svgPaths = None

        self.svgFilename = LaserWidgets.ObservableValue(None)
        self.ctrl.serializer.add("svg.filename", self.svgFilename)
        self.svgFilename.observe(self._svgFilenameSet)

        loadBtn = wx.Button(self, label="Load SVG...")
        loadBtn.Bind(wx.EVT_BUTTON, self.onLoadSVG)
        self.ctrl.buttonRow.Add(loadBtn, 0, wx.ALL, 2)

        self.reloadBtn = wx.Button(self, label="Reload")
        self.reloadBtn.Bind(wx.EVT_BUTTON, self.onReload)
        self.reloadBtn.Disable()
        self.ctrl.buttonRow.Add(self.reloadBtn, 0, wx.ALL, 2)

        vmRow = wx.BoxSizer(wx.HORIZONTAL)
        vbox.Add(vmRow, 0, wx.EXPAND | wx.ALL, 2)

        plotter = LaserWidgets.VMPlot2D(self)
        self.fb.observeFront(plotter.setInstructions)
        vmRow.Add(plotter)

        self.beamParams = VectorMachine.BeamParams(self.ctrl.serializer)
        self.beamParams.observeAll(self.redraw)
	vmRow.Add(LaserWidgets.ValueGrid(self, self.beamParams.items), 1, wx.EXPAND | wx.ALL, 2)
        
        self.SetSizer(vbox)
        self.SetAutoLayout(1)
        vbox.Fit(self)

        self.Show()

    def redraw(self):
        if not self.svgPaths:
            self.fb.replace(VectorMachine.blankFrame())
            return

        en = VectorMachine.BeamAwareEncoder(self.beamParams)

	def transform((x, y)):
            return (x-128, y-128)

        for path in self.svgPaths:
            en.svgPath(path.data, transform)

        en.close()
        self.fb.replace(en.inst)

    def onClose(self, event):
        self.ctrl.close()
        self.Destroy()
    
    def onLoadSVG(self, event):
        dialog = wx.FileDialog(None, "Load an SVG file", defaultDir="../data",
                               wildcard="*.svg", style=wx.FD_OPEN)
        if dialog.ShowModal() == wx.ID_OK:
            self.svgFilename.set(dialog.GetPath())

    def _svgFilenameSet(self, value):
        if value:
            self.reloadBtn.Enable()
            self.onReload()

    def onReload(self, event=None):
        filename = self.svgFilename.value
    	if filename:
            self.svgPaths = SVGPath.parseSVG(filename).paths
        else:
            self.svgPaths = None
        self.redraw()


if __name__ == "__main__":
    app = wx.App(0)
    frame = MainWindow()
    app.MainLoop()

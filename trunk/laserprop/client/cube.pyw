#!/usr/bin/env python

import LaserWidgets
import LaserObjects
import VectorMachine
import SVGPath
import wx
import time
import math


class MainWindow(wx.Dialog):                 
    _lastTime = None
    _animValue = 0

    cube = [
        (0,0,0),
        (0,1,0),
        (0,1,1),
        (0,0,1),
        (1,0,1),
        (1,1,1),
        (1,1,0),
        (1,0,0),
        (0,0,0),
        (0,0,1),
        (1,0,1),
        (1,0,0),
        (1,1,0),
        (0,1,0),
        (0,1,1),
        (1,1,1),
        (1,0,1),
        (1,0,0),
        ]

    def __init__(self, parent=None, id=wx.ID_ANY, title="LaserProp Cube"):
        wx.Dialog.__init__(self, parent, id, title)
        vbox = wx.BoxSizer(wx.VERTICAL)
        
        self.ctrl = LaserWidgets.LaserController(self)
        vbox.Add(self.ctrl, 0, wx.EXPAND)

        self.beamParams = VectorMachine.BeamParams(self.ctrl.serializer)
        self.animSpeed = LaserObjects.AdjustableValue(100, min=0, max=1000)
        self.rotX = LaserObjects.AdjustableValue(25, min=-90, max=90)
        self.rotZ = LaserObjects.AdjustableValue(0, min=-90, max=90)
	vbox.Add(LaserWidgets.ValueGrid(self, self.beamParams.items + [
                    ("Animation speed", self.animSpeed),
                    ("Rotate X", self.rotX),
                    ("Rotate Z", self.rotZ),
                    ]), 0, wx.EXPAND | wx.ALL, 2)

        self.fb = VectorMachine.FrameBuffer(self.ctrl.bt)
	self.fb.setEmptyQueueCallback(self.animate)
        self.en = VectorMachine.BeamAwareEncoder(self.beamParams)

        plotter = LaserWidgets.VMPlot2D(self)
        self.fb.observeFront(plotter.setInstructions)
        vbox.Add(plotter)

        self.Bind(wx.EVT_CLOSE, self.onClose)

        self.fb.append(self.animate())
        
        self.SetSizer(vbox)
        self.SetAutoLayout(1)
        vbox.Fit(self)
        self.Show()

    def onClose(self, event):
        self.ctrl.close()
        self.Destroy()
              
    def animate(self):
        now = time.time()
        if self._lastTime:
            dt = now - self._lastTime
        else:
            dt = 0
        self._lastTime = now
        self._animValue += dt * self.animSpeed.value

        ts = SVGPath.Transform3D()
        ts.scale(100)
        ts.rotateZ(self.rotZ.value)
        ts.rotateX(self.rotX.value)
        ts.rotateY(self._animValue)
        ts.translate(-0.5, -0.5, -0.5)

        for x,y,z in self.cube:
            self.en.lineTo(*ts.transform(x,y,z)[:2])

        return self.en.getFrame()


if __name__ == "__main__":
    app = wx.App(0)
    frame = MainWindow()
    app.MainLoop()

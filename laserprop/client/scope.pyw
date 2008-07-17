#!/usr/bin/env python

import LaserWidgets
import LaserObjects
import VectorMachine
import SVGPath
import wx
import time
import math
import random


class MainWindow(wx.Dialog):
    def __init__(self, parent=None, id=wx.ID_ANY, title="Laser oscilloscope test"):
        wx.Dialog.__init__(self, parent, id, title)
        vbox = wx.BoxSizer(wx.VERTICAL)
        
        self.ctrl = LaserWidgets.LaserController(self)
        vbox.Add(self.ctrl, 0, wx.EXPAND)

        self.beamParams = VectorMachine.BeamParams(self.ctrl.serializer)
	vbox.Add(LaserWidgets.ValueGrid(self, self.beamParams.items + [
                    ]), 0, wx.EXPAND | wx.ALL, 2)

        self.fb = VectorMachine.FrameBuffer(self.ctrl.bt)
	self.fb.setEmptyQueueCallback(self.animate)
        self.en = VectorMachine.BeamAwareEncoder(self.beamParams)

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
        steps = 75
        for i in xrange(0, steps):
            a = i * 2.0 * math.pi / steps
            r = 50 + random.random() * 50
            self.en.lineTo(math.sin(a)*r, math.cos(a)*r)

        return self.en.getFrame()


if __name__ == "__main__":
    app = wx.App(0)
    frame = MainWindow()
    app.MainLoop()

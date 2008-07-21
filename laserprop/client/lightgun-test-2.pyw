#!/usr/bin/env python

from __future__ import division
import LaserWidgets
import LaserObjects
import SVGPath
import VectorMachine
import wx
import math, time, struct


class MainWindow(wx.Dialog):           
    def __init__(self, parent=None, id=wx.ID_ANY, title="Laser Modulator"):
        wx.Dialog.__init__(self, parent, id, title)
        vbox = wx.BoxSizer(wx.VERTICAL)
        self.ducks = []

        self.ctrl = LaserWidgets.LaserController(self)
        vbox.Add(self.ctrl, 0, wx.EXPAND)

        self.beamParams = VectorMachine.BeamParams(self.ctrl.serializer)
        self.fb = VectorMachine.FrameBuffer(self.ctrl.bt)

        self.beamParams.speed.set(85000)
        self.beamParams.preStartEmphasis.set(150)
        self.beamParams.postStartEmphasis.set(0)
        self.beamParams.preBlankEmphasis.set(35)
        self.beamParams.cornerEmphasis.set(26)

        self.Bind(wx.EVT_CLOSE, self.onClose)

        self.followGain = LaserObjects.AdjustableValue(0, 0, 1.0)
        self.growRate = LaserObjects.AdjustableValue(1.0, 1.0, 2.0)
        self.shrinkRate = LaserObjects.AdjustableValue(1.0, 0.0, 1.0)

        calMS = LaserObjects.AdjustableValue(100, min=0, max=2000)
        LaserObjects.BTConnector(calMS, self.ctrl.bt, "zapper", 4)

        minSN = LaserObjects.AdjustableValue(0, min=0, max=0x7FFFFFFF)
        LaserObjects.BTConnector(minSN, self.ctrl.bt, "zapper", 5)

        tPulse = LaserObjects.AdjustableValue(0, min=0, max=0x7FFFFFFF)
        LaserObjects.BTConnector(tPulse, self.ctrl.bt, "zapper", 6)

        tHyst = LaserObjects.AdjustableValue(0, min=0, max=0x7FFFFFFF)
        LaserObjects.BTConnector(tHyst, self.ctrl.bt, "zapper", 7)

        vbox.Add(LaserWidgets.ValueGrid(self, self.beamParams.items + [
                    ("Follower gain", self.followGain),
                    ("Min S/N", minSN),
                    ("Calibration period (ms)", calMS),
                    ("Pulse threshold", tPulse),
                    ("Hysteresis threshold", tHyst),
                    ]), 0, wx.EXPAND | wx.ALL, 2)

        light_cnt = LaserObjects.AdjustableValue()
        LaserWidgets.PollingBTConnector(light_cnt, self.ctrl.bt, "zapper", 1)
        vbox.Add(LaserWidgets.ValueLabel(self, light_cnt, "light_cnt", "%d"))

        thresh_pulse = LaserObjects.AdjustableValue()
        LaserWidgets.PollingBTConnector(thresh_pulse, self.ctrl.bt, "zapper", 15)
        vbox.Add(LaserWidgets.ValueLabel(self, thresh_pulse, "thresh_pulse", "%d"))

        thresh_hyst = LaserObjects.AdjustableValue()
        LaserWidgets.PollingBTConnector(thresh_hyst, self.ctrl.bt, "zapper", 16)
        vbox.Add(LaserWidgets.ValueLabel(self, thresh_hyst, "thresh_hyst", "%d"))

        a = LaserObjects.AdjustableValue()
        LaserWidgets.PollingBTConnector(a, self.ctrl.bt, "zapper", 8)
        vbox.Add(LaserWidgets.ValueLabel(self, a, "Pulse len", "%d"))

        self.lightXRaw = LaserObjects.AdjustableValue()
        self.lightYRaw = LaserObjects.AdjustableValue()
        LaserWidgets.PollingBTConnector(self.lightXRaw, self.ctrl.bt, "zapper", 2)
        LaserWidgets.PollingBTConnector(self.lightYRaw, self.ctrl.bt, "zapper", 3)
        self.lightX = LaserWidgets.CalibratedPositionValue(self.lightXRaw, self.ctrl.adj.calibration, 'x')
        self.lightY = LaserWidgets.CalibratedPositionValue(self.lightYRaw, self.ctrl.adj.calibration, 'y')        

        vbox.Add(LaserWidgets.ScatterPlot2D(self, self.lightX, self.lightY, size=(384,384), topDown=True), 1, wx.EXPAND | wx.ALL)

        self.SetSizer(vbox)
        self.SetAutoLayout(1)
        vbox.Fit(self)
        self.Show()

        self.beamParams.observeAll(self.redraw)
        self.lightXRaw.observe(self.followLightGun)
	
    def onClose(self, event):
        self.ctrl.close()
        self.Destroy()

    def redraw(self, _=None):
        en = VectorMachine.BeamAwareEncoder(self.beamParams)
        en.circle(0, 0, 20)
        self.fb.replace(en.inst)

    def followLightGun(self, _=None):
        lgx = self.lightXRaw.value
        lgy = self.lightYRaw.value
        
        if lgx and lgy:
            self.ctrl.adj.x.vcmCenterRaw.set(lgx)
            self.ctrl.adj.y.vcmCenterRaw.set(lgy)


if __name__ == "__main__":
    app = wx.App(0)
    frame = MainWindow()
    app.MainLoop()

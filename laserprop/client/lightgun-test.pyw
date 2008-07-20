#!/usr/bin/env python

import LaserWidgets
import LaserObjects
import VectorMachine
import wx
import math, time

class MainWindow(wx.Dialog):                 
    def __init__(self, parent=None, id=wx.ID_ANY, title="Laser Modulator"):
        wx.Dialog.__init__(self, parent, id, title)
        vbox = wx.BoxSizer(wx.VERTICAL)

        self.ctrl = LaserWidgets.LaserController(self)
        vbox.Add(self.ctrl, 0, wx.EXPAND)

        self.fb = VectorMachine.FrameBuffer(self.ctrl.bt)

        self.Bind(wx.EVT_CLOSE, self.onClose)

        self.beamParams = VectorMachine.BeamParams(self.ctrl.serializer)
        self.beamParams.observeAll(self.redraw)

        light_thresh = LaserObjects.AdjustableValue(1000, 10, 1000000)
        LaserObjects.BTConnector(light_thresh, self.ctrl.bt, "zapper", 4)

        vbox.Add(LaserWidgets.ValueGrid(self, self.beamParams.items + [
                    ("Light threshold", light_thresh),
                    ]), 0, wx.EXPAND | wx.ALL, 2)

        buttons = LaserObjects.AdjustableValue()
        LaserWidgets.PollingBTConnector(buttons, self.ctrl.bt, "nes", 0)
        vbox.Add(LaserWidgets.ValueLabel(self, buttons, "buttons", "%02x"))

        trigger = LaserObjects.AdjustableValue()
        LaserWidgets.PollingBTConnector(trigger, self.ctrl.bt, "zapper", 0)
        vbox.Add(LaserWidgets.ValueLabel(self, trigger, "trigger_cnt", "%02x"))

        light = LaserObjects.AdjustableValue()
        LaserWidgets.PollingBTConnector(light, self.ctrl.bt, "zapper", 1)
        vbox.Add(LaserWidgets.ValueLabel(self, light, "light", "%08x"))

        light_min = LaserObjects.AdjustableValue()
        LaserWidgets.PollingBTConnector(light_min, self.ctrl.bt, "zapper", 2)
        vbox.Add(LaserWidgets.ValueLabel(self, light_min, "light_min", "%08x"))

        light_max = LaserObjects.AdjustableValue()
        LaserWidgets.PollingBTConnector(light_max, self.ctrl.bt, "zapper", 3)
        vbox.Add(LaserWidgets.ValueLabel(self, light_max, "light_max", "%08x"))

        light_cnt = LaserObjects.AdjustableValue()
        LaserWidgets.PollingBTConnector(light_cnt, self.ctrl.bt, "zapper", 5)
        vbox.Add(LaserWidgets.ValueLabel(self, light_cnt, "light_cnt", "%08x"))

        lightX = LaserObjects.AdjustableValue()
        lightY = LaserObjects.AdjustableValue()
        LaserWidgets.PollingBTConnector(lightX, self.ctrl.bt, "zapper", 6)
        LaserWidgets.PollingBTConnector(lightY, self.ctrl.bt, "zapper", 7)
        vbox.Add(LaserWidgets.ValueLabel(self, lightX, "light_x", "%08x"))
        vbox.Add(LaserWidgets.ValueLabel(self, lightY, "light_y", "%08x"))

        self.lightX = LaserWidgets.CalibratedPositionValue(lightX, self.ctrl.adj.calibration, 'x')
        self.lightY = LaserWidgets.CalibratedPositionValue(lightY, self.ctrl.adj.calibration, 'y')        
        #self.lightX.observe(self.followLightGun)

        vbox.Add(LaserWidgets.ScatterPlot2D(self, self.lightX, self.lightY, size=(384,384), topDown=True), 1, wx.EXPAND | wx.ALL)

        self.SetSizer(vbox)
        self.SetAutoLayout(1)
        vbox.Fit(self)
        self.Show()

    def onClose(self, event):
        self.ctrl.close()
        self.Destroy()

    def redraw(self, _=None):
        en = VectorMachine.BeamAwareEncoder(self.beamParams)
        en.spiral(0, 0, 20, 30, 3)
        en.close()
        self.fb.replace(en.inst)

    def followLightGun(self, _=None):
        lgx = self.lightX.value
        lgy = self.lightY.value
        if lgx and lgy:
            self.ctrl.adj.x.vcmCenter.set(lgx)
            self.ctrl.adj.y.vcmCenter.set(lgy)


if __name__ == "__main__":
    app = wx.App(0)
    frame = MainWindow()
    app.MainLoop()

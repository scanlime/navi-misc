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
        vbox.Add(LaserWidgets.ValueGrid(self, self.beamParams.items + [
                    ]), 1, wx.EXPAND | wx.ALL, 2)
        
        buttons = LaserObjects.AdjustableValue()
        LaserWidgets.PollingBTConnector(buttons, self.ctrl.bt, "nes", 0)
        vbox.Add(LaserWidgets.ValueLabel(self, buttons, "buttons", "%02x"))

        trigger = LaserObjects.AdjustableValue()
        LaserWidgets.PollingBTConnector(trigger, self.ctrl.bt, "nes", 1)
        vbox.Add(LaserWidgets.ValueLabel(self, trigger, "trigger_cnt", "%02x"))

        light = LaserObjects.AdjustableValue()
        LaserWidgets.PollingBTConnector(light, self.ctrl.bt, "nes", 2)
        vbox.Add(LaserWidgets.ValueLabel(self, light, "light_cnt", "%02x"))

        lightX = LaserObjects.AdjustableValue()
        lightY = LaserObjects.AdjustableValue()
        LaserWidgets.PollingBTConnector(lightX, self.ctrl.bt, "nes", 3)
        LaserWidgets.PollingBTConnector(lightY, self.ctrl.bt, "nes", 4)
        vbox.Add(LaserWidgets.ValueLabel(self, lightX, "light_x", "%08x"))
        vbox.Add(LaserWidgets.ValueLabel(self, lightY, "light_y", "%08x"))

        self.lightX = LaserWidgets.CalibratedPositionValue(lightX, self.ctrl.adj.calibration, 'x')
        self.lightY = LaserWidgets.CalibratedPositionValue(lightY, self.ctrl.adj.calibration, 'y')        
        self.lightX.observe(self.followLightGun)

        self.SetSizer(vbox)
        self.SetAutoLayout(1)
        vbox.Fit(self)
        self.Show()

    def onClose(self, event):
        self.ctrl.close()
        self.Destroy()

    def redraw(self, _=None):
        en = VectorMachine.BeamAwareEncoder(self.beamParams)
        en.spiral(0, 0, 20, 30, 5)
        en.close()
        self.fb.replace(en.inst)

    def followLightGun(self, _=None):
        lgx = self.lightX.value
        lgy = self.lightY.value
        cx = self.ctrl.adj.x.vcmCenter.value
        cy = self.ctrl.adj.y.vcmCenter.value
        print (lgx, lgy), (cx, cy)
        if lgx and lgy:

            cx += (lgx - cx) * 0.5
            cy += (lgy - cy) * 0.5

            self.ctrl.adj.x.vcmCenter.set(cx)
            self.ctrl.adj.y.vcmCenter.set(cy)


if __name__ == "__main__":
    app = wx.App(0)
    frame = MainWindow()
    app.MainLoop()

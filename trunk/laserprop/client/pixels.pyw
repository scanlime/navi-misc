import LaserWidgets
import VectorMachine
import wx
import math
import time


class MainWindow(wx.Dialog):                 
    image = ['  *     *  ',
             '*  *   *  *',
             '* ******* *',
             '*** *** ***',
             '***********',
             ' ********* ',
             '  *     *  ',
             ' *       * ']

    xRaster = range(len(image[0]))
    yRaster = range(len(image))

    def __init__(self, parent=None, id=wx.ID_ANY, title="LaserProp Pixels"):
        wx.Dialog.__init__(self, parent, id, title)
        vbox = wx.BoxSizer(wx.VERTICAL)

        self.ctrl = LaserWidgets.LaserController(self)
        vbox.Add(self.ctrl, 0, wx.EXPAND)

        self.beamParams = VectorMachine.BeamParams(self.ctrl.serializer)
	vbox.Add(LaserWidgets.ValueGrid(self, self.beamParams.items), 0, wx.EXPAND | wx.ALL, 2)

        self.beamParams.speed.set(53325)
        self.beamParams.preStartEmphasis.set(35)
        self.beamParams.postStartEmphasis.set(0)
        self.beamParams.preBlankEmphasis.set(70)

        self.fb = VectorMachine.FrameBuffer(self.ctrl.bt)
	self.fb.setEmptyQueueCallback(self.animate)

        self.Bind(wx.EVT_CLOSE, self.onClose)
        
        self.SetSizer(vbox)
        self.SetAutoLayout(1)
        vbox.Fit(self)
        self.Show()

    def onClose(self, event):
        self.ctrl.close()
        self.Destroy()

    def animate(self):
        en = VectorMachine.BeamAwareEncoder(self.beamParams)
        
        self.xRaster.reverse()
        for x in self.xRaster:
            self.yRaster.reverse()
            for y in self.yRaster:
                if self.image[y][x] == '*':
                    en.moveTo(x*10, y*10)

        return en.inst


if __name__ == "__main__":
    app = wx.App(0)
    frame = MainWindow()
    app.MainLoop()

#!/usr/bin/env python
from BZEngine.UI import Viewport, ThreeDRender, ThreeDControl, Drawable, Sequencer
from BZEngine import Event, Util
import cPickle

loop = Event.EventLoop()
viewport = Viewport.OpenGLViewport(loop)
viewport.setCaption("Wasabi")
view = ThreeDRender.View(viewport)
control = ThreeDControl.Viewing(view, viewport)


class Monkey(Sequencer.Page):
    def __init__(self, view):
        Sequencer.Page.__init__(self, view)

        self.meshes = tuple(Drawable.VRML.load("monkey.wrl").values())
        self.view.scene.add(self.meshes)

        self.view.camera.position = (0,0,0)
        self.view.camera.distance = 73
        self.view.camera.azimuth = 210
        self.view.camera.elevation = -2
        self.view.camera.jump()

        self.viewport.onSetupFrame.observe(self.setupFrame)

    def setupFrame(self):
        dt = self.time.step()
        self.view.camera.azimuth += dt * 80

    def finalize(self):
        self.view.scene.remove(self.meshes)


class Sparks(Sequencer.Page):
    def __init__(self, view):
        Sequencer.Page.__init__(self, view)

        self.drawables = tuple([cPickle.load(open(Util.dataFile(name))) for name in
                                ('smoke.particle', 'welding_sparks.particle')])
        self.view.scene.add(self.drawables)

        self.view.camera.position = (0,0,5)
        self.view.camera.distance = 17
        self.view.camera.azimuth = 0
        self.view.camera.elevation = 0
        self.view.camera.jump()

    def finalize(self):
        self.view.scene.remove(self.drawables)


class TextureTest(Sequencer.Page):
    def __init__(self, view):
        Sequencer.Page.__init__(self, view)

        self.meshes = tuple(Drawable.VRML.load("texture_test.wrl").values())
        self.view.scene.add(self.meshes)

        self.view.camera.position = (-5,-0.337,0)
        self.view.camera.distance = 50
        self.view.camera.azimuth = 57
        self.view.camera.elevation = 29
        self.view.camera.jump()

        self.viewport.onSetupFrame.observe(self.setupFrame)

    def setupFrame(self):
        dt = self.time.step()
        self.view.camera.distance -= dt * 8
        if self.view.camera.distance < 7:
            self.onFinish()

    def finalize(self):
        self.view.scene.remove(self.meshes)


book = Sequencer.Book(view, [TextureTest, Monkey])
loop.run()

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
    def __init__(self, book):
        Sequencer.Page.__init__(self, book)

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
    def __init__(self, book):
        Sequencer.Page.__init__(self, book)

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
    """A scene featuring the texture_test model. The camera zooms in toward one
       of the crates, then the scene ends
       """
    def __init__(self, book):
        Sequencer.Page.__init__(self, book)

        # Load the 'texture_test' model into the scene
        self.meshes = tuple(Drawable.VRML.load("texture_test.wrl").values())
        self.view.scene.add(self.meshes)

        # Place the camera somewhat far away, centered on one of the crates
        self.view.camera.position = (-5,-0.337,0)
        self.view.camera.distance = 50
        self.view.camera.azimuth = 57
        self.view.camera.elevation = 29
        self.view.camera.jump()

        self.viewport.onSetupFrame.observe(self.setupFrame)

    def setupFrame(self):
        # Move the camera in closer
        dt = self.time.step()
        self.view.camera.distance -= dt * 8

        # Once the camera is too close to the crate, declare this scene finished
        if self.view.camera.distance < 8:
            self.onFinish()

    def finalize(self):
        self.view.scene.remove(self.meshes)


# Load a sequence of the above pages into a book
book = Sequencer.Book(view, [

    # Zoom in on the 'texture_test' model. This page finishes itself when the camera is close enough.
    # We fade in from black slowly, and fade out to white quickly
    Sequencer.FadeIn(3, (0,0,0), Sequencer.FadeOut(0.1, (1,1,1), TextureTest)),

    # Show some perty particle systems for 5 seconds, using the PageTimer
    # Fade in from white quickly, fade out to white quickly.
    Sequencer.FadeIn(0.1, (1,1,1), Sequencer.FadeOut(0.1, (1,1,1), Sequencer.PageTimer(5, Sparks))),

    # Spin the 'monkey' model until any key or mouse button is pressed.
    # Also fade in from white quickly, fade out to black slowly.
    Sequencer.FadeIn(0.1, (1,1,1), Sequencer.FadeOut(1, (0,0,0), Sequencer.PageInterrupter([
    viewport.onKeyDown, viewport.onMouseButtonDown], Monkey))),
    ])

# Run our event loop until the book finishes
book.onFinish.observe(loop.stop)
loop.run()

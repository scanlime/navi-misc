#!/usr/bin/env python
from BZEngine.UI import Viewport, ThreeDRender, ThreeDControl, Drawable, Sequencer, HUD
from BZEngine import Event, Util, Animated
import cPickle

loop = Event.EventLoop()
viewport = Viewport.OpenGLViewport(loop)
viewport.setCaption("Wasabi")
view = ThreeDRender.View(viewport)
control = ThreeDControl.Viewing(view, viewport)


class SpinningModel(Sequencer.Page):
    """A generic sequencer page that shows a spinning VRML model"""
    def __init__(self, book, modelName,
                 cameraDistance  = 70,
                 cameraAzimuth   = 270,
                 cameraElevation = 0,
                 azimuthSpeed    = 80,
                 elevationSpeed  = 0
                 ):
        Sequencer.Page.__init__(self, book)
        self.azimuthSpeed = azimuthSpeed
        self.elevationSpeed = elevationSpeed

        self.meshes = Drawable.VRML.load(modelName).values()
        self.view.scene.add(self.meshes)

        self.view.camera.position = (0,0,0)
        self.view.camera.distance = cameraDistance
        self.view.camera.azimuth = cameraAzimuth
        self.view.camera.elevation = cameraElevation
        self.view.camera.jump()

        self.viewport.onSetupFrame.observe(self.setupFrame)

    def setupFrame(self):
        dt = self.time.step()
        self.view.camera.azimuth += dt * self.azimuthSpeed
        self.view.camera.elevation += dt * self.elevationSpeed

    def finalize(self):
        self.view.scene.remove(self.meshes)


class Sparks(Sequencer.Page):
    """A page showing two particle systems generating sparks and smoke"""
    def __init__(self, book):
        Sequencer.Page.__init__(self, book)

        self.drawables = [cPickle.load(open(Util.dataFile(name))) for name in
                          ('smoke.particle', 'welding_sparks.particle')]
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

        self.meshes = Drawable.VRML.load("texture_test.wrl").values()
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
        self.view.camera.distance -= dt * 40

        # Once the camera is too close to the crate, declare this scene finished
        if self.view.camera.distance < 5:
            self.onFinish()

    def finalize(self):
        self.view.scene.remove(self.meshes)


class Warp(Sequencer.Page):
    """A warp-like particle system, performing integration steps during
       initialization to start in the middle of the animation.
       """
    def __init__(self, book):
        Sequencer.Page.__init__(self, book)

        # Load the particle system, and simulate it for a few time steps
        # so we jump right into the middle of an active animation rather than
        # having to wait for the particles to start flowing.
        self.particle = cPickle.load(open(Util.dataFile('warp.particle')))
        for step in xrange(50):
            self.particle.model.integrate(0.05)
        self.view.scene.add(self.particle)

        self.view.camera.position = (0,0,0)
        self.view.camera.distance = 20
        self.view.camera.azimuth = 0
        self.view.camera.elevation = 0
        self.view.camera.jump()

    def finalize(self):
        self.view.scene.remove(self.particle)


class Preloader(Sequencer.Page):
    """A page that displays a 'Loading' message while caching VRML models"""
    def __init__(self, book, modelList):
        Sequencer.Page.__init__(self, book)
        self.modelList = modelList

        self.message = HUD.Text(self.viewport.region(self.viewport.rect),
                              "Loading...",
                              color     = (1,1,1,1),
                              fontSize  = 50,
                              alignment = (0.5, 0.5))

        self.frameCounter = 0
        self.viewport.onSetupFrame.observe(self.setupFrame)

    def setupFrame(self):
        self.frameCounter += 1

        # Actually do the precaching after one complete frame, so our message is displayed
        if self.frameCounter == 2:
            for model in self.modelList:
                Drawable.VRML.load(model)

            # Absorb the gigantic bubble we've just put in the TimeMaster so that it doesn't
            # cause animations the next frame to jump wildly.
            Animated.currentTimeMaster.jump()

            # Terminate this page
            self.onFinish()


# Define some common transitions
fromWhiteFast = lambda page: Sequencer.FadeIn(0.1, (1,1,1), page)
toWhiteFast   = lambda page: Sequencer.FadeOut(0.1, (1,1,1), page)
whiteFast     = lambda page: fromWhiteFast(toWhiteFast(page))
toBlackSlow   = lambda page: Sequencer.FadeOut(2, (0,0,0), page)
fromBlackSlow = lambda page: Sequencer.FadeIn(2, (0,0,0), page)
blackSlow     = lambda page: fromBlackSlow(toBlackSlow(page))

# Specific versions of the general SpinningModel page
SpinningMonkey = lambda page: SpinningModel(page, 'monkey.wrl')
SpinningBox    = lambda page: SpinningModel(page, 'metal_box.wrl',
                                            cameraDistance  = 6,
                                            cameraAzimuth   = 20,
                                            cameraElevation = 40,
                                            azimuthSpeed    = 20,
                                            elevationSpeed  = -10)

# A looping SubBook, cycling between TextureTest, Warp, and MetalBox
zoomSparkCycle = Sequencer.SubBook(Sequencer.CyclicBook, [
    whiteFast(TextureTest),
    whiteFast(Sequencer.PageTimer(2, Warp)),
    whiteFast(Sequencer.PageTimer(2, SpinningBox)),
    ])

# Our main book, running only once
mainBook = Sequencer.Book(view, [
    # Preload VRML models
    lambda page: Preloader(page, ('monkey.wrl', 'metal_box.wrl')),

    # Show some perty particle systems for 4 seconds, using the PageTimer
    fromBlackSlow(toWhiteFast(Sequencer.PageTimer(4, Sparks))),

    # Execute the zoomSparkCycle book until user input, then fade to black slowly
    toBlackSlow(Sequencer.UserPageInterrupter(zoomSparkCycle)),

    # Spin the 'monkey' model until any key or mouse button is pressed.
    blackSlow(Sequencer.UserPageInterrupter(SpinningMonkey)),
    ])

# Exit our main loop once the mainBook finishes
mainBook.onFinish.observe(loop.stop)
loop.run()

#!/usr/bin/env python
from BZEngine.UI import Viewport, ThreeDRender, ThreeDControl, HUD
from BZEngine import Event, Animated, Geometry, Noise
import cPickle, copy
from Numeric import *

loop = Event.EventLoop()
viewport = Viewport.OpenGLViewport(loop)
view = ThreeDRender.View(viewport)
control = ThreeDControl.Viewing(view, viewport)
viewport.setCaption("Wasabi")

viewport.mode = Viewport.GL.ClearedMode(clearColor=(0, 0, 0, 1))

view.camera.position = (0, 0, 0)
view.camera.distance = 10
view.camera.azimuth = 0
view.camera.elevation = 0
view.camera.jump()

title = HUD.Text(viewport.region(viewport.rect), "wasabi",
                 color     = (0, 0, 0, 0.75),
                 fontSize  = viewport.size[1] / 6,
                 alignment = (0.5, 0.5),
                 fontName  = "geodesic.ttf")


class OrbitingParticles:
    """A particle system orbiting around the origin"""
    def __init__(self, radius=2, speed=2, randomness=3):
        self.particle = cPickle.load(open("data/green_nebula.particle"))
        (self.center, j) = Noise.randomVectors((2,3), magnitude=radius)
        self.velocity = Geometry.cross(self.center, j)
        self.speed = speed
        self.randomness = randomness

    def getDrawables(self):
        return [self.particle]

    def integrate(self, dt):
        self.center += self.velocity * dt * self.speed
        self.velocity -= self.center * dt * self.speed
        self.velocity += Noise.randomVectors((1,3), magnitude=self.randomness * dt)[0]
        self.particle.emitter.position = self.center


orbits = [OrbitingParticles() for i in xrange(2)]
for orbit in orbits:
    view.scene.add(orbit)

time = Animated.Timekeeper()
def setupFrame():
    dt = time.step()
    for orbit in orbits:
        orbit.integrate(dt)

viewport.onSetupFrame.observe(setupFrame)
loop.run()

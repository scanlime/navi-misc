#!/usr/bin/env python
from BZEngine.UI import Viewport, ThreeDRender, ThreeDControl, HUD
from BZEngine import Event, Animated, Geometry, Util
import cPickle, copy
from Numeric import *

loop = Event.EventLoop()
viewport = Viewport.OpenGLViewport(loop)
viewport.setCaption("Wasabi")
view = ThreeDRender.View(viewport)
control = ThreeDControl.Viewing(view, viewport)


class OrbitingParticles:
    """A particle system orbiting around the origin"""
    def __init__(self, particle, radius=3, rotationPeriod=1, noisePeriod=10, seed=0):
        self.particle = particle
        self.plane = Animated.Vector(Animated.PerlinNoise(frequency = 1.0 / noisePeriod))
        self.angle = Animated.Value(Animated.RampFunction(rotationPeriod, (0, 2*math.pi)))
        self.radius = radius

    def getDrawables(self):
        return [self.particle]

    def integrate(self, dt):
        self.angle.integrate(dt)
        self.plane.integrate(dt)

        # Create two perpendicular vectors on our orbit plane, and use those
        # and our angle to calculate the emitter position
        plane = self.plane.get()
        v1 = Geometry.normalize(Geometry.cross(plane, (1,0,0)))
        v2 = Geometry.normalize(Geometry.cross(plane, v1))
        self.particle.emitter.position = (cos(self.angle.value)*v1 + sin(self.angle.value)*v2) * self.radius


class Scene:
    """A collection of scene objects and stuff"""

class WasabiTitle:
    def __init__(self, view):
        self.viewport = view.viewport
        self.view = view
        self.time = Animated.Timekeeper()

        self.viewport.mode = Viewport.GL.ClearedMode(clearColor=(0, 0, 0, 1))

        self.view.camera.position = (0.0314, -3, 0)
        self.view.camera.distance = 4.49
        self.view.camera.azimuth = 3
        self.view.camera.elevation = 2.4
        self.view.camera.jump()

        self.title = HUD.Text(self.viewport.region(self.viewport.rect),
                              "wasabi",
                              color     = (0, 0, 0, 1),
                              fontSize  = self.viewport.size[1] / 5,
                              alignment = (0.5, 0.5),
                              fontName  = "geodesic.ttf")

        orbitParticles = ("green_flame.particle", "green_nebula.particle")
        self.orbits = [OrbitingParticles(cPickle.load(open(Util.dataFile(name)))) for name in orbitParticles]
        for orbit in self.orbits:
            self.view.scene.add(orbit)
        self.viewport.onSetupFrame.observe(self.setupFrame)

    def setupFrame(self):
        dt = self.time.step()
        for orbit in self.orbits:
            orbit.integrate(dt)


foo = WasabiTitle(view)
loop.run()

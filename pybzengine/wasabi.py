#!/usr/bin/env python
from BZEngine.UI import Viewport, ThreeDRender, ThreeDControl, HUD
from BZEngine import Event, Animated, Geometry, Util
import cPickle, copy
from Numeric import *

loop = Event.EventLoop()
viewport = Viewport.OpenGLViewport(loop)
view = ThreeDRender.View(viewport)
control = ThreeDControl.Viewing(view, viewport)
viewport.setCaption("Wasabi")

viewport.mode = Viewport.GL.ClearedMode(clearColor=(0, 0, 0, 1))

view.camera.position = (0.0314, -3, 0)
view.camera.distance = 4.49
view.camera.azimuth = 3
view.camera.elevation = 2.4
view.camera.jump()

title = HUD.Text(viewport.region(viewport.rect), "wasabi",
                 color     = (0, 0, 0, 0.75),
                 fontSize  = viewport.size[1] / 6,
                 alignment = (0.5, 0.5),
                 fontName  = "geodesic.ttf")


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


orbitParticles = ("green_flame.particle", "green_nebula.particle")
orbits = [OrbitingParticles(cPickle.load(open(Util.dataFile(name)))) for name in orbitParticles]
for orbit in orbits:
    view.scene.add(orbit)

view.scene.add(cPickle.load(open(Util.dataFile("galaxy.particle"))))


time = Animated.Timekeeper()
def setupFrame():
    dt = time.step()
    for orbit in orbits:
        orbit.integrate(dt)

viewport.onSetupFrame.observe(setupFrame)
loop.run()

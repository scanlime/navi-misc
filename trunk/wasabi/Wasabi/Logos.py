""" Wasabi.Logos

At the highest level, this module creates a Sequence.Book with
a set of Wasabi logos. Below this it provides Page subclasses
implementing each logo, and utility classes needed to implement
these logos.
"""
#
# Wasabi Project
# Copyright (C) 2003-2004 Micah Dowty <micahjd@users.sourceforge.net>
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

from BZEngine.UI import Sequencer, Viewport, HUD, Drawable
from BZEngine import Util, Animated, Geometry
import cPickle, math, Hardware
from Numeric import *


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


class OrbitingLogo(Sequencer.Page):
    """Base class for logos based on a background of orbiting green particle systems"""
    def __init__(self, view):
        Sequencer.Page.__init__(self, view)

        Hardware.dev.showStatus("Wasabi")

        self.view.camera.position = (0, 0, 0)
        self.view.camera.distance = 7.5
        self.view.camera.azimuth = 0
        self.view.camera.elevation = 0
        self.view.camera.jump()

        # We need the viewport to clear our background to black
        self.viewport.mode = Viewport.GL.ClearedMode()

        orbitParticles = ("green_flame.particle", "green_nebula.particle")
        self.orbits = [OrbitingParticles(cPickle.load(open(Util.dataFile(name)))) for name in orbitParticles]
        for orbit in self.orbits:
            self.view.scene.add(orbit)
        self.viewport.onSetupFrame.observe(self.setupFrame)

    def setupFrame(self):
        dt = self.time.step()
        for orbit in self.orbits:
            orbit.integrate(dt)

    def finalize(self):
        for orbit in self.orbits:
            self.view.scene.remove(orbit)


class EnglishLogo(OrbitingLogo):
    def __init__(self, view):
        OrbitingLogo.__init__(self, view)
        self.title = HUD.Text(self.viewport.region(self.viewport.rect),
                              "wasabi",
                              color     = (0, 0, 0, 1),
                              fontSize  = self.viewport.size[1] / 5,
                              alignment = (0.5, 0.5),
                              fontName  = "geodesic.ttf")


class JapaneseLogo(OrbitingLogo):
    def __init__(self, view):
        OrbitingLogo.__init__(self, view)
        height = self.viewport.size[1] / 2
        self.title = HUD.Image(self.viewport.region(self.viewport.rect),
                               "wasabi_hiragana.png",
                               (height * 0.306358, height),
                               color = (0,0,0,1),
                               alignment = (0.5, 0.5))


def getLogoList():
    """Returns a list of factory functions for all the logos"""

    # Define some common transitions
    toBlack   = lambda page: Sequencer.FadeOut(2, (0,0,0), page)
    fromBlack = lambda page: Sequencer.FadeIn(2, (0,0,0), page)
    fade      = lambda page: fromBlack(toBlack(page))

    return [
        fade(Sequencer.PageTimer(60, JapaneseLogo)),
        fade(Sequencer.PageTimer(60, EnglishLogo)),
        ]


def getLogoSubBook():
    """Returns a page-compatible SubBook containing all the logos"""
    return Sequencer.SubBook(Sequencer.CyclicBook, getLogoList())

### The End ###

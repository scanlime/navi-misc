""" BZFlag.UI.Drawable.Sky

Drawables used in creating PyBZFlag's sky. These drawables are all
used in BZFlag.UI.Environment.Sky
"""
#
# Python BZFlag Package
# Copyright (C) 2003 Micah Dowty <micahjd@users.sourceforge.net>
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

from __future__ import division
from DisplayList import DisplayList
from OpenGL.GL import *
from OpenGL.GLU import *
from BZFlag import Animated, Noise
from BZFlag.UI import GLNoise, Texture
import VRML, math
from Numeric import *
from OpenGL.GL.ARB.multitexture import *


class SkyDrawable(DisplayList):
    """Drawable functionality common to all elements of the sky"""
    def set(self, sky):
        self.sky = sky

        # We render in a z-buffer-isolated pass before all other geometry.
        # This means that we can work with the sky on a scale independent of the rest of the world.
        self.render.background = True

        # The mesh itself is in a display list, but this drawable needs to update the texture
        # coordinates each frame, so don't cache this drawable in the big render pass display lists
        self.render.static = False


class ColoredSkyMesh(SkyDrawable):
    """A SkyDrawable with colors animated using one track from the sky_colors texture.
       The colorTrack values should be replaced by subclasses with the position of the
       color track in texels. Fractional texels can and should be used to account for
       the linear interpolation overshoot.
       The X axis of the sky color is temporal, indicating the time of day.
       The Y axis of the sky color maps from 0 to 1 on the model.
       """
    textureName      = 'sky_colors.png'
    colorTrackOrigin = None
    colorTrackHeight = None
    meshName         = None

    def drawToList(self, rstate):
        """The display list should hold everything except the time-dependent
           texGen plane equation.
           """
        glTexGenfv(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR)
        glTexGenfv(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR)
        glTexGenfv(GL_T, GL_OBJECT_PLANE, (0, 0,
                                           self.colorTrackHeight / self.render.textures[0].size[1],
                                           self.colorTrackOrigin / self.render.textures[0].size[1]))
        glEnable(GL_TEXTURE_GEN_S)
        glEnable(GL_TEXTURE_GEN_T)

        VRML.load('sky.wrl')[self.meshName].drawToList(rstate)

        glDisable(GL_TEXTURE_GEN_S)
        glDisable(GL_TEXTURE_GEN_T)

    def draw(self, rstate):
        glTexGenfv(GL_S, GL_OBJECT_PLANE, (0, 0, 0, self.sky.unitDayTime))
        DisplayList.draw(self, rstate)


class Colors(ColoredSkyMesh):
    """The sky itself, with colors changing over the course of the day"""
    colorTrackOrigin = 32.5
    colorTrackHeight = 30
    meshName         = 'dome'


class CloudTexture(GLNoise.MappedPerlinTexture):
    """Perlin noise with mapping applied to create a cloud-like texture"""
    def map(self, y):
        try:
            return math.log((y-0.35) * 40 + 1) * 0.27
        except ValueError:
            return 0


class Clouds(ColoredSkyMesh):
    """Dynamic perlin-noise-based clouds"""
    colorTrackOrigin = 13
    colorTrackHeight = 1
    meshName         = 'dome'

    def __init__(self, *args, **kw):
        ColoredSkyMesh.__init__(self, *args, **kw)
        self.render.textures += (CloudTexture(),)
        self.motion = Animated.Value(Animated.RampFunction(1000))
        self.time = Animated.Timekeeper()
        self.scale = 0.2

    def drawToList(self, rstate):
        glEnable(GL_BLEND)
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA)

        # Set up texture coordinate generation. The plane equations
        # are set up every frame in draw() to animate the clouds' motion
        glActiveTextureARB(GL_TEXTURE1_ARB)
        glTexGenfv(GL_T, GL_OBJECT_PLANE, (0, self.scale, 0, 0))
        glTexGenfv(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR)
        glTexGenfv(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR)
        glEnable(GL_TEXTURE_GEN_S)
        glEnable(GL_TEXTURE_GEN_T)
        glActiveTextureARB(GL_TEXTURE0_ARB)

        # Do the usual ColoredSkyMesh rendering
        ColoredSkyMesh.drawToList(self, rstate)

        # Cleanup!
        glActiveTextureARB(GL_TEXTURE1_ARB)
        glDisable(GL_TEXTURE_GEN_S)
        glDisable(GL_TEXTURE_GEN_T)
        glActiveTextureARB(GL_TEXTURE0_ARB)
        glDisable(GL_BLEND)
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)

    def draw(self, rstate):
        """Animate the texture coordinates every frame"""
        self.motion.integrate(self.time.step())
        glActiveTextureARB(GL_TEXTURE1_ARB)
        glTexGenfv(GL_S, GL_OBJECT_PLANE, (self.scale, 0, 0, self.motion.value))
        glActiveTextureARB(GL_TEXTURE0_ARB)
        DisplayList.draw(self, rstate)


class MountainTexture(Texture.DynamicTexture):
    """Perlin-noise-based mountain range silhouette"""
    def __init__(self, size=(512,128), noise=None):
        if not noise:
            noise = Noise.PerlinNoise(octaves=5, fundamental=10)
        self.noise = noise
        Texture.DynamicTexture.__init__(self, size)
        self.setRepeat(GL_REPEAT, GL_CLAMP)
        self.format = GL_INTENSITY8

    def draw(self):
        # To get the most quality for our texel, sample the perlin noise
        # at twice our texture's horizontal resolution.
        samples = arange(0, 1, 1/(self.viewport.size[0] * 2))
        heights = self.noise.get(samples)

        glClearColor(0,0,0,1)
        glClear(GL_COLOR_BUFFER_BIT)
        glDisable(GL_DEPTH_TEST)
        glDisable(GL_CULL_FACE)
        glDisable(GL_LIGHTING)
        glEnable(GL_BLEND)
        glEnable(GL_LINE_SMOOTH)
        glDisable(GL_TEXTURE_2D)
        glColor3f(1,1,1)

        glBegin(GL_LINES)
        for i in xrange(len(samples)):
            x = samples[i] * self.viewport.size[0]
            glVertex2f(x, (-heights[i]/2 +0.5) * self.viewport.size[1])
            glVertex2f(x, self.viewport.size[1])
        glEnd()

        glDisable(GL_BLEND)
        glDisable(GL_LINE_SMOOTH)


class Mountains(ColoredSkyMesh):
    """Some mountains and a chasm to cover up the horizon"""
    colorTrackOrigin = 24.5
    colorTrackHeight = 7
    meshName         = 'horizon'

    def __init__(self, *args, **kw):
        SkyDrawable.__init__(self, *args, **kw)
        self.render.textures += (MountainTexture(),)

    def drawToList(self, rstate):
        """Do everything except the frame-dependent texgen setup in the display list"""
        glEnable(GL_BLEND)
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA)

        # Set up texture coordinates for the mountain silhouette.
        # We can't do cylindrical mapping with glTexGen, and we don't have
        # texture coordinates in the VRML loader yet, so just fudge
        # something reasonableish with linear mapping.
        glActiveTextureARB(GL_TEXTURE1_ARB)
        glTexGenfv(GL_S, GL_OBJECT_PLANE, (1, 0, 0, 0))
        glTexGenfv(GL_T, GL_OBJECT_PLANE, (0, 0, 1, 0))
        glTexGenfv(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR)
        glTexGenfv(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR)
        glEnable(GL_TEXTURE_GEN_S)
        glEnable(GL_TEXTURE_GEN_T)
        glActiveTextureARB(GL_TEXTURE0_ARB)

        # Do the usual ColoredSkyMesh rendering
        ColoredSkyMesh.drawToList(self, rstate)

        # Cleanup!
        glActiveTextureARB(GL_TEXTURE1_ARB)
        glDisable(GL_TEXTURE_GEN_S)
        glDisable(GL_TEXTURE_GEN_T)
        glActiveTextureARB(GL_TEXTURE0_ARB)
        glDisable(GL_BLEND)
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)


class Void(ColoredSkyMesh):
    """A whole lot of nothing, right below the mountains"""
    colorTrackOrigin = 16.5
    colorTrackHeight = 7
    meshName         = 'void'

### The End ###

""" BZFlag.UI.Drawable.Pyramid

A class for drawing the pyramids in the world
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
from DisplayList import *
from OpenGL.GL import *
from BZFlag.UI import CubeMap, GLExtension
from OpenGL.GL.ARB.multitexture import *
from BZFlag.Geometry import *
from Numeric import *


class Pyramid(DisplayList):
    """A default pyramid, with XY planar texture mapping and a marble texture"""
    textureName = "black_marble.jpeg"

    def __init__(self, pyramid):
        self.uvMap = self.getUVMap(pyramid)
        self.uvMap2 = self.getUVMap2(pyramid)
        DisplayList.__init__(self, pyramid)

    def getUVMap(self, pyramid):
        """Default UV map: XY planar"""
        repeats = (pyramid.size[0] / 4,
                   pyramid.size[1] / 4)
        return ((0, 0),
                (0, repeats[1]),
                repeats,
                (repeats[0], 0),

                (repeats[0], 0),
                repeats,
                (repeats[0]/2, repeats[1]/2),

                repeats,
                (0, repeats[1]),
                (repeats[0]/2, repeats[1]/2),

                (0, repeats[1]),
                (0, 0),
                (repeats[0]/2, repeats[1]/2),

                (0, 0),
                (repeats[0], 0),
                (repeats[0]/2, repeats[1]/2))

    def getUVMap2(self, pyramid):
        """Returns the UV map for the second texture. This defaults to the same as the first texture"""
        return self.getUVMap(pyramid)

    def set(self, pyramid):
        self.pyramid = pyramid

    def drawToList(self, rstate):
        poly = self.pyramid.toPolygon()

        uvstack = list(self.uvMap)
        uvstack.reverse()
        uvstack2 = list(self.uvMap2)
        uvstack2.reverse()

        flip = 'flipZ' in self.pyramid.options
        if flip:
            poly.reverse()
            z2 = self.pyramid.center[2]
            z  = self.pyramid.center[2] + self.pyramid.size[2]
        else:
            z  = self.pyramid.center[2]
            z2 = self.pyramid.center[2] + self.pyramid.size[2]

        # Base
        glBegin(GL_QUADS)
        if flip:
            glNormal3f(0, 0, 1)
        else:
            glNormal3f(0, 0, -1)
        rpoly = poly[:]
        rpoly.reverse()
        for vertex in rpoly:
            glTexCoord2f(*uvstack.pop())
            glMultiTexCoord2fARB(GL_TEXTURE1_ARB, *uvstack2.pop())
            glVertex3f(vertex[0], vertex[1], z)
        glEnd()

        # Sides
        glBegin(GL_TRIANGLES)
        center = sum(array(poly)) / len(poly)
        poly.append(poly[0])
        for i in xrange(len(poly)-1):
            v1 = (poly[i][0], poly[i][1], z)
            v2 = (poly[i+1][0], poly[i+1][1], z)
            v3 = (center[0], center[1], z2)
            norm = normalize(cross(subtract(v2,v1), subtract(v3,v1)))

            glNormalf(*norm)
            glTexCoord2f(*uvstack.pop())
            glMultiTexCoord2fARB(GL_TEXTURE1_ARB, *uvstack2.pop())
            glVertex3f(*v1)
            glTexCoord2f(*uvstack.pop())
            glMultiTexCoord2fARB(GL_TEXTURE1_ARB, *uvstack2.pop())
            glVertex3f(*v2)
            glTexCoord2f(*uvstack.pop())
            glMultiTexCoord2fARB(GL_TEXTURE1_ARB, *uvstack2.pop())
            glVertex3f(*v3)
        glEnd()


class Pillar(Pyramid):
    """Alternate textures for very pointy pyramids"""
    textureName = 'pillar.jpeg'

    def getUVMap(self, pyramid):
        # This maps each side of the pyramid to a triangle based at the bottom
        # of the texture with its tip at the top-center. The bottom of the pyramid
        # is mapped to a the top-left 1/4 of the texture.
        return ((0,   3/4),
                (1/4, 3/4),
                (1/4, 1),
                (0,   1),

                (0, 0),
                (1, 0),
                (0.5, 1),

                (0, 0),
                (1, 0),
                (0.5, 1),

                (0, 0),
                (1, 0),
                (0.5, 1),

                (0, 0),
                (1, 0),
                (0.5, 1))



class Water(Pyramid):
    """Water pyramid, now with animation!"""
    textureNames = ('caustic%02d.jpeg:50.0', 'water.jpeg')

    def set(self, pyramid):
        Pyramid.set(self, pyramid)
        self.render.blended = True

    def drawToList(self, rstate):
        # Use texture coordinate generation to texture our pyramid in world coordinates
        causticScale = 20
        waterScale = 100
        glActiveTextureARB(GL_TEXTURE1_ARB)
        glTexGenfv(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR)
        glTexGenfv(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR)
        glTexGenfv(GL_S, GL_OBJECT_PLANE, (1/waterScale,0,0,0))
        glTexGenfv(GL_T, GL_OBJECT_PLANE, (0,1/waterScale,0,0))
        glEnable(GL_TEXTURE_GEN_S)
        glEnable(GL_TEXTURE_GEN_T)
        glActiveTextureARB(GL_TEXTURE0_ARB)
        glTexGenfv(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR)
        glTexGenfv(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR)
        glTexGenfv(GL_S, GL_OBJECT_PLANE, (1/causticScale,0,0,0))
        glTexGenfv(GL_T, GL_OBJECT_PLANE, (0,1/causticScale,0,0))
        glEnable(GL_TEXTURE_GEN_S)
        glEnable(GL_TEXTURE_GEN_T)

        glColor4f(1,1,1, 0.6)
        Pyramid.drawToList(self, rstate)
        glColor4f(1,1,1,1)

        glActiveTextureARB(GL_TEXTURE1_ARB)
        glDisable(GL_TEXTURE_GEN_S)
        glDisable(GL_TEXTURE_GEN_T)
        glActiveTextureARB(GL_TEXTURE0_ARB)
        glDisable(GL_TEXTURE_GEN_S)
        glDisable(GL_TEXTURE_GEN_T)


class Reflection(Pyramid):
    """A reflective surface applied on top of a normal pyramid"""
    def __init__(self, pyramid):
        mapCenter = add(pyramid.center, multiply((0,0,0.5), pyramid.size))
        self.textureName = CubeMap.CubeMap(mapCenter)
        Pyramid.__init__(self, pyramid)
        self.render.reflection = True

    def drawToList(self, rstate):
        glColor4f(1,1,1, 0.4)
        Pyramid.drawToList(self, rstate)
        glColor4f(1,1,1,1)


def detectPyramidDrawables(pyramid):
    """Given a pyramid WorldObject, return a list of the drawables that should be used
       to represent it. This looks at the size and placement of the pyramid to determine
       which drawables to use to represent it.
       """
    # Don't try to draw pyramids with a zero size, they will give is a divide by zero later
    if not (pyramid.size[0] and pyramid.size[1] and pyramid.size[2]):
        return []

    drawables = []
    
    # Calculate the slope, we'll use this to help decide how to draw the pyramid
    try:
        slope = pyramid.size[2] / min(pyramid.size[0], pyramid.size[1])
    except ZeroDivisionError:
        slope = 1e100

    if slope > 10:
        # It's a very pointy pyramid
        drawables.append(Pillar(pyramid))

    elif slope < 0.5 and pyramid.center[2] > 0:
        # It's very flat and not touching the ground. These are often used
        # to simulate water, so make them look that way too :)
        drawables.append(Water(pyramid))

    else:
        # Default pyramid, but with a shiny finish if we can support one
        drawables.append(Pyramid(pyramid))
        if GLExtension.multitexture and GLExtension.cubeMap:
            drawables.append(Reflection(pyramid))

    ## If it's on the ground and not too thin, add some grass poking up around it
    ## Commented out because this looks silly on pyramids
    #if pyramid.center[2] == 0 and pyramid.size[2] > 1:
    #    from Box import GrassEdge
    #    drawables.append(GrassEdge(pyramid))

    return drawables
        
### The End ###

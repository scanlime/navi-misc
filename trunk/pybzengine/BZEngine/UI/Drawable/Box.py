""" BZFlag.UI.Drawable.Box

Classes to draw the boxes in the world
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
from OpenGL.GL.ARB.multitexture import *
from BZFlag.UI import GLExtension
from BZFlag.World import Scale
from BZFlag.Geometry import *
from Numeric import *
import random


class Sides(DisplayList):
    """Abstract base class for box sides"""
    tex2Repeats = None

    def set(self, box):
        self.center = box.center
        self.angle = box.angle
        self.size = box.size
        try:
            self.render.textures[1].texEnv = GL_MODULATE
        except IndexError:
            pass

    def drawToList(self, rstate):
        glPushMatrix()
        glTranslatef(*self.center)
        glRotatef(self.angle, 0.0, 0.0, 1.0)
        glBegin(GL_QUADS)
        # Y+ side
        glNormal3f(0, 1, 0)
        glTexCoord2f(0, 0)
        if self.tex2Repeats:
            glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0, 0)
        glVertex3f(-self.size[0], self.size[1], 0)
        glTexCoord2f(0, self.texRepeats[2])
        if self.tex2Repeats:
            glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0, self.tex2Repeats[2])
        glVertex3f(-self.size[0], self.size[1], self.size[2])
        glTexCoord2f(self.texRepeats[0], self.texRepeats[2])
        if self.tex2Repeats:
            glMultiTexCoord2fARB(GL_TEXTURE1_ARB, self.tex2Repeats[0], self.tex2Repeats[2])
        glVertex3f(self.size[0], self.size[1], self.size[2])
        glTexCoord2f(self.texRepeats[0], 0)
        if self.tex2Repeats:
            glMultiTexCoord2fARB(GL_TEXTURE1_ARB, self.tex2Repeats[0], 0)
        glVertex3f(self.size[0], self.size[1], 0)
        # Y- side
        glNormal3f(0, -1, 0)
        glTexCoord2f(0, 0)
        if self.tex2Repeats:
            glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0, 0)
        glVertex3f(self.size[0], -self.size[1], 0)
        glTexCoord2f(0, self.texRepeats[2])
        if self.tex2Repeats:
            glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0, self.tex2Repeats[2])
        glVertex3f(self.size[0], -self.size[1], self.size[2])
        glTexCoord2f(self.texRepeats[0], self.texRepeats[2])
        if self.tex2Repeats:
            glMultiTexCoord2fARB(GL_TEXTURE1_ARB, self.tex2Repeats[0], self.tex2Repeats[2])
        glVertex3f(-self.size[0], -self.size[1], self.size[2])
        glTexCoord2f(self.texRepeats[0], 0)
        if self.tex2Repeats:
            glMultiTexCoord2fARB(GL_TEXTURE1_ARB, self.tex2Repeats[0], 0)
        glVertex3f(-self.size[0], -self.size[1], 0)
        # X+ side
        glNormal3f(1, 0, 0)
        glTexCoord2f(0, 0)
        if self.tex2Repeats:
            glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0, 0)
        glVertex3f(self.size[0], self.size[1], 0)
        glTexCoord2f(0, self.texRepeats[2])
        if self.tex2Repeats:
            glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0, self.tex2Repeats[2])
        glVertex3f(self.size[0], self.size[1], self.size[2])
        glTexCoord2f(self.texRepeats[1], self.texRepeats[2])
        if self.tex2Repeats:
            glMultiTexCoord2fARB(GL_TEXTURE1_ARB, self.tex2Repeats[1], self.tex2Repeats[2])
        glVertex3f(self.size[0], -self.size[1], self.size[2])
        glTexCoord2f(self.texRepeats[1], 0)
        if self.tex2Repeats:
            glMultiTexCoord2fARB(GL_TEXTURE1_ARB, self.tex2Repeats[1], 0)
        glVertex3f(self.size[0], -self.size[1], 0)
        # X- side
        glNormal3f(-1, 0, 0)
        glTexCoord2f(0, 0)
        if self.tex2Repeats:
            glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0, 0)
        glVertex3f(-self.size[0], -self.size[1], 0)
        glTexCoord2f(0, self.texRepeats[2])
        if self.tex2Repeats:
            glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0, self.tex2Repeats[2])
        glVertex3f(-self.size[0], -self.size[1], self.size[2])
        glTexCoord2f(self.texRepeats[1], self.texRepeats[2])
        if self.tex2Repeats:
            glMultiTexCoord2fARB(GL_TEXTURE1_ARB, self.tex2Repeats[1], self.tex2Repeats[2])
        glVertex3f(-self.size[0], self.size[1], self.size[2])
        glTexCoord2f(self.texRepeats[1], 0)
        if self.tex2Repeats:
            glMultiTexCoord2fARB(GL_TEXTURE1_ARB, self.tex2Repeats[1], 0)
        glVertex3f(-self.size[0], self.size[1], 0)
        glEnd()
        glPopMatrix()


class BrickSides(Sides):
    """Sides of a standard bzflag brick box"""
    textureNames = ("brick.jpeg", "wall_grime.jpeg")
    def __init__(self, box):
        # This is brick, so we want to make sure we don't end up
        # with a fractional number of bricks. This brick texture has
        # 5 bricks vertically and about 5 horizontally.
        numBricks = (int(box.size[0]),
                     int(box.size[1]),
                     int(box.size[2]))
        self.texRepeats = (numBricks[0] / 5,
                           numBricks[1] / 5,
                           numBricks[2] / 5)
        self.tex2Repeats = (box.size[0] / 20,
                            box.size[1] / 20,
                            1)
        Sides.__init__(self, box)


class MetalSides(Sides):
    """Sides of a square metal box"""
    textureName = "metal_box.jpeg"
    def __init__(self, box):
        self.texRepeats = (1,1,1)
        Sides.__init__(self, box)


class PlatformSides(Sides):
    """Sides of a thin platform box. This makes platforms look like two
       thin pieces of concrete sandwiching some lighter concrete, with
       bolts holding it all together. Looks a bit better than a very thin
       section of brick.
       """
    textureNames = ("platform_side.jpeg", "wall_grime.jpeg")
    def __init__(self, box):
        aspect = 8/10
        self.texRepeats = (box.size[0] / box.size[2] * 2 * aspect,
                           box.size[1] / box.size[2] * 2 * aspect,
                           1)
        self.tex2Repeats = (box.size[0] / 4,
                            box.size[1] / 4,
                            1)    
        Sides.__init__(self, box)


class End(DisplayList):
    """Abstract base class for the top and bottom sides of a box"""
    textureNames = ('concrete.jpeg',)
    texScale = 20
    tex2Scale = 300

    def drawToList(self, rstate):
        glPushMatrix()
        glTranslatef(0, 0, self.height)
        glNormal3f(0, 0, 1)
        glBegin(GL_POLYGON)
        for i in xrange(4):
            vertex = self.polygon[i]
            glTexCoord2f(vertex[0] / self.texScale, vertex[1] / self.texScale)
            glMultiTexCoord2fARB(GL_TEXTURE1_ARB, vertex[0] / self.tex2Scale, vertex[1] / self.tex2Scale)
            glVertex2f(*vertex)
        glEnd()
        glPopMatrix()


class Top(End):
    def set(self, box):
        self.polygon = box.toPolygon()
        self.height = box.center[2] + box.size[2]


class MetalEnd(DisplayList):
    """The top or bottom of a square metal box"""
    textureName = 'metal_box.jpeg'
    uvMap = ( (0,0), (1,0), (1,1), (0,1) )

    def drawToList(self, rstate):
        glPushMatrix()
        glTranslatef(0, 0, self.height)
        glNormal3f(0, 0, 1)
        glBegin(GL_POLYGON)
        for i in xrange(4):
            vertex = self.polygon[i]
            glTexCoord2f(*self.uvMap[i])
            glVertex2f(*vertex)
        glEnd()
        glPopMatrix()


class MetalTop(MetalEnd):
    def set(self, box):
        self.polygon = box.toPolygon()
        self.height = box.center[2] + box.size[2]


class MetalBottom(MetalEnd):
    def set(self, box):
        self.polygon = box.toPolygon()
        self.polygon.reverse()
        self.height = box.center[2]


class MultitexTop(Top):
    textureNames = ('concrete.jpeg', 'lightmap.jpeg')
    def set(self, box):
        Top.set(self, box)
        self.render.textures[1].texEnv = GL_MODULATE


class Bottom(End):
    def set(self, box):
        self.polygon = box.toPolygon()
        self.polygon.reverse()
        self.height = box.center[2]


class TopDecal(DisplayList):
    """Abstract base class for decals covering the entire top of a box"""
    texCoords = ( (0,0), (1,0), (1,1), (0,1) )

    def set(self, box):
        self.center = box.center
        self.size = box.size
        self.angle = box.angle
        self.render.decal = True

    def drawToList(self, rstate):
        glPushMatrix()
        glNormal3f(0, 0, -1)
        glTranslatef(*self.center)
        glTranslatef(0, 0, self.size[2])
        glRotatef(self.angle, 0.0, 0.0, 1.0)

        glBegin(GL_QUADS)
        glTexCoord2f(*self.texCoords[0])
        glMultiTexCoord2fARB(GL_TEXTURE1_ARB, *self.texCoords[0])
        glVertex2f(-self.size[0], -self.size[1])
        glTexCoord2f(*self.texCoords[1])
        glMultiTexCoord2fARB(GL_TEXTURE1_ARB, *self.texCoords[1])
        glVertex2f( self.size[0], -self.size[1])
        glTexCoord2f(*self.texCoords[2])
        glMultiTexCoord2fARB(GL_TEXTURE1_ARB, *self.texCoords[2])
        glVertex2f( self.size[0],  self.size[1])
        glTexCoord2f(*self.texCoords[3])
        glMultiTexCoord2fARB(GL_TEXTURE1_ARB, *self.texCoords[3])
        glVertex2f(-self.size[0],  self.size[1])
        glEnd()
        glPopMatrix()


class AlignedTopDecal(TopDecal):
    """Abstract base class for a TopDecal that has its X axis aligned with the
       box's major axis and the Y axis with the box's minor axis.
       """
    def __init__(self, box):
        if box.size[1] > box.size[0]:
            self.texCoords = ( (0,0), (0,1), (1,1), (1,0) )
        TopDecal.__init__(self, box)


class TiledTopDecal(Top):
    """Abstract base class for a TopDecal that is tiled in world coordinates"""
    def set(self, box):
        Top.set(self, box)
        self.render.decal = True


class OilStain(TopDecal):
    """A random oil stain decal"""
    def __init__(self, box):
        self.textureName = random.choice(('oilstain_1.png',
                                          'oilstain_2.png',
                                          'oilstain_3.png',
                                          'swerve_1.png'))
        TopDecal.__init__(self, box)


class TreadStain(AlignedTopDecal):
    """A tread stain decal"""
    textureName = "treadstain_1.png"


class FixedDecal(DisplayList):
    """Abstract base class for a decal with a fixed size and aspect ratio,
       positioned within a plane. origin, vx, and vy specify the plane's coordinate space.
       position is in this space. width is in world coordinates. anchor is the position
       in texture coordinates that should be anchored to the given position in plane coordinates.
       """
    def set(self, origin, vx, vy, width, position=(0.5, 0.5), anchor=(0.5, 0.5)):
        """           """
        self.render.decal = True

        # Transform width from world coordinates to plane coordinates
        width /= magnitude(vx)
        
        # Calculate a height to keep the image aspect ratio correct
        texSize = self.render.textures[0].size
        height = width * texSize[1] / texSize[0] * magnitude(vx) / magnitude(vy)

        # Compute the amount of the texture to each side of the given position
        leftSize = anchor[0] * width
        rightSize = (1 - anchor[0]) * width
        topSize = anchor[1] * height
        bottomSize = (1 - anchor[1]) * height

        # Compute the vertices of a quadrilateral using the given vectors
        self.quad = (multiply(vx, position[0] - leftSize) +
                     multiply(vy, position[1] - topSize) + origin,
                     multiply(vx, position[0] + rightSize) +
                     multiply(vy, position[1] - topSize) + origin,
                     multiply(vx, position[0] + rightSize) +
                     multiply(vy, position[1] + bottomSize) + origin,
                     multiply(vx, position[0] - leftSize) +
                     multiply(vy, position[1] + bottomSize) + origin)

        # Compute a normal to the given plane
        self.normal = normalize(cross(vx, vy))
        
    def drawToList(self, rstate):
        glNormal3f(*self.normal)
        glBegin(GL_QUADS)
        glTexCoord2f(0,0)
        glVertex3f(*self.quad[0])
        glTexCoord2f(1,0)
        glVertex3f(*self.quad[1])
        glTexCoord2f(1,1)
        glVertex3f(*self.quad[2])
        glTexCoord2f(0,1)
        glVertex3f(*self.quad[3])
        glEnd()


class FixedWallDecal(FixedDecal):
    """Abstract base class for a fixed-size decal applied to a box wall"""
    def set(self, box, wallNumber, width, position=(0.5, 0.5), anchor=(0.5, 0.5)):
        poly = box.toPolygon()
        edge1 = poly[(wallNumber * 2) % len(poly)]
        edge2 = poly[(wallNumber * 2 + 1) % len(poly)]

        # Place the origin at the ground, with X pointing along the wall and Y pointing up
        origin = (edge1[0], edge1[1], box.center[2])
        vx = subtract((edge2[0], edge2[1], box.center[2]), origin)
        vy = (0, 0, box.size[2])

        FixedDecal.set(self, origin, vx, vy, width, position, anchor)


class LogoDecal(FixedWallDecal):
    """A BZFlag logo applied to a random box wall"""
    textureName = 'logo.png'
    
    def set(self, box):
        FixedWallDecal.set(self, box, random.randint(0,3), 10, (0, 1), (-0.1, 1.1))


class DustPuppyDecal(FixedWallDecal):
    """It's Dust Puppy!"""
    textureName = 'dust_puppy.png'
    
    def set(self, box):
        FixedWallDecal.set(self, box, random.randint(0,3), 10, (0.5, 0), (0.5, 0))


class FixedTopDecal(FixedDecal):
    """Abstract base class for a fixed-size decal applied to a box top"""
    def set(self, box, width, position=(0.5, 0.5), anchor=(0.5, 0.5)):
        poly = box.toPolygon()
        height = box.center[2] + box.size[2]
        origin = (poly[0][0], poly[0][1], height)
        vx = subtract((poly[1][0], poly[1][1], height), origin)
        vy = subtract((poly[-1][0], poly[-1][1], height), origin)
        FixedDecal.set(self, origin, vx, vy, width, position, anchor)


class GrassEdge(Sides):
    """Grass at the edge of a box sitting on the ground"""
    textureNames = ('grass_edge.png', 'lightmap.jpeg')
    
    def __init__(self, box):
        Sides.__init__(self,box)
        self.render.decal = True

    def set(self, box):
        Sides.set(self, box)
        self.size = (self.size[0],
                     self.size[1],
                     1.5)
        self.texRepeats = (self.size[0] / 2,
                           self.size[1] / 2,
                           0.95)
        self.tex2Repeats = (self.size[0] / 400,
                           self.size[1] / 400,
                           0)


def detectBoxDrawables(box):
    """Given a box WorldObject, return a list of the drawables that should be used
       to represent it. This looks at the size and placement of the box to determine
       which drawables to use to represent it.
       """
    sides = None
    top = None
    bottom = None
    drawables = []

    # Don't try to draw boxes with a zero size, they will give is a divide by zero later
    if not (box.size[0] and box.size[1] and box.size[2]):
        return []

    # Calculate a few values based on the box proportions to make the below decisions clearer
    height = box.size[2]
    majorAxis = max(box.size[0], box.size[1])
    minorAxis = min(box.size[0], box.size[1])
    axisRatio = majorAxis / minorAxis
    heightRatio = height / minorAxis

    # If the box is on the ground and it's high enough, add some grass around the edges
    if box.center[2] == 0 and height > 1:
        drawables.append(GrassEdge(box))

    # If the box isn't on the ground and it's fairly flat, assume it's a platform
    if height < 8 and box.center[2] > 1:
        sides = PlatformSides(box)

    if majorAxis < 8 and axisRatio < 1.1 and heightRatio < 2.2 and heightRatio > 1.8:
        # If the box is a fairly small cube, make it metal
        sides = MetalSides(box)
        top = MetalTop(box)
        bottom = MetalBottom(box)
    elif minorAxis > Scale.TankWidth and minorAxis < Scale.TankWidth * 4 and majorAxis > Scale.TankWidth * 4:
        # If the box is about the same width as a tank, put some tread marks on it...
        drawables.append(TreadStain(box))
    elif minorAxis > 10 and axisRatio < 2:
        # ...or, if the box is fairly large and squareish, use an oil-stain texture
        drawables.append(OilStain(box))
    elif minorAxis > 10:
        # ...or if it's fairly large and we have nothing better to
        #    do to the top, multitexture it so it isn't so boring
        top = MultitexTop(box)
    
    # Randomly put BZFlag logos on properly sized boxes
    if minorAxis > 10 and height > 4 and random.random() > 0.7:
        drawables.append(LogoDecal(box))

    # Randomly put a Dust Puppy on properly sized boxes, with a smaller probability
    if minorAxis > 10 and height > 15 and random.random() > 0.98:
        drawables.append(DustPuppyDecal(box))
        
    # Only draw the bottom if we're not on the ground
    if box.center[2] > 0:
        if bottom:
            drawables.append(bottom)
        else:
            drawables.append(Bottom(box))

    # Pick defaults for any characteristics we haven't decided on already and return a list
    if sides:
        drawables.append(sides)
    else:
        drawables.append(BrickSides(box))
    if top:
        drawables.append(top)
    else:
        drawables.append(Top(box))
    return drawables

### The End ###

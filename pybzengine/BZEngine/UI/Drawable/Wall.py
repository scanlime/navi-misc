""" BZFlag.UI.Drawable.Wall

A class to draw the walls in the world
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

from DisplayList import *
from OpenGL.GL import *
import Box, math


def thickify(center, size, angle, thickness=0.75):
    """Add some thickness to an otherwise flat wall.
       Returns a (center, size) tuple.
       """
    size = (thickness,
            size[1],
            size[2])
    radians = angle * math.pi / 180
    center = (center[0] - math.cos(radians) * thickness,
              center[1] - math.sin(radians) * thickness,
              center[2])
    return (center, size)


class WallSides(Box.Sides):
    textureNames = ('outer_wall.jpeg', 'wall_grime.jpeg')
    def set(self, wall):
        Box.Sides.set(self, wall)
        (self.center, self.size) = thickify(self.center, self.size, self.angle)
        self.texRepeats = (self.size[0] / self.size[2],
                           self.size[1] / self.size[2],
                           1)                           
        self.tex2Repeats = (self.size[0] / 20,
                            self.size[1] / 20,
                            1)


class WallTop(DisplayList):
    textureName = 'field_generator.jpeg'
    def set(self, wall):
        self.angle = wall.angle
        (self.center, self.size) = thickify(wall.center, wall.size, wall.angle)
        
    def drawToList(self, rstate):
        glPushMatrix()
        glTranslatef(*self.center)
        glTranslatef(0, 0, self.size[2])
        glRotatef(self.angle, 0.0, 0.0, 1.0)
        glNormal3f(0, 0, 1)
        glBegin(GL_QUADS)
        glTexCoord2f(0,0)
        glVertex2f(-self.size[0], -self.size[1])
        glTexCoord2f(1,0)
        glVertex2f( self.size[0], -self.size[1])
        glTexCoord2f(1, self.size[1] / self.size[0])
        glVertex2f( self.size[0],  self.size[1])
        glTexCoord2f(0, self.size[1] / self.size[0])
        glVertex2f(-self.size[0],  self.size[1])
        glEnd()
        glPopMatrix()
    pass

### The End ###

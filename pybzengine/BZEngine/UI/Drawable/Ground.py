""" BZEngine.UI.Drawable.Ground

A class to draw the ground in the world
"""
#
# Python BZEngine Package
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
from OpenGL.GL.ARB.multitexture import *
from BZEngine.UI import GLExtension


class Ground(DisplayList):
    textureNames = ('grass.jpeg', 'lightmap.jpeg')
    baseTexRepeats = 90
    overlayTexRepeats = 1

    def set(self, size):
        self.size = size / 2
        self.render.textures[1].texEnv = GL_MODULATE

    def drawToList(self, rstate):
        glPushMatrix()

        # We want to draw both sides of the surface. This will have OpenGL
        # automatically flip the surface normals when drawing the back side
        glDisable(GL_CULL_FACE)
        glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1)

        glBegin(GL_QUADS)
        glNormal3f(0, 0, 1);
        
        glTexCoord2f(self.baseTexRepeats, self.baseTexRepeats)
        glMultiTexCoord2fARB(GL_TEXTURE1_ARB, self.overlayTexRepeats, self.overlayTexRepeats)
        glVertex3f(self.size, self.size, 0)

        glTexCoord2f(0, self.baseTexRepeats)
        glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0, self.overlayTexRepeats)
        glVertex3f(-self.size, self.size, 0)

        glTexCoord2f(0,0)
        glMultiTexCoord2fARB(GL_TEXTURE1_ARB, 0,0)
        glVertex3f(-self.size, -self.size, 0)

        glTexCoord2f(self.baseTexRepeats, 0)
        glMultiTexCoord2fARB(GL_TEXTURE1_ARB, self.overlayTexRepeats, 0)
        glVertex3f(self.size, -self.size, 0)

        glEnd()
        glEnable(GL_CULL_FACE)
        glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0)
        glPopMatrix()

### The End ###

""" BZFlag.UI.Drawable.TestWidget

A temporary module here to test 3D widget functionality.
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

class TestWidget(DisplayList):
    def set(self, center):
        self.center = center
        self.render.overlay = True

    def drawToList(self, rstate):
        size = 0.2
        glPushMatrix()
	glLoadIdentity()
        glColor3f(1.0, 0.0, 0.0)
        # lower left corner
        glTranslatef(*self.center)
        glBegin(GL_QUADS)
        # front
        glNormal3f(0, 0, -1)
        glVertex3f(-size, size, -size)
        glVertex3f(size, size, -size)
        glVertex3f(size, -size, -size)
        glVertex3f(-size, -size, -size)
        # back
        glNormal3f(0, 0, 1)
        glVertex3f(size, size, size)
        glVertex3f(-size, size, size)
        glVertex3f(-size, -size, size)
        glVertex3f(size, -size, size)
        # top
        glNormal3f(0, 1, 0)
        glVertex3f(-size, size, -size)
        glVertex3f(-size, size, size)
        glVertex3f(size, size, size)
        glVertex3f(size, size, -size)
        # bottom
        glNormal3f(0, -1, 0)
        glVertex3f(-size, -size, -size)
        glVertex3f(size, -size, -size)
        glVertex3f(size, -size, size)
        glVertex3f(-size, -size, size)
        # left
        glNormal3f(-1, 0, 0)
        glVertex3f(-size, -size, size)
        glVertex3f(-size, size, size)
        glVertex3f(-size, size, -size)
        glVertex3f(-size, -size, -size)
        # right
        glNormal3f(1, 0, 0)
        glVertex3f(size, -size, -size)
        glVertex3f(size, size, -size)
        glVertex3f(size, size, size)
        glVertex3f(size, -size, size)
        glEnd()
        glColor3f(1.0, 1.0, 1.0)
        glPopMatrix()


### The End ###

""" BZFlag.UI.Drawable.Teleporter

Classes for drawing the teleporters in the world
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


class TeleporterField(DisplayList):
    textureName = 'caustic%02d.jpeg:50.0'
    def set(self, center, angle, size):
        self.center = center
        self.angle = angle
        self.size = size
        self.render.blended = True

        texScale  = 8
        texAspect = 1.3
        self.texRepeats = (self.size[2] / (texScale * texAspect),
                           self.size[1] / texScale)

    def drawToList(self, rstate):
        glBlendFunc(GL_SRC_ALPHA, GL_ONE)
        glDisable(GL_LIGHTING)
        glPushMatrix()
        glTranslatef(*self.center)
        glRotatef(self.angle, 0.0, 0.0, 1.0)
        glColor4f(0.89, 0.90, 0.99, 0.3)
        glBegin(GL_QUADS)
        # X+ side
        glNormal3f(1, 0, 0)
        glTexCoord2f(0,0)
        glVertex3f(self.size[0] / 2, self.size[1], 0)
        glTexCoord2f(self.texRepeats[0],0)
        glVertex3f(self.size[0] / 2, self.size[1], self.size[2])
        glTexCoord2f(self.texRepeats[0],self.texRepeats[1])
        glVertex3f(self.size[0] / 2, -self.size[1], self.size[2])
        glTexCoord2f(0,self.texRepeats[1])
        glVertex3f(self.size[0] / 2, -self.size[1], 0)
        # X- side
        glNormal3f(-1, 0, 0)
        glTexCoord2f(0,0)
        glVertex3f(-self.size[0] / 2, -self.size[1], 0)
        glTexCoord2f(self.texRepeats[0],0)
        glVertex3f(-self.size[0] / 2, -self.size[1], self.size[2])
        glTexCoord2f(self.texRepeats[0],self.texRepeats[1])
        glVertex3f(-self.size[0] / 2, self.size[1], self.size[2])
        glTexCoord2f(0,self.texRepeats[1])
        glVertex3f(-self.size[0] / 2, self.size[1], 0)
        glEnd()
        glColor3f(1.0, 1.0, 1.0)
        glPopMatrix()
        glEnable(GL_LIGHTING)
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
	

class TeleporterBorder(DisplayList):
    textureName = 'caution.png'
    def set(self, center, angle, size, border):
        self.center = center
        self.angle = angle
        self.size = size
        self.border = border

    def drawToList(self, rstate):
        glPushMatrix()
        glTranslatef(*self.center)
        glRotatef(self.angle, 0.0, 0.0, 1.0)
        glBegin(GL_TRIANGLE_STRIP)
        glNormal3f(-1, 0, 0)
        glTexCoord2f(0, 0)
        glVertex3f(-self.border / 2, self.size[1] + self.border, 0)
        glTexCoord2f(self.border, 0)
        glVertex3f(-self.border / 2, self.size[1], 0)
        glTexCoord2f(0, self.size[2] + self.border)
        glVertex3f(-self.border / 2, self.size[1] + self.border, self.size[2] + self.border)
        glTexCoord2f(self.border, self.size[2])
        glVertex3f(-self.border / 2, self.size[1], self.size[2])
        glTexCoord2f(2 * self.border + 2 * self.size[1], self.size[2] + self.border)
        glVertex3f(-self.border / 2, -self.size[1] - self.border, self.size[2] + self.border)
        glTexCoord2f(self.border + 2 * self.size[1], self.size[2])
        glVertex3f(-self.border / 2, -self.size[1], self.size[2])
        glTexCoord2f(2 * self.border + 2 * self.size[1], 0)
        glVertex3f(-self.border / 2, -self.size[1] - self.border, 0)
        glTexCoord2f(self.border + 2 * self.size[1], 0)
        glVertex3f(-self.border / 2, -self.size[1], 0)
        glEnd()
        glFrontFace(GL_CW)
        glBegin(GL_TRIANGLE_STRIP)
        glNormal3f(1, 0, 0)
        glTexCoord2f(0, 0)
        glVertex3f(self.border / 2, self.size[1] + self.border, 0)
        glTexCoord2f(self.border, 0)
        glVertex3f(self.border / 2, self.size[1], 0)
        glTexCoord2f(0, self.size[2] + self.border)
        glVertex3f(self.border / 2, self.size[1] + self.border, self.size[2] + self.border)
        glTexCoord2f(self.border, self.size[2])
        glVertex3f(self.border / 2, self.size[1], self.size[2])
        glTexCoord2f(2 * self.border + 2 * self.size[1], self.size[2] + self.border)
        glVertex3f(self.border / 2, -self.size[1] - self.border, self.size[2] + self.border)
        glTexCoord2f(self.border + 2 * self.size[1], self.size[2])
        glVertex3f(self.border / 2, -self.size[1], self.size[2])
        glTexCoord2f(2 * self.border + 2 * self.size[1], 0)
        glVertex3f(self.border / 2, -self.size[1] - self.border, 0)
        glTexCoord2f(self.border + 2 * self.size[1], 0)
        glVertex3f(self.border / 2, -self.size[1], 0)
        glEnd()
        glFrontFace(GL_CCW)
        glBegin(GL_QUADS)
        # top
        glNormal3f(0.4, 0, 1)
        glTexCoord2f(0.4, 1)
        glVertex3f(self.border / 2, -self.size[1] - self.border, self.size[2] + self.border)
        glTexCoord2f(2 * self.border + 2 * self.size[1] + 0.4, 1)
        glVertex3f(self.border / 2, self.size[1] + self.border, self.size[2] + self.border)
        glTexCoord2f(2 * self.border + 2 * self.size[1] + 0.4, 0)
        glVertex3f(-self.border / 2, self.size[1] + self.border, self.size[2] + self.border)
        glTexCoord2f(0.4, 0)
        glVertex3f(-self.border / 2, -self.size[1] - self.border, self.size[2] + self.border)
        # underside of top
        glNormal3f(0, 0, -1)
        glTexCoord2f(0.4, 0)
        glVertex3f(-self.border / 2, -self.size[1] - self.border, self.size[2])
        glTexCoord2f(2 * self.border + 2 * self.size[1] + 0.4, 0)
        glVertex3f(-self.border / 2, self.size[1] + self.border, self.size[2])
        glTexCoord2f(2 * self.border + 2 * self.size[1] + 0.4, 0.0001)
        glVertex3f(self.border / 2, self.size[1] + self.border, self.size[2])
        glTexCoord2f(0.4, 0.0001)
        glVertex3f(self.border / 2, -self.size[1] - self.border, self.size[2])
        # Y+ outside
        glNormal3f(0, 1, 0)
        glTexCoord2f(self.border, 0)
        glVertex3f(-self.border / 2, self.size[1] + self.border, 0)
        glTexCoord2f(self.border, self.size[2] + self.border)
        glVertex3f(-self.border / 2, self.size[1] + self.border, self.size[2] + self.border)
        glTexCoord2f(0, self.size[2] + self.border)
        glVertex3f(self.border / 2, self.size[1] + self.border, self.size[2] + self.border)
        glTexCoord2f(0, 0)
        glVertex3f(self.border / 2, self.size[1] + self.border, 0)
        # Y+ inside
        glNormal3f(0, -1, 0)
        glTexCoord2f(0, 0)
        glVertex3f(self.border / 2, self.size[1], 0)
        glTexCoord2f(0, self.size[2])
        glVertex3f(self.border / 2, self.size[1], self.size[2])
        glTexCoord2f(self.border, self.size[2])
        glVertex3f(-self.border / 2, self.size[1], self.size[2])
        glTexCoord2f(self.border, 0)
        glVertex3f(-self.border / 2, self.size[1], 0);
        # Y- outside
        glNormal3f(0, 1, 0)
        glTexCoord2f(0, 0)
        glVertex3f(self.border / 2, -self.size[1] - self.border, 0)
        glTexCoord2f(0, self.size[2] + self.border)
        glVertex3f(self.border / 2, -self.size[1] - self.border, self.size[2] + self.border)
        glTexCoord2f(self.border, self.size[2] + self.border)
        glVertex3f(-self.border / 2, -self.size[1] - self.border, self.size[2] + self.border)
        glTexCoord2f(self.border, 0)
        glVertex3f(-self.border / 2, -self.size[1] - self.border, 0)
        # Y- inside
        glNormal3f(0, -1, 0)
        glTexCoord2f(self.border, 0)
        glVertex3f(-self.border / 2, -self.size[1], 0)
        glTexCoord2f(self.border, self.size[2])
        glVertex3f(-self.border / 2, -self.size[1], self.size[2])
        glTexCoord2f(0, self.size[2])
        glVertex3f(self.border / 2, -self.size[1], self.size[2])
        glTexCoord2f(0, 0)
        glVertex3f(self.border / 2, -self.size[1], 0)
        # Y+ leg bottom
        glNormal3f(0, 0, -1)
        glTexCoord2f(self.border, self.border)
        glVertex3f(self.border / 2, self.size[1] + self.border, 0)
        glTexCoord2f(0, self.border)
        glVertex3f(self.border / 2, self.size[1], 0)
        glTexCoord2f(0, 0)
        glVertex3f(-self.border / 2, self.size[1], 0)
        glTexCoord2f(self.border, 0)
        glVertex3f(-self.border / 2, self.size[1] + self.border, 0)
        # Y- leg bottom
        glNormal3f(0, 0, -1);
        glTexCoord2f(self.border, self.border);
        glVertex3f(-self.border / 2, -self.size[1] - self.border, 0);
        glTexCoord2f(self.border, 0);
        glVertex3f(-self.border / 2, -self.size[1], 0);
        glTexCoord2f(0, 0);
        glVertex3f(self.border / 2, -self.size[1], 0);
        glTexCoord2f(0, self.border);
        glVertex3f(self.border / 2, -self.size[1] - self.border, 0);
        glEnd()
        glPopMatrix()

### The End ###

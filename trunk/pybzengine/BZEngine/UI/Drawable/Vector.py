""" BZEngine.UI.Drawable.Vector

For debugging and demonstration, a drawable that shows the value of one vector
"""
#
# Python BZEngine Package
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

from DisplayList import *
from OpenGL.GL import *
import math, Numeric


class Vector(DisplayList):
    """A drawable that overlays the magnitude and direction of a vector onto the scene,
       preserving the camera's direction. The size and position of the overlay, in
       camera coordinates, can be supplied. By default the overlay is placed very far
       from the camera so that the effects of perspective are minimized.
       """
    def set(self, vector, color=(0,1,0,1), center=(-200, -200, -1000), size=100):
        self.vector = vector
        self.center = center
        self.size = size
        self.color = color

        # Render in the overlay pass, and don't get cached in a display list
        self.render.overlay = True
        self.render.static = False

    def unitCircle(self):
        """Draws a unit circle in the current color"""
        glBegin(GL_LINE_LOOP)
        for theta in Numeric.arange(0, math.pi*2, 0.15):
            glVertex2f(math.cos(theta), math.sin(theta))
        glEnd()

    def unitSquare(self):
        """Draw a square in the current color, extending from -1 to 1 in each axis"""
        glBegin(GL_LINE_LOOP)
        glVertex2f(-1,-1)
        glVertex2f( 1,-1)
        glVertex2f( 1, 1)
        glVertex2f(-1, 1)
        glEnd()

    def drawToList(self, rstate):
        """The static portions of our widget are stored in a display list"""
        # Draw unit circles/squares in the X, Y, and Z planes
        glColor4f(1,1,1, 0.3)
        self.unitCircle()
        self.unitSquare()
        glPushMatrix()
        glRotatef(90, 1,0,0)
        self.unitCircle()
        self.unitSquare()
        glPopMatrix()
        glPushMatrix()
        glRotatef(90, 0,1,0)
        self.unitCircle()
        self.unitSquare()
        glPopMatrix()

    def draw(self, rstate):
        glDisable(GL_LIGHTING)
        glDisable(GL_DEPTH_TEST)
        glEnable(GL_BLEND)
        glEnable(GL_LINE_SMOOTH)
        glPushMatrix()

        # Save the top 3x3 part of the modelview matrix so we can
        # reapply the camera's rotation around the scene later
        rotation = Numeric.identity(4, Numeric.Float32)
        rotation[:3,:3] = glGetFloatv(GL_MODELVIEW_MATRIX)[:3,:3]
        rotation[3,3] = 1

        # Position ourselves in front of the camera
        glLoadIdentity()
        glTranslatef(*self.center)
        glScalef(self.size, self.size, self.size)

        # Reapply the camera's rotation
        glMultMatrixf(rotation)

        # Draw the static portion from above
        DisplayList.draw(self, rstate)

        # Draw the vector itself
        glColor3f(0,1,0)
        glBegin(GL_LINES)
        glVertex3f(0,0,0)
        glVertex3f(*tuple(self.vector))
        glEnd()

        glPopMatrix()
        glEnable(GL_LIGHTING)
        glEnable(GL_DEPTH_TEST)
        glDisable(GL_BLEND)
        glDisable(GL_LINE_SMOOTH)
        glColor3f(1,1,1)

### The End ###

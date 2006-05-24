"""Define bones for rendering a stick figure from an ASF file.

Classes:
    - Bone      An OpenGL bone
"""

# Copyright (C) 2006 W. Evan Sheehan
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

from OpenGL.GL import *
import Numeric

class Bone:
    """Represent a bone to be drawn with OpenGL."""

    def __init__(self, name, direction, length, dof):
        self.name = name
        self.direction = Numeric.array([float(x) for x in direction])
        self.length = float(length)
        self.dof = dof
        self.children = []

    def addChild(self, child):
        """Add a child of this bone."""
        self.children.append(child)

    def draw(self, data):
        pass


class StickFigure(Bone):
    def draw(self, data):
        """Draw the bone on screen.

        'data' is a dictionary mapping bone name to position.
        """
        # Some joints defined in the ASF file have no degrees of freedom. So if
        # there's no data for this bone don't draw anything and move on to the
        # children of the bone.
        if self.name in data:
            frame = data[self.name]
            if "rx" in self.dof:
                glRotatef(frame[self.dof.index("rx")], 1.0, 0.0, 0.0)
            if "ry" in self.dof:
                glRotatef(frame[self.dof.index("ry")], 0.0, 1.0, 0.0)
            if "rz" in self.dof:
                glRotatef(frame[self.dof.index("rz")], 0.0, 0.0, 1.0)

            tx = ty = tz = 0.0
            if "tx" in self.dof:
                tx = frame[self.dof.index("tx")]
            if "ty" in self.dof:
                ty = frame[self.dof.index("ty")]
            if "tz" in self.dof:
                tz = frame[self.dof.index("tz")]

            # Add the translation for this bone
            glTranslatef(tx, ty, tz)

            # Draw the line
            glBegin(GL_LINES)
            glVertex3f(0.0, 0.0, 0.0)
            glVertex3f(self.direction[0] * self.length,
                    self.direction[1] * self.length,
                    self.direction[2] * self.length)
            glEnd()

            # Add a translation to draw the children at the end of this bone
            glTranslatef(self.direction[0] * self.length,
                    self.direction[1] * self.length,
                    self.direction[2] * self.length)

        # Draw each child of this bone
        for child in self.children:
            # Push the matrix onto the stack so that rotations and translations
            # of one child do not affect another.
            glPushMatrix()
            child.draw(data)
            glPopMatrix()
        

# vim: ts=4:sw=4:et

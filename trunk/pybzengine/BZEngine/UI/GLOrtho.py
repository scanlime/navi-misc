""" BZEngine.UI.GLOrtho

Utilities for doing 2D drawing in an orthogonal mode OpenGL viewport
(viewport.fov is None, no scaling transformations have been applied)

These are all implemented as bare functions, since they operate only
against the current OpenGL context.
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

from OpenGL.GL import *
from BZEngine.UI import GLExtension


def rectVertices(size, alignment=(0,0)):
    """Specifies vertices of a rectangle of the given size.
       The rectangle is placed such that the location given by
       'alignment' is at the origin. This is consistent with
       the usage of alignment elsewhere- (0,0) places the
       top-left corner at the origin, (1,1) places the bottom-right
       at the origin, and so on.
       Texture coordinates such that (0,0) is at the top-left and
       (1,1) is at the bottom-right are specified.
       Vertices are in clockwise order.
       """
    glTexCoord2f(0,0)
    glVertex2f(-alignment[0] * size[0],
               -alignment[1] * size[1])
    glTexCoord2f(1,0)
    glVertex2f(size[0] - alignment[0] * size[0],
               -alignment[1] * size[1])
    glTexCoord2f(1,1)
    glVertex2f(size[0] - alignment[0] * size[0],
               size[1] - alignment[1] * size[1])
    glTexCoord2f(0,1)
    glVertex2f(-alignment[0] * size[0],
               size[1] - alignment[1] * size[1])


def filledRect(size, alignment=(0,0)):
    """Draws a filled rectangle, size and alignment are as described
       in rectVertices above.
       """
    glBegin(GL_POLYGON)
    rectVertices(size, alignment)
    glEnd()


def rectOutline(size, alignment=(0,0)):
    """Draws a rectangle outline, size and alignment are as described
       in rectVertices above.
       """
    glBegin(GL_LINE_LOOP)
    rectVertices(size, alignment)
    glEnd()


def setColor(*color):
    """Set a 4-tuple color. Equivalent to glColor4f, but here to keep the
       level of abstraction consistent.
       """
    glColor4f(*color)


def setup():
    """Set up the current OpenGL context for default 2D rendering"""
    glEnable(GL_BLEND)
    glDisable(GL_LIGHTING)
    glDisable(GL_CULL_FACE)
    glDisable(GL_COLOR_MATERIAL)
    glDisable(GL_DEPTH_TEST)
    GLExtension.disableMultitex()
    glDisable(GL_TEXTURE_2D)
    glLoadIdentity()

### The End ###

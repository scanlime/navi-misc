""" BZEngine.UI.Drawable.Container

Basic containers that can be used to group together multiple drawables
into one, and to perform transformations on other drawables.
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
from BZEngine.UI.Drawable.GLDrawable import *
from BZEngine.UI.Drawable.DisplayList import *
from OpenGL.GL import *
import copy

__all__ = ['Group', 'Transformer', 'Transform']


class Group(DisplayList):
    """When several drawables are generally used together, this drawable
       can be used to group them all together into a single display list.

       This may only be used with drawables that would be placed in the same rendering pass,
       and have the same texture.
       """
    def set(self, children):
        self.render = children[0].render
        self.children = children

    def drawToList(self, rstate):
        for child in self.children:
            child.drawToList(rstate)


class Transformer(GLDrawable):
    """Holds one drawable, applying transforms before rendering it.
       To transform several drawables using the same settings, wrap each drawable with a
       Transformer but point them all to one central Transform list.
       """
    def __init__(self, child, transforms):
        GLDrawable.__init__(self)
        self.child = child
        self.transforms = transforms
        self.reversedTransforms = transforms[:]
        self.reversedTransforms.reverse()
        self.render = copy.copy(child.render)

    def draw(self, rstate):
        for transform in self.transforms:
            transform.apply()
        self.child.draw(rstate)
        for transform in self.reversedTransforms:
            transform.unapply()
        

class Transform:
    """Abstract base class for transforms designed for use with Transformer"""
    def apply(self):
        """Apply this transform to the current OpenGL state"""
        pass

    def unapply(self):
        """Reverse the effects of apply()"""
        pass

### The End ###

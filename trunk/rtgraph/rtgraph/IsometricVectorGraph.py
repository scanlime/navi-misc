""" rtgraph.IsometricVectorGraph

Graphs 3D vectors with an isometric view
"""
#
# rtgtaph real-time graphing package for PyGTK
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
from Graph import *
import gtk

__all__ = ["IsometricVectorGraph"]


class IsometricVectorGraph(StaticGridGraph):
    """A graph that shows multiple 3D cartesian vectors as dots in
       an isometric grid.
       Expects channel values to be 3-tuples of values in the range [0,1]
       """
    def __init__(self, size=(384,384), channels=[]):
        StaticGridGraph.__init__(self, size, channels)

    def graphChannel(self, channel):
        """Draw the given channel on the backbuffer"""
        vector = channel.getValue()

    def initGrid(self, drawable, width, height):
        """Draw a grid to the given drawable at the given size"""
        drawable.draw_rectangle(self.bgGc, gtk.TRUE, 0, 0, width, height)

### The End ###

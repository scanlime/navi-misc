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
import math

__all__ = ["IsometricVectorGraph"]


class IsometricVectorGraph(StaticGridGraph):
    """A graph that shows multiple 3D cartesian vectors as dots in
       an isometric grid.
       Expects channel values to be 3-tuples of values in the range [0,1]
       """
    def __init__(self, size=(384,384), channels=[]):
        StaticGridGraph.__init__(self, size, channels)

        # Default axis vectors
        self.axes = [(1, 0.6),
                     (-1, 0.6),
                     (0, -1)]

        # Make unit vectors orthogonal to each axis
        self.axisUnitOrthos = [normalize(orthogonal(b)) for b in self.axes]

        self.numTickMarks = 10

    def graphChannel(self, channel):
        """Draw the given channel on the backbuffer"""
        radius = 2
        value = channel.getValue()
        gc = channel.getGC(self)

        # The vector itself
        v = self.mapVector(value)
        self.backingPixmap.draw_rectangle(gc, gtk.TRUE,
                                          int(v[0] - radius), int(v[1] - radius),
                                          radius*2, radius*2)

        # Tick marks on all axes
        self.drawTick(self.backingPixmap, gc, 0, value[0])
        self.drawTick(self.backingPixmap, gc, 1, value[1])
        self.drawTick(self.backingPixmap, gc, 2, value[2])

    def initGrid(self, drawable, width, height):
        """Draw a grid to the given drawable at the given size"""
        drawable.draw_rectangle(self.bgGc, gtk.TRUE, 0, 0, width, height)

        # Determine the center of the graph and a reasonable size for it
        self.center = (self.width * 0.5, self.height * 0.5)
        self.scale = min(self.width, self.height) * 0.45

        # Draw x, y, and z axes
        origin = self.mapVector((0,0,0))
        for axis in range(3):
            # The axis itself
            v = [0,0,0]
            v[axis] = 1
            mapped = self.mapVector(v)
            drawable.draw_line(self.gridGc, int(origin[0]), int(origin[1]),
                               int(mapped[0]), int(mapped[1]))

            # Draw tick marks
            for i in xrange(self.numTickMarks):
                i /= self.numTickMarks
                self.drawTick(drawable, self.gridGc, axis, i)

    def mapVector(self, v):
        """Map the given 3-tuple into 2D space. Same as multiplying by
           the 3x2 matrix with columns self.axes, then scaling/translating
           according to self.scale and self.center.
           """
        return ((self.axes[0][0] * v[0] + self.axes[1][0] * v[1] + self.axes[2][0] * v[2]) *
                self.scale + self.center[0],
                (self.axes[0][1] * v[0] + self.axes[1][1] * v[1] + self.axes[2][1] * v[2]) *
                self.scale + self.center[1])

    def drawTick(self, drawable, gc, axis, position):
        """Draw a tick mark on an axis. The axis should be either 0, 1, or 2.
           Position should be in the range [0,1]
           """
        tickRadius = 2
        v = [0] * 3
        v[axis] = position
        posVector = self.mapVector(v)
        unitOrtho = self.axisUnitOrthos[axis]
        drawable.draw_line(gc,
                           int(posVector[0] - unitOrtho[0] * tickRadius),
                           int(posVector[1] - unitOrtho[1] * tickRadius),
                           int(posVector[0] + unitOrtho[0] * tickRadius),
                           int(posVector[1] + unitOrtho[1] * tickRadius))


def orthogonal(v):
    """Returns a 2-vector orthogonal to the given 2-vector"""
    return (v[1], -v[0])

def normalize(v):
    """Normalize the given 2-vector"""
    l = math.sqrt(v[0]**2 + v[1]**2)
    return (v[0]/l, v[1]/l)

### The End ###

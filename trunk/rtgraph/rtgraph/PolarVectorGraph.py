""" rtgraph.PolarVectorGraph

Graphs polar vectors, represented as lines on a unit circle.
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

__all__ = ["PolarVectorGraph"]


class PolarVectorGraph(StaticGridGraph):
    """A graph that shows multiple polar vectors, lines superimposed
       on a polar grid.
       Expects channel values to be (magnitude, angle) tuples, with
       magnitude in the range [0,1] and angle in radians.
       """
    def __init__(self, size=(128,128), channels=[]):
        StaticGridGraph.__init__(self, size, channels)

        # by default, draw grids at two radii and 8 angles
        self.radiusGrids = (0.25, 0.5, 0.75, 1)
        self.angleGrids = [i*math.pi*2/8 for i in range(8)]

    def graphChannel(self, channel):
        """Draw the given channel on the backbuffer"""
        radius, angle = channel.getValue()
        self.backingPixmap.draw_line(channel.getGC(self),
                                     int(self.center[0]),
                                     int(self.center[1]),
                                     int(math.cos(angle) * radius * self.pixelRadius + self.center[0]),
                                     int(math.sin(angle) * radius * self.pixelRadius + self.center[1]))

    def initGrid(self, drawable, width, height):
        """Draw a grid to the given drawable at the given size"""
        drawable.draw_rectangle(self.bgGc, gtk.TRUE, 0, 0, width, height)

        # Calculate a center and radius for our new size
        self.pixelRadius = min(width, height)/2 - 4
        self.center = (self.width/2, self.height/2)

        for radius in self.radiusGrids:
            # Draw a circle at this radius.
            # The calculations for this are a bit weird since gtk positions
            # circles by bounding box rather than center and radius.
            drawable.draw_arc(self.gridGc, gtk.FALSE,
                              int(self.center[0] - radius * self.pixelRadius),
                              int(self.center[1] - radius * self.pixelRadius),
                              int(2 * radius * self.pixelRadius),
                              int(2 * radius * self.pixelRadius),
                              0, 64*360)

        for angle in self.angleGrids:
            # Draw lines at each angle
            drawable.draw_line(self.gridGc,
                               int(self.center[0]),
                               int(self.center[1]),
                               int(math.cos(angle) * self.pixelRadius + self.center[0]),
                               int(math.sin(angle) * self.pixelRadius + self.center[1]))

### The End ###

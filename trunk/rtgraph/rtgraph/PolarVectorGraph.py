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


class PolarVectorGraph(Graph):
    """A graph that multiple polar vectors, as lines superimposed
       on a polar grid.
       Expects channel values to be (magnitude, angle) tuples, with
       magnitude in the range [0,1] and angle in radians.
       """
    def __init__(self, size=(128,128), channels=[]):
        Graph.__init__(self, size, channels)

        # by default, draw grids at two radii and 8 angles
        self.radiusGrids = (0.25, 0.5, 0.75, 1)
        self.angleGrids = [i*math.pi*2/8 for i in range(8)]

    def graphChannel(self, channel):
        """Draw the given channel on the backbuffer"""
        radius, angle = channel.getValue()
        width = self.width
        height = self.height
        self.backingPixmap.draw_line(channel.getGC(self), int(width/2), int(height/2),
                                     int(math.cos(angle) * radius * (width/2) + width/2),
                                     int(math.sin(angle) * radius * (width/2) + width/2))

    def drawBackground(self):
        """Draws our grid after changing sizes. Initializes the grid to
           a separate pixmap, then draws that to the backbuffer,
           so the grid can be quickly redrawn for each frame.
           """
        self.gridPixmap = gtk.gdk.Pixmap(self.window, self.width, self.height)
        self.initGrid(self.gridPixmap, self.width, self.height)
        self.blitGrid()

    def initGrid(self, drawable, width, height):
        """Draw a grid to the given drawable at the given size"""
        drawable.draw_rectangle(self.bgGc, gtk.TRUE, 0, 0, width, height)

        for radius in self.radiusGrids:
            # Draw a circle at this radius.
            # The calculations for this are a bit weird since gtk positions
            # circles by bounding box rather than center and radius.
            drawable.draw_arc(self.gridGc, gtk.FALSE,
                              int(width/2 - radius*width/2), int(height/2 - radius*height/2),
                              int(radius*width), int(radius*height),
                              0, 64*360)

        for angle in self.angleGrids:
            # Draw lines at each angle
            drawable.draw_line(self.gridGc, int(width/2), int(height/2),
                               int(math.cos(angle) * (width/2) + width/2),
                               int(math.sin(angle) * (width/2) + width/2))

    def blitGrid(self):
        """Blit our grid pixmap to the backbuffer, erasing current channel lines"""
        self.backingPixmap.draw_drawable(self.bgGc, self.gridPixmap, 0,0,0,0, self.width, self.height)

    def integrate(self, dt):
        """Updates any channels that have changed"""
        changedChannels = [channel for channel in self.channels if channel.hasChanged(self)]
        if changedChannels:
            self.blitGrid()
            for channel in changedChannels:
                self.graphChannel(channel)
            self.queue_draw_area(0, 0, self.width, self.height)

### The End ###

""" rtgraph.HScrollLineGraph

A widget for graphing multiple channels of data on a line graph
that scrolls horizontally as time progresses.
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
from HScrollGraph import *

__all__ = ["HScrollLineGraph"]


class HScrollLineGraph(HScrollGraph):
    """A horizontally scrolling real-time line plot.
       Expects scalar values within the given range.
    """
    def __init__(self,
                 size       = (384,128),
                 channels   = [],
                 gridSize   = 32,
                 scrollRate = 80,
                 range      = (0,1),
                 ):
        HScrollGraph.__init__(self, size, channels, gridSize, scrollRate)
        self.range = range
        self.penVectors = {}

    def graphChannel(self, channel):
        # Scale the channel value to match a range of (0,1)
        scaled = (channel.getValue() - self.range[0]) / (self.range[1] - self.range[0])

        # Calculate a current pen position, always at the right side of the graph
        penVector = (self.width-1, int((self.height-1) * (1-scaled)))

        # If we have both a new pen vector and an old pen vector, we can draw a line
        if self.penVectors.has_key(channel):
            oldPenVector = self.penVectors[channel]
            self.backingPixmap.draw_line(channel.getGC(self),
                                         oldPenVector[0], oldPenVector[1],
                                         penVector[0], penVector[1])

        # Store the pen vector for later
        self.penVectors[channel] = penVector

    def resized(self):
        HScrollGraph.resized(self)

        # Invalidate saved pen vectors
        self.penVectors = {}

    def exposedPixels(self, nPixels):
        """Scrolls our old pen vectors along with the graph,
           culling out old vectors while we're at it.
           """
        # Note that it's important to use items() here,
        # ince the penVectors dict might change while we're
        # iterating.
        for channel, penVector in self.penVectors.items():
            if channel in self.channels:
                self.penVectors[channel] = (
                    penVector[0] - nPixels,
                    penVector[1])
            else:
                del self.penVectors[channel]

### The End ###

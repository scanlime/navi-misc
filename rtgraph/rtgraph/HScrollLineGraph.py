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

from HScrollGraph import *

__all__ = ["HScrollLineGraph"]


class HScrollLineGraph(HScrollGraph):
    """A horizontally scrolling real-time line plot"""
    def __init__(self, *args, **kwargs):
        HScrollGraph.__init__(self, *args, **kwargs)
        self.penVectors = {}

    def graphChannel(self, channel):
        # Calculate a current pen position, always at the right side of the graph
        penVector = (self.width-1, int((self.height-1) * (1-channel.getValue())))

        # If we have both a new pen vector and an old pen vector, we can draw a line
        if self.penVectors.has_key(channel):
            oldPenVector = self.penVectors[channel]
            self.backingPixmap.draw_line(channel.getGC(self),
                                         oldPenVector[0], oldPenVector[1],
                                         penVector[0], penVector[1])

        # Store the pen vector for later
        self.penVectors[channel] = penVector

    def resize(self):
        HScrollGraph.resize(self)

        # Invalidate saved pen vectors
        self.penVectors = {}

    def exposedPixels(self, nPixels):
        """Scrolls our old pen vectors along with the graph,
           culling out old vectors while we're at it.
           """
        for channel in self.penVectors.iterkeys():
            if channel in self.channels:
                self.penVectors[channel] = (
                    self.penVectors[channel][0] - nPixels,
                    self.penVectors[channel][1])
            else:
                del self.penVectors[channel]

### The End ###

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

    def __init__(self, size=(384,128), channels=[], gridSize=32):
        Graph.__init__(self, size, channels)
        self.gridSize = gridSize

        # Number of pixels we've scrolled, modulo the gridSize (floating point)
        self.gridPhase = 0.0

        # Our initial scroll rate, in pixels per second
        self.scrollRate = 80.0

        # channelPenVectors stores the last position each channel was drawn at,
        # adjusted for graph scrolling. Disabled channels or channels that have
        # not been drawn yet should have None, other channels should have a 2-tuple
        # with the (x,y) coordinates.
        self.channelPenVectors = [None] * numChannels

    def graphChannels(self):
        """Graph the current values of each channel. Called each time the
           graph is scrolled and/or the channel values are changed.
           """
        for i in range(len(self.channels)):
            channel = self.channels[i]
            if channel is not None:
                # Calculate a current pen position, always at the right side of the graph
                penVector = (self.width-1, int((self.height-1) * (1-channel)))

                # If we have both a new pen vector and an old pen vector, we can draw a line
                oldPenVector = self.channelPenVectors[i]
                if oldPenVector is not None:
                    gc = self.get_style().fg_gc[gtk.STATE_NORMAL]
                    self.backingPixmap.draw_line(gc,
                                                 oldPenVector[0], oldPenVector[1],
                                                 penVector[0], penVector[1])

                # Store the pen vector for later
                self.channelPenVectors[i] = penVector
            else:
                # Ensure that we have no saved pen vector, so that if a channel is enabled
                # after being disabled for a while, we don't get a line from it's ancient
                # position to its new position
                self.channelPenVectors[i] = None

    def gtkConfigureEvent(self, widget=None, event=None):
        """Called when the widget is created or resized, we use it
           to create an appropriately sized backing store pixmap and
           grid pixmap.
           """
        # Make the backing pixmap the size of our whole widget
        x, y, self.width, self.height = self.get_allocation()
        self.backingPixmap = gtk.gdk.Pixmap(self.window, self.width, self.height)

        # Create a grid pixmap as wide as our grid and as high as our window,
        # used to quickly initialize new areas of the graph with our grid pattern.
        self.gridPixmap = gtk.gdk.Pixmap(self.window, self.gridSize, self.height)
        self.initGrid(self.gridPixmap, self.gridSize, self.height)

        # Initialize the backing store
        self.drawGrid(0, self.width)

        # Since our graph has a new size and the backing store has been blown away,
        # we need to invalidate the old pen vectors lest we get funky lines connecting
        # each channel's pre-resize position with it's new pen position
        for i in range(len(self.channels)):
            self.channelPenVectors[i] = None
        return gtk.TRUE

    def initGrid(self, drawable, width, height):
        """Draw our grid on the given drawable"""
        background = self.get_style().light_gc[gtk.STATE_NORMAL]
        gridColor = self.get_style().mid_gc[gtk.STATE_NORMAL]

        drawable.draw_rectangle(background, gtk.TRUE, 0, 0, width, height)

        # Horizontal grid lines
        for y in range(0, height, self.gridSize):
            drawable.draw_rectangle(gridColor, gtk.TRUE, 0, y, width, 1)

        # Vertical grid lines
        for x in range(0, width, self.gridSize):
            drawable.draw_rectangle(gridColor, gtk.TRUE, x, 0, 1, height)

    def drawGrid(self, x, width):
        """Draw grid lines on our backing store, using the current gridPhase,
           to the rectangle (x, 0, width, self.height)
           """
        srcOffset = (x + int(self.gridPhase)) % self.gridSize
        gc = self.get_style().fg_gc[gtk.STATE_NORMAL]

        if srcOffset > 0:
            # Draw the first partial grid column
            columnWidth = self.gridSize - srcOffset
            if columnWidth > width:
                columnWidth = width
            self.backingPixmap.draw_drawable(gc, self.gridPixmap, srcOffset, 0, x, 0,
                                             columnWidth, self.height)
            x += columnWidth
            width -= columnWidth

        while width > 0:
            # Draw each remaining full or partial grid column
            columnWidth = self.gridSize
            if columnWidth > width:
                columnWidth = width
            self.backingPixmap.draw_drawable(gc, self.gridPixmap, 0, 0, x, 0,
                                             columnWidth, self.height)
            x += self.gridSize
            width -= self.gridSize

    def integrate(self, dt):
        """Update the graph, given a time delta from the last call to this function"""
        # Calculate the new gridPhase and the number of freshly exposed pixels,
        # correctly accounting for subpixel gridPhase changes.
        oldGridPhase = self.gridPhase
        self.gridPhase += dt * self.scrollRate
        newPixels = int(self.gridPhase) - int(oldGridPhase)
        self.gridPhase %= self.gridSize

        if newPixels > 0:
            # Scroll the backing store left by newPixels pixels
            self.backingPixmap.draw_drawable(self.get_style().fg_gc[gtk.STATE_NORMAL],
                                             self.backingPixmap, newPixels, 0, 0, 0,
                                             self.width - newPixels, self.height)

            # Draw a blank grid in the new area
            self.drawGrid(self.width - newPixels, newPixels)

            # Update the old pen vectors for active channels, and draw them
            for i in range(len(self.channels)):
                penVector = self.channelPenVectors[i]
                if penVector is not None:
                   self.channelPenVectors[i] = (penVector[0] - newPixels, penVector[1])
            self.graphChannels()
            self.oldChannels = self.channels[:]

            # Schedule an expose event to blit the whole backbuffer to the screen
            self.queue_draw_area(0, 0, self.width, self.height)

        else:
            # Even if we're not scrolling, we should update the graph if the channel
            # values have changed. This is especially necessary when the channels
            # are being updated much more often than the graph is scrolled.
            # Don't bother actually blitting this to the screen, since without a
            # scroll it should only affect the rightmost column of the graph.
            if self.channels != self.oldChannels:
                self.graphChannels()
                self.oldChannels = self.channels[:]

### The End ###

""" rtgraph.HScrollGraph

Abstract base class for graphs that scroll horizontally, revealing
new data on the right edge, with a grid background.
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

__all__ = ["HScrollGraph"]


class HScrollGraph(Graph):
    """A graph that shows time on the horizontal axis, multiple channels
       of data on the Y axis, and scrolls horizontally so current data
       is always on the right edge of the graph.

           gridSize: grid size, in pixels
         scrollRate: Graph scrolling rate, in pixels per second
       """
    def __init__(self, size=(384,128), channels=[], gridSize=32, scrollRate=80):
        Graph.__init__(self, size, channels)
        self.gridSize = gridSize
        self.scrollRate = scrollRate
        self.gridPhase = 0.0       # Number of pixels we've scrolled, modulo gridSize

    def graphChannel(self, channel):
        """Hook for graphing the current values of each channel. Called for
           every channel each time the graph is scrolled, and called on one
           channel each time that channel changes.
           """
        pass

    def drawBackground(self):
        """Draw our grid pixmap and backing store"""
        # Create a grid pixmap as wide as our grid and as high as our window,
        # used to quickly initialize new areas of the graph with our grid pattern.
        self.gridPixmap = gtk.gdk.Pixmap(self.window, self.gridSize, self.height)
        self.initGrid(self.gridPixmap, self.gridSize, self.height)

        # Initialize the backing store
        self.drawGrid(0, self.width)

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

            # Let subclasses update their positions to account for our scrolling
            self.exposedPixels(newPixels)

            # Graph all channels
            for channel in self.channels:
                # Effectively clear the channel's "dirty flag", so in deciding
                # whether a draw is necessary if we're not scrolling we don't account
                # for changes that will be drawn now.
                channel.hasChanged(self)
                self.graphChannel(channel)

            # Schedule an expose event to blit the whole backbuffer to the screen
            self.queue_draw_area(0, 0, self.width, self.height)

        else:
            # Even if we're not scrolling, we should update the graph if the channel
            # values have changed. This is especially necessary when the channels
            # are being updated much more often than the graph is scrolled.
            # Don't bother actually blitting this to the screen, since without a
            # scroll it should only affect the rightmost column of the graph.
            for channel in self.channels:
                if channel.hasChanged(self):
                    self.graphChannel(channel)

    def exposedPixels(self, nPixels):
        """Called when the graph scrolls, with the number of pixels it has scrolled by.
           Used as a hook for updating drawing coordinates in subclasses.
           """
        pass

### The End ###

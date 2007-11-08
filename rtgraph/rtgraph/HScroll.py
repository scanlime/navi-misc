from __future__ import division
from Graph import PolledGraph
import gtk, os

class HScrollGraph(PolledGraph):
    """A graph that shows time on the horizontal axis, multiple channels
       of data on the Y axis, and scrolls horizontally so current data
       is always on the right edge of the graph.

           gridSize: grid size, in pixels
         scrollRate: Graph scrolling rate, in pixels per second
       """
    def __init__(self,
                 size         = (384,128),
                 channels     = [],
                 gridSize     = 32,
                 scrollRate   = 50,
                 pollInterval = 10,
                 bgColor      = None,
                 gridColor    = None,
                 ):
        PolledGraph.__init__(self, size, channels, pollInterval, bgColor, gridColor)
        self.gridSize = gridSize
        self.scrollRate = scrollRate
        self.gridPhase = 0.0       # Number of pixels we've scrolled, modulo gridSize

        # Normally we copy from backbuffer to backbuffer when scrolling.
        # This breaks under the win32 implementation of GDK, so use a temporary
        # buffer then.
        self.useTemporaryPixmap = os.name == "nt"

    def resized(self):
        PolledGraph.resized(self)
        if self.useTemporaryPixmap:
            self.tempPixmap = gtk.gdk.Pixmap(self.window, self.width, self.height)

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
        drawable.draw_rectangle(self.bgGc, True, 0, 0, width, height)

        # Horizontal grid lines
        for y in range(0, height, self.gridSize):
            drawable.draw_rectangle(self.gridGc, True, 0, y, width, 1)

        # Vertical grid lines
        for x in range(0, width, self.gridSize):
            drawable.draw_rectangle(self.gridGc, True, x, 0, 1, height)

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

        # Can't update if we aren't mapped
        if not (self.width and self.height):
            return

        # Calculate the new gridPhase and the number of freshly exposed pixels,
        # correctly accounting for subpixel gridPhase changes.
        oldGridPhase = self.gridPhase
        self.gridPhase += dt * self.scrollRate
        newPixels = int(self.gridPhase) - int(oldGridPhase)
        self.gridPhase %= self.gridSize

        if newPixels > 0:
            # Scroll the backing store left by newPixels pixels

            if self.useTemporaryPixmap:
                # We can't safely copy from and to the same pixmap, copy this
                # via a temporary off-screen buffer.
                self.tempPixmap.draw_drawable(self.get_style().fg_gc[gtk.STATE_NORMAL],
                                              self.backingPixmap, newPixels, 0, 0, 0,
                                              self.width - newPixels, self.height)
                self.backingPixmap.draw_drawable(self.get_style().fg_gc[gtk.STATE_NORMAL],
                                                 self.tempPixmap, 0, 0, 0, 0,
                                                 self.width - newPixels, self.height)
            else:
                # Copy directly from and to the backbuffer
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
            for channel in self.channels:
                if channel.hasChanged(self):
                    self.graphChannel(channel)
            self.queue_draw_area(self.width-1, 0, 1, self.height)

    def exposedPixels(self, nPixels):
        """Called when the graph scrolls, with the number of pixels it has scrolled by.
           Used as a hook for updating drawing coordinates in subclasses.
           """
        pass

    def getTweakControls(self):
        import Tweak
        return PolledGraph.getTweakControls(self) + [
            Tweak.Quantity(self, 'scrollRate', range=(0,200), name="Scroll Rate")
            ]

class HScrollLineGraph(HScrollGraph):
    """A horizontally scrolling real-time line plot.
       Expects scalar values within the given range.
       """
    def __init__(self,
                 size         = (384,128),
                 channels     = [],
                 gridSize     = 32,
                 scrollRate   = 50,
                 range        = (0,1),
                 pollInterval = 10,
                 bgColor      = None,
                 gridColor    = None,
                 ):
        HScrollGraph.__init__(self, size, channels, gridSize,
                              scrollRate, pollInterval, bgColor, gridColor)
        self.range = range
        self.penVectors = {}

    def graphChannel(self, channel):
        value = channel.getValue()
        if value is None:
            return

        # Scale the channel value to match a range of (0,1)
        scaled = (value - self.range[0]) / (self.range[1] - self.range[0])
        scaled = min(1.5, max(-0.5, scaled))

        # Calculate a current pen position, always at the right side of the graph
        penVector = (self.width-1, int((self.height-1) * (1-scaled)))

        # If we have both a new pen vector and an old pen vector, we can draw a line
        if self.penVectors.has_key(channel):
            oldPenVector = self.penVectors[channel]
            self.graphChannelLine(channel, oldPenVector, penVector)

        # Store the pen vector for later
        self.penVectors[channel] = penVector

    def graphChannelLine(self, channel, oldPenVector, penVector):
        self.backingPixmap.draw_line(channel.getGC(self),
                                     oldPenVector[0], oldPenVector[1],
                                     penVector[0], penVector[1])

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

class HScrollAreaGraph(HScrollLineGraph):
    """A horizontally scrolling real-time filled area plot."""
    def graphChannelLine(self, channel, oldPenVector, penVector):
        self.backingPixmap.draw_polygon(channel.getGC(self), True, (
            (oldPenVector[0], oldPenVector[1]),
            (penVector[0], penVector[1]),
            (penVector[0], self.height-1),
            (oldPenVector[0], self.height-1)))

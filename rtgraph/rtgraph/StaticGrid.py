from Graph import PolledGraph
import gtk

class StaticGridGraph(PolledGraph):
    """A graph with an unchanging grid, cached in a pixmap and blitted during each 'frame'"""
    def __init__(self, *args, **kwargs):
        PolledGraph.__init__(self, *args, **kwargs)
        self.graphedChannels = []

    def graphChannel(self, channel):
        """Draw the given channel on the backbuffer.
           This must be implemented by subclasses.
           """
        pass

    def drawBackground(self):
        """Draws our grid after changing sizes. Initializes the grid to
           a separate pixmap, then draws that to the backbuffer,
           so the grid can be quickly redrawn for each frame.
           """
        self.gridPixmap = gtk.gdk.Pixmap(self.window, self.width, self.height)
        self.initGrid(self.gridPixmap, self.width, self.height)
        self.blitGrid()

    def initGrid(self, drawable, width, height):
        """Draw a grid to the given drawable at the given size.
           To be implemented by subclasses.
           """
        pass

    def blitGrid(self):
        """Blit our grid pixmap to the backbuffer, erasing current channel lines"""
        self.backingPixmap.draw_drawable(self.bgGc, self.gridPixmap, 0,0,0,0, self.width, self.height)

    def integrate(self, dt):
        """Updates any channels that have changed"""
        changedChannels = [channel for channel in self.channels if channel.hasChanged(self)]

        if changedChannels or self.channels != self.graphedChannels:
            self.blitGrid()
            for channel in changedChannels:
                self.graphChannel(channel)
            self.queue_draw_area(0, 0, self.width, self.height)
            self.graphedChannels = list(self.channels)

    def resized(self):
        """After resizing, immediately redraw all channels to the backing pixmap"""
        for channel in self.channels:
            self.graphChannel(channel)

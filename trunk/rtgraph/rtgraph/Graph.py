import gtk, time, gobject

class Graph(gtk.DrawingArea):
    """An abstract animated graph widget. Provides double-buffering,
       basic event handlers, and a common method for dealing with channels.
       """
    def __init__(self,
                 size         = None,
                 channels     = None,
                 bgColor      = None,
                 gridColor    = None,
                 ):
        gtk.DrawingArea.__init__(self)
        if size:
            self.set_size_request(*size)
        self.bgColor = bgColor
        self.gridColor = gridColor
        self.channels = channels or []

        # Handle configure (resize) and expose events
        self.connect("expose_event", self.gtkExposeEvent)
        self.connect("configure_event", self.gtkConfigureEvent)
        self.set_events(gtk.gdk.EXPOSURE_MASK)

        # Until we've been mapped onto the screen and configured by gtk,
        # our width and height are undefined
        self.width = self.height = None

    def makeColorGC(self, color):
        """Make a new GC with the given (red, green, blue) color
           as the foreground color.
           """
        gdkColor = self.get_colormap().alloc_color(*[int(c * 65535) for c in color])
        return self.window.new_gc(foreground=gdkColor)

    def initStyle(self):
        """Setup colors important for this widget, but not specific
           to any one input channel.
           """
        if self.bgColor:
            # Use a specific color
            self.bgGc = self.makeColorGC(self.bgColor)
        else:
            # Default gtk 'light' color
            self.bgGc = self.get_style().light_gc[gtk.STATE_NORMAL]

        if self.gridColor:
            # Use a specific color
            self.gridGc = self.makeColorGC(self.gridColor)
        else:
            # Default gtk 'mid' color
            self.gridGc = self.get_style().mid_gc[gtk.STATE_NORMAL]

    def gtkConfigureEvent(self, widget=None, event=None):
        """Called when the widget is created or resized, we use it
           to create an appropriately sized backing store pixmap and
           grid pixmap.
           """
        x, y, self.width, self.height = self.get_allocation()
        self.initStyle()

        # Make the backing pixmap the size of our whole widget
        self.backingPixmap = gtk.gdk.Pixmap(self.window, self.width, self.height)
        self.drawBackground()

        # Any extra resize handling the subclass needs
        self.resized()
        return True

    def gtkExposeEvent(self, widget, event):
        """Redraw the damaged area of our widget using the backing store"""
        x , y, width, height = event.area
        widget.window.draw_drawable(widget.get_style().fg_gc[gtk.STATE_NORMAL],
                                    self.backingPixmap, x, y, x, y, width, height)
        return False

    def drawBackground(self):
        """Hook for subclasses to initialize backingPixmap after a resize"""
        pass

    def resized(self):
        """Hook for extra processing to be done after a resize"""
        pass

    def integrate(self, dt):
        """Hook to animate this graph, given the delta-t since the last call to integrate()"""
        pass

    def getTweakControls(self):
        """Returns a list of tweak controls for tweakable parts of this widget"""
        return []

class PolledGraph(Graph):
    """An abstract animated graph that polls its channels for information."""
    def __init__(self,
                 size           = None,
                 channels       = None,
                 pollInterval   = 10,
                 bgColor        = None,
                 gridColor      = None,
                 ):
        Graph.__init__(self, size, channels, bgColor, gridColor)

        # Set up a timer that gets called frequently to handle
        # updating the graph if necessary.
        self.gtkTimeout = None
        self.lastUpdateTime = None
        self.setPollInterval(pollInterval)

    def setPollInterval(self, interval):
        """Set the minimum interval between calls to integrate(),
           in milliseconds. This is the reciprocal of the maximum frame
           rate. An interval of None will disable calls to integrate().
           """
        if self.gtkTimeout:
            gtk.timeout_remove(self.gtkTimeout)
        if interval is None:
            self.gtkTimeout = None
        else:
            self.gtkTimeout = gobject.timeout_add(interval, self.timerHandler)

    def timerHandler(self):
        """Called frequently to update the graph. This calculates the delta-t
           and passes on the real work to integrate()
           """
        now = time.time()
        if self.lastUpdateTime is not None:
            self.integrate(now - self.lastUpdateTime)
        self.lastUpdateTime = now

        # Keep calling this handler, rather than treating it as a one-shot timer
        return True

class NotifiedGraph(Graph):
    """An abstract animated graph that is notified by its channels when there is new information."""
    def __init__(self,
                 size           = None,
                 channels       = None,
                 bgColor        = None,
                 gridColor      = None,
                 ):
        Graph.__init__(self, size, channels, bgColor, gridColor)

    def notifyData(self, channel):
        self.update(channel)

    def update(self, channel):
        """Hook for subclasses to redraw the data they're interested in"""
        pass

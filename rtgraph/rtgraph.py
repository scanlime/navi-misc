""" rtgraph

A python package providing several widgets for graphing data
in real-time, using PyGTK, and UI components for controlling
the graphs.

  rtgraph real-time graphing package for PyGTK
  Copyright (C) 2003 Micah Dowty <micahjd@users.sourceforge.net>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
"""
from __future__ import generators, division
import time, gtk, math, gobject

__version__ = "pre-0.71"

__all__ = ["Channel", "FunctionChannel", "TimeVariable", "Graph",
           "StaticGridGraph", "HScrollGraph", "HScrollLineGraph",
           "HScrollAreaGraph", "IsometricVectorGraph", "PolarVectorGraph",
           "ChannelList", "GraphUI", 'GraphUIWindow',
           ]


class Channel(object):
    """An analog reading that varies over time, and information about drawing the channel.
       The 'value' property gets and sets this channel's value, updating its timestamp
       when set.
       """
    def __init__(self, value=None, color=(0,0,1), name=None):
        self._value = value
        self._timestamp = None
        self.timeMap = {}
        self.name = name
        self.setColor(color)

    def getValue(self):
        """Retrieves the current value, automatically called when accessing the 'value' property"""
        return self._value

    def setValue(self, value, timestamp=None):
        """Set the current value, by default giving it the current time as a timestamp"""
        if timestamp is None:
            timestamp = time.time()
        self._value = value
        self._timestamp = timestamp


    # Create wrappers around getValue/setValue so if subclasses override them,
    # the property still points to them properly.
    def _getValue(self):
        return self.getValue()
    def _setValue(self, value):
        return self.setValue(value)
    value = property(_getValue, _setValue)

    def getTimestamp(self):
        """Return the time at which this channel was last updated, to determine
           if there is new data available. Returns None if the value has not been set.
           """
        return self._timestamp

    def getColor(self):
        """Returns a color to graph this channel in, as a (red,green,blue) tuple with
           values between 0 and 1.
           """
        return self._color

    def setColor(self, color):
        """Change this channel's color, invalidating our GC cache"""
        self._color = tuple(color)
        self.gcMap = {}

    color = property(getColor, setColor)

    def getStyle(self, widget):
        """Returns a dictionary of keyword arguments to use when constructing a GC
           to draw this channel in.
           """
        color = self.getColor()
        gdkColor = widget.get_colormap().alloc_color(*[int(c * 65535) for c in color])
        return {'foreground': gdkColor}

    def getGC(self, widget):
        """For the given widget, return a GC for drawing this channel. Caches the GCs
           in our gcMap.
           """
        try:
            return self.gcMap[widget]
        except KeyError:
            gc = widget.window.new_gc(**self.getStyle(widget))
            self.gcMap[widget] = gc
            return gc

    def hasChanged(self, obj):
        """Determine whether this channel has changed, from the point of view of 'obj'.
           This returns True only once for a given value of 'obj' after this channel's
           timestamp has changed.
           """
        if not self.timeMap.has_key(obj):
            # obj has never seen this channel yet
            return True

        if self.timeMap[obj] == self._timestamp:
            return False
        else:
            self.timeMap[obj] = self._timestamp
            return True

    def __str__(self):
        """Returns the channel's name, if it has one, or the repr() otherwise"""
        if self.name is not None:
            return self.name
        else:
            return repr(self)

    def autoColor(self, i=None):
        """Automatically color this channel, returning an iterator that can be
           passed to autoColor instead of None to continue coloring other channels.
           For example:

              i = None
              for channel in channels:
                  i = channel.autoColor(i)
           """
        # If we don't have an iterator from before, start a fresh one
        if i is None:
            i = self.colorGenerator()
        self.color = i.next()
        return i

    def colorGenerator(self):
        """A generator that indefinitely yields color values to use when
           autocoloring channels.
           """
        import colorsys
        while True:
            for luma in (0.9, 0.5):
                for hue in (0.66, 0, 0.33, 0.75):
                    yield colorsys.hsv_to_rgb(hue, 1, luma)

    def strValue(self):
        """Get a string representation of the current value. This works
           well on floats, tuples, and strings, but if you need other
           representations it's easy to override this in a Channel subclass.
           """
        return limitFloatPrecision(self.value, 4)


def limitFloatPrecision(v, d):
    """Return a string representation of v with at most d digits of precision.
       Operates recursively on tuples/lists, and uses repr() for other data types.
       """
    if type(v) == float:
        return ("%%.0%df" % d) % v
    elif type(v) == tuple:
        return "(%s)" % ", ".join([limitFloatPrecision(i, d) for i in v])
    elif type(v) == list:
        return "[%s]" % ", ".join([limitFloatPrecision(i, d) for i in v])
    else:
        return repr(v)


class FunctionChannel(Channel):
    """A channel that evaluates a function, specified as a Python expression

       fString : The function to evaluate, as a string
       name    : The name to give this channel, None to use the function string
       modules : Modules to import the full contents of into globals
       globals : Extra items to place in the function's global namespace
       locals  : Extra items to place in the function's local namespace
       vars    : A dict of callables that are called and placed in the function's
                 global namespace before each evaluation.
       """
    def __init__(self,
                 fString,
                 name     = None,
                 modules  = ('math', 'random'),
                 globals  = {},
                 locals   = {},
                 vars     = {}
                 ):
        if name is None:
            name = fString
        self.globals = dict(globals)
        self.locals = dict(locals)
        self.vars = dict(vars)
        self.fString = fString
        Channel.__init__(self, name=fString)

        # Import the specified modules into globals
        for module in modules:
            self.globals.update(__import__(module).__dict__)

    def getValue(self):
        # Evaluate vars
        for var, callable in self.vars.iteritems():
            self.globals[var] = callable()

        return eval(self.fString, self.globals, self.locals)


class TimeVariable:
    """A callable class that returns the number of seconds since it was created.
       This is helpful for use with FunctionChannel's variables feature.
       """
    def __init__(self):
        self.tInitial = time.time()

    def __call__(self):
        return time.time() - self.tInitial


class Graph(gtk.DrawingArea):
    """An abstract animated graph widget. Provides double-buffering,
       basic event handlers, and a common method for dealing with channels.
       """
    def __init__(self,
                 size         = None,
                 channels     = [],
                 pollInterval = 10,
                 bgColor      = None,
                 gridColor    = None,
                 ):
        gtk.DrawingArea.__init__(self)
        if size:
            self.set_size_request(*size)
        self.bgColor = bgColor
        self.gridColor = gridColor
        self.channels = channels

        # Handle configure (resize) and expose events
        self.connect("expose_event", self.gtkExposeEvent)
        self.connect("configure_event", self.gtkConfigureEvent)
        self.set_events(gtk.gdk.EXPOSURE_MASK)

        # Set up a timer that gets called frequently to handle
        # updating the graph if necessary.
        self.gtkTimeout = None
        self.lastUpdateTime = None
        self.setPollInterval(pollInterval)

        # Until we've been mapped onto the screen and configured by gtk,
        # our width and height are undefined
        self.width = self.height = None

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
            self.gtkTimeout = gtk.timeout_add(interval, self.timerHandler)

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
        return gtk.TRUE

    def gtkExposeEvent(self, widget, event):
        """Redraw the damaged area of our widget using the backing store"""
        x , y, width, height = event.area
        widget.window.draw_drawable(widget.get_style().fg_gc[gtk.STATE_NORMAL],
                                    self.backingPixmap, x, y, x, y, width, height)
        return gtk.FALSE

    def timerHandler(self):
        """Called frequently to update the graph. This calculates the delta-t
           and passes on the real work to integrate()
           """
        now = time.time()
        if self.lastUpdateTime is not None:
            self.integrate(now - self.lastUpdateTime)
        self.lastUpdateTime = now

        # Keep calling this handler, rather than treating it as a one-shot timer
        return gtk.TRUE

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


class StaticGridGraph(Graph):
    """A graph with an unchanging grid, cached in a pixmap and blitted during each 'frame'"""
    def __init__(self, *args, **kwargs):
        Graph.__init__(self, *args, **kwargs)
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


class HScrollGraph(Graph):
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
        Graph.__init__(self, size, channels, pollInterval, bgColor, gridColor)
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
        drawable.draw_rectangle(self.bgGc, gtk.TRUE, 0, 0, width, height)

        # Horizontal grid lines
        for y in range(0, height, self.gridSize):
            drawable.draw_rectangle(self.gridGc, gtk.TRUE, 0, y, width, 1)

        # Vertical grid lines
        for x in range(0, width, self.gridSize):
            drawable.draw_rectangle(self.gridGc, gtk.TRUE, x, 0, 1, height)

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
        return Graph.getTweakControls(self) + [
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
        self.backingPixmap.draw_polygon(channel.getGC(self), gtk.TRUE, (
            (oldPenVector[0], oldPenVector[1]),
            (penVector[0], penVector[1]),
            (penVector[0], self.height-1),
            (oldPenVector[0], self.height-1)))


class IsometricVectorGraph(StaticGridGraph):
    """A graph that shows multiple 3D cartesian vectors as dots in
       an isometric grid.
       Expects channel values to be 3-tuples of values in the range [0,1]
       """
    def __init__(self,
                 size         = (384,384),
                 channels     = [],
                 pollInterval = 10,
                 bgColor      = None,
                 gridColor    = None,
                 ):
        StaticGridGraph.__init__(self, size, channels, pollInterval,
                                 bgColor, gridColor)

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


class PolarVectorGraph(StaticGridGraph):
    """A graph that shows multiple polar vectors, lines superimposed
       on a polar grid.
       Expects channel values to be (magnitude, angle) tuples, with
       magnitude in the range [0,1] and angle in radians.
       """
    def __init__(self,
                 size         = (128,128),
                 channels     = [],
                 pollInterval = 10,
                 bgColor      = None,
                 gridColor    = None,
                 ):
        StaticGridGraph.__init__(self, size, channels, pollInterval,
                                 bgColor, gridColor)

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


class ChannelList(gtk.TreeView):
    """A list of available channels, inserted into the given graph
       widget when a 'visible' checkbox is set.

       Automatically generates new colors for each channel if autoColor is true.
       If valueUpdateInterval is specified, the channel values are displayed
       in the list and updated every valueUpdateInterval milliseconds.
       """
    def __init__(self, graph, channels, autoColor=True, valueUpdateInterval=None):
        self.graph = graph
        self.channels = channels
        self.valueUpdateInterval = valueUpdateInterval

        if autoColor:
            i = None
            for channel in channels:
                i = channel.autoColor(i)

        self.initModel()
        gtk.TreeView.__init__(self, self.model)
        self.initView()

        # Filling the model requires creating pixmaps for the color samples,
        # which needs a valid self.window. Delay filling the model until after
        # this widget has been realized.
        self.modelFilled = False
        self.connect_after("realize", self.gtkRealizeSignal)

    def gtkRealizeSignal(self, widget=None, event=None):
        """Called after this widget has been realized (its gdk resources created)
           so that self.fillModel can be run with a valid window.
           """
        if not self.modelFilled:
            self.fillModel()
        return gtk.TRUE

    def initModel(self):
        """Create the model representing the data in this list"""
        self.model = gtk.ListStore(gobject.TYPE_PYOBJECT,  # (0) Channel instance
                                   gobject.TYPE_STRING,    # (1) Channel name
                                   gobject.TYPE_BOOLEAN,   # (2) Visibility flag
                                   gobject.TYPE_BOOLEAN,   # (3) Activatable flag
                                   gobject.TYPE_OBJECT,    # (4) Color sample pixbuf
                                   gobject.TYPE_STRING,    # (5) Channel value
                                   )

    def fillModel(self):
        """Fills the model with data, must be called after self.window is valid"""
        for channel in self.channels:
            i = self.model.append()
	    self.model.set(i,
                0, channel,
                1, str(channel),
                2, gtk.FALSE,
                3, gtk.TRUE,
                4, self.makeColorSamplePixbuf(channel),
                5, "",
		)
        self.modelFilled = True

        if self.valueUpdateInterval:
            # Give all values one initial update, then start a callback for updating them regularly.
            self.oldValueStr = {}
            for channel in self.channels:
                self.oldValueStr[channel] = None
            self.updateValues()
            self.valueUpdateTimeout = gtk.timeout_add(self.valueUpdateInterval, self.updateValues)

    def initView(self):
        """Initializes all columns in the model viewed by this class"""
        # Read/write toggle for channel visibility
        renderer = gtk.CellRendererToggle()
        renderer.connect('toggled', self.visibilityToggleCallback, self.model)
        self.append_column(gtk.TreeViewColumn("Visible", renderer, active=2, activatable=3))

        # Show the channel color
        self.append_column(gtk.TreeViewColumn("Color", gtk.CellRendererPixbuf(), pixbuf=4))

        # Show the channel name
        self.append_column(gtk.TreeViewColumn("Name", gtk.CellRendererText(), text=1))

        # Show the current channel value
        if self.valueUpdateInterval:
            self.append_column(gtk.TreeViewColumn("Value", gtk.CellRendererText(), text=5))

    def visibilityToggleCallback(self, cell, path, model):
        """Callback triggered by clicking on a 'visible' checkbox.
           Toggles the value of the checkbox, then updates the graph
           to add or remove this channel.
           """
        i = model.get_iter((int(path,)))
        visible = not cell.get_active()
        model.set_value(i, 2, visible)
        channel = model.get_value(i, 0)

        if visible == gtk.TRUE:
            self.graph.channels.append(channel)
        else:
            self.graph.channels.remove(channel)

    def makeColorSamplePixbuf(self, channel, width=26, height=14):
        """Create a small pixbuf giving a color sample for the given channel"""
        # Create a pixmap first, since they're much easier to draw on
        pixmap = gtk.gdk.Pixmap(self.window, width, height)

        # Fill it with our channel color, and give it black and white borders
        # so it shows up on all backgrounds.
        # Note that due to some boneheaded decisions made in xlib near the dawn of time,
        # unfilled rectangles are actually 1 pixel wider in each dimension than the width
        # and height we give draw_rectangle.
        pixmap.draw_rectangle(channel.getGC(self), gtk.TRUE, 2, 2, width-4, height-4)
        pixmap.draw_rectangle(self.get_style().black_gc, gtk.FALSE, 1, 1, width-3, height-3)
        pixmap.draw_rectangle(self.get_style().white_gc, gtk.FALSE, 0, 0, width-1, height-1)

        # Convert it to a pixbuf
        pixbuf = gtk.gdk.Pixbuf(gtk.gdk.COLORSPACE_RGB, 0, 8, width, height)
        pixbuf.get_from_drawable(pixmap, self.window.get_colormap(), 0,0, 0,0, width, height)
        return pixbuf

    def updateValues(self):
        """Update the 'value' column for all channels"""
        row = 0
        for channel in self.channels:
            s = str(channel.strValue())
            # Only set the model value if our string has changed
            if s != self.oldValueStr[channel]:
                i = self.model.get_iter(row)
                self.model.set_value(i, 5, s)
                self.oldValueStr[channel] = s
            row += 1
        return gtk.TRUE


class GraphUI(gtk.VPaned):
    """A paned widget combining a specified graph with
       a ChannelList and a list of tweakable parameters.
       If the supplied graph is None, this creates a default HScrollLineGraph instance.

       Automatically generates new colors for each channel if autoColor is true.
       If valueUpdateInterval is specified, the channel values are displayed
       in the list and updated every valueUpdateInterval milliseconds.
       """
    def __init__(self, channels, graph=None, autoColor=True, valueUpdateInterval=None):
        self.autoColor = autoColor
        self.valueUpdateInterval = valueUpdateInterval
        if not graph:
            graph = HScrollLineGraph()
        self.graph = graph
        self.channels = channels
        gtk.VPaned.__init__(self)
        self.add1(self.createTopContents())
        self.add2(self.createBottomContents())

    def createTopContents(self):
        """Returns the widget to occupy the top of pane.
           By default, it is the graph.
           """
        return self.createGraphFrame()

    def createBottomContents(self):
        """Returns the widget to occupy the bottom pane.
           By default, this is the tweak controls on top
           of the channel list.
           """
        box = gtk.VBox(gtk.FALSE, 5)
        box.pack_start(self.createTweakList(), gtk.FALSE)
        box.pack_start(self.createChannelList())
        box.show()
        return box

    def createGraphFrame(self):
        """Create our container for the graph widget"""
        self.graph.show()
        frame = gtk.Frame()
        frame.set_shadow_type(gtk.SHADOW_IN)
        frame.add(self.graph)
        frame.show()
        return frame

    def createTweakList(self):
        """Create the widget holding our graph's tweakable settings"""
        import Tweak
        tweaks = Tweak.List(self.graph.getTweakControls())
        tweaks.show()
        return tweaks

    def createChannelList(self):
        """Create the channel list widget and a scrolling container for it"""
        self.channelList = ChannelList(self.graph, self.channels,
                                       autoColor = self.autoColor,
                                       valueUpdateInterval = self.valueUpdateInterval)
        self.channelList.show()

        scroll = gtk.ScrolledWindow()
        scroll.set_policy(gtk.POLICY_AUTOMATIC, gtk.POLICY_AUTOMATIC)
        scroll.add(self.channelList)
        scroll.show()

        frame = gtk.Frame()
        frame.add(scroll)
        frame.show()
        return frame


def GraphUIWindow(channels, graph=None, title=None, defaultSize=(400,400)):
    """Creates a window containing a GraphUI widget"""
    win = gtk.Window(gtk.WINDOW_TOPLEVEL)
    ui = GraphUI(channels, graph)
    if title:
        win.set_title(title)
    win.set_border_width(8)
    ui.show()
    win.add(ui)
    win.set_default_size(*defaultSize)
    win.show()
    return win

### The End ###

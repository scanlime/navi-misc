import gtk, gobject
from rtgraph import HScroll

class ChannelList(gtk.TreeView):
    """A list of available channels, inserted into the given graph
       widget when a 'visible' checkbox is set.

       Automatically generates new colors for each channel if autoColor is true.
       If valueUpdateInterval is specified, the channel values are displayed
       in the list and updated every valueUpdateInterval milliseconds.
       """
    def __init__(self, graph, channels,
                 autoColor = True,
                 valueUpdateInterval = None,
                 visibilityDefault = False):
        self.graph = graph
        self.channels = channels
        self.valueUpdateInterval = valueUpdateInterval
        self.visibilityDefault = visibilityDefault

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
        return True

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
                2, self.visibilityDefault,
                3, True,
                4, self.makeColorSamplePixbuf(channel),
                5, "",
        	)
            if self.visibilityDefault:
                self.graph.channels.append(channel)
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

        if visible == True:
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
        pixmap.draw_rectangle(channel.getGC(self), True, 2, 2, width-4, height-4)
        pixmap.draw_rectangle(self.get_style().black_gc, False, 1, 1, width-3, height-3)
        pixmap.draw_rectangle(self.get_style().white_gc, False, 0, 0, width-1, height-1)

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
        return True


class GraphUI(gtk.VPaned):
    """A paned widget combining a specified graph with
       a ChannelList and a list of tweakable parameters.
       If the supplied graph is None, this creates a default HScrollLineGraph instance.

       Automatically generates new colors for each channel if autoColor is true.
       If valueUpdateInterval is specified, the channel values are displayed
       in the list and updated every valueUpdateInterval milliseconds.
       """
    def __init__(self, channels,
                 graph               = None,
                 autoColor           = True,
                 valueUpdateInterval = None,
                 visibilityDefault   = False,
                 showTweakControls   = True
                 ):
        self.autoColor = autoColor
        self.valueUpdateInterval = valueUpdateInterval
        self.visibilityDefault = visibilityDefault
        self.showTweakControls = showTweakControls
        if not graph:
            graph = HScroll.HScrollLineGraph()
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
        box = gtk.VBox(False, 5)
        if self.showTweakControls:
            box.pack_start(self.createTweakList(), False)
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
                                       valueUpdateInterval = self.valueUpdateInterval,
                                       visibilityDefault = self.visibilityDefault)
        self.channelList.show()

        scroll = gtk.ScrolledWindow()
        scroll.set_policy(gtk.POLICY_AUTOMATIC, gtk.POLICY_AUTOMATIC)
        scroll.add(self.channelList)
        scroll.show()

        frame = gtk.Frame()
        frame.add(scroll)
        frame.show()
        return frame


def GraphUIWindow(channels,
                  graph=None,
                  title               = None,
                  defaultSize         = (400,400),
                  autoColor           = True,
                  valueUpdateInterval = None,
                  visibilityDefault   = False,
                  ):
    """Creates a window containing a GraphUI widget"""
    win = gtk.Window(gtk.WINDOW_TOPLEVEL)
    ui = GraphUI(channels, graph,
                 autoColor=autoColor,
                 valueUpdateInterval=valueUpdateInterval,
                 visibilityDefault=visibilityDefault)
    if title:
        win.set_title(title)
    win.set_border_width(8)
    ui.show()
    win.add(ui)
    win.set_default_size(*defaultSize)
    win.show()
    return win

### The End ###

""" rtgraph.UI

Gtk user interface elements for high-level control of graphs
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

import gtk
import gobject
from HScrollLineGraph import *
import Tweak

__all__ = ['ChannelList', 'GraphUI', 'GraphUIWindow']


class ChannelList(gtk.TreeView):
    """A list of available channels, inserted into the given graph
       widget when a 'visible' checkbox is set.
       Automatically generates new colors for each channel if autoColor is true.
       """
    def __init__(self, graph, channels, autoColor=True, valueUpdateInterval=50):
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
            self.model.append(row=(
                channel,
                str(channel),
                gtk.FALSE,
                gtk.TRUE,
                self.makeColorSamplePixbuf(channel),
                "",
                ))
        self.modelFilled = True

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
        self.append_column(gtk.TreeViewColumn("Value", gtk.CellRendererText(), text=5))

    def visibilityToggleCallback(self, cell, path, model):
        """Callback triggered by clicking on a 'visible' checkbox.
           Toggles the value of the checkbox, then updates the graph
           to add or remove this channel.
           """
        i = model.get_iter(path)
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
       If the supplied graph is None, this creates a default
       HScrollLineGraph instance.
       """
    def __init__(self, channels, graph=None):
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
        frame.add(self.graph)
        frame.show()
        return frame

    def createTweakList(self):
        """Create the widget holding our graph's tweakable settings"""
        tweaks = Tweak.List(self.graph.getTweakControls())
        tweaks.show()
        return tweaks

    def createChannelList(self):
        """Create the channel list widget and a scrolling container for it"""
        self.channelList = ChannelList(self.graph, self.channels)
        self.channelList.show()

        scroll = gtk.ScrolledWindow()
        scroll.set_policy(gtk.POLICY_AUTOMATIC, gtk.POLICY_AUTOMATIC)
        scroll.add(self.channelList)
        scroll.show()

        frame = gtk.Frame()
        frame.add(scroll)
        frame.show()
        return frame


def GraphUIWindow(channels, graph=None, title=None):
    """Creates a window containing a GraphUI widget"""
    win = gtk.Window(gtk.WINDOW_TOPLEVEL)
    ui = GraphUI(channels, graph)
    if title:
        win.set_title(title)
    win.set_border_width(8)
    ui.show()
    win.add(ui)
    return win

### The End ###

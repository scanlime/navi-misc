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
import colorsys
import Tweak

__all__ = ['ChannelList', 'GraphUI', 'GraphUIWindow']


def autoColorChannels(channels):
    """Automatically assign hopefully-unique colors to the given channels"""
    # List of colors to choose from, the most preferable ones on the top
    availableColors = [
        (0,   0,   1  ),
        (1,   0,   0  ),
        (0,   0.8, 0  ),
        (0,   0,   0.5),
        (0.5, 0,   0  ),
        (1,   0.5, 0  ),
        (0,   0.5, 0.5),
        ]

    colors = []
    for channel in channels:
        # If we do run out of colors, loop through the ones we have
        if not colors:
            colors = availableColors[:]
        channel.color = colors[0]
        del colors[0]


class ChannelList(gtk.TreeView):
    """A list of available channels, inserted into the given graph
       widget when a 'visible' checkbox is set.
       Automatically generates new colors for each channel if autoColor is true.
       """
    def __init__(self, graph, availableChannels, autoColor=True):
        self.graph = graph

        if autoColor:
            autoColorChannels(availableChannels)

        # Create the model behind our TreeView:
        model = gtk.ListStore(gobject.TYPE_PYOBJECT,  # (0) Channel instance
                              gobject.TYPE_STRING,    # (1) Channel name
                              gobject.TYPE_BOOLEAN,   # (2) Visibility flag
                              gobject.TYPE_BOOLEAN,   # (3) Activatable flag
                              )
        for channel in availableChannels:
            i = model.append(row=(
                channel,
                str(channel),
                gtk.FALSE,
                gtk.TRUE,
                ))

        # Now we can initialize this class, the view
        gtk.TreeView.__init__(self, model)

        # Read/write toggle for channel visibility
        renderer = gtk.CellRendererToggle()
        renderer.connect('toggled', self.visibilityToggleCallback, model)
        self.append_column(gtk.TreeViewColumn("Visible", renderer, active=2, activatable=3))

        # Show the channel name
        self.append_column(gtk.TreeViewColumn("Name", gtk.CellRendererText(), text=1))

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


class GraphUI(gtk.VPaned):
    """A paned widget combining a specified graph with
       a ChannelList and a list of tweakable parameters.
       If the supplied graph is None, this creates a default
       HScrollLineGraph instance.
       """
    def __init__(self, availableChannels, graph=None):
        if not graph:
            graph = HScrollLineGraph()
        self.graph = graph
        self.availableChannels = availableChannels
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
        self.channelList = ChannelList(self.graph, self.availableChannels)
        self.channelList.set_size_request(256, 128)
        self.channelList.show()
        scroll = gtk.ScrolledWindow()
        scroll.set_policy(gtk.POLICY_AUTOMATIC, gtk.POLICY_AUTOMATIC)
        scroll.add(self.channelList)
        scroll.show()
        frame = gtk.Frame()
        frame.add(scroll)
        frame.show()
        return frame


def GraphUIWindow(availableChannels, graph=None, title=None):
    """Creates a window containing a GraphUI widget"""
    win = gtk.Window(gtk.WINDOW_TOPLEVEL)
    ui = GraphUI(availableChannels, graph)
    if title:
        win.set_title(title)
    win.set_border_width(8)
    ui.show()
    win.add(ui)
    win.show()

### The End ###

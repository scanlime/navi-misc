""" rtgraph.Graph

The base class on which all rtgraph widgets are built.
Provides infrastructure common to all animated graphs,
and a common interface for assigning Channel instances.
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

import time, gtk

__all__ = ["Graph"]


class Graph(gtk.DrawingArea):
    """An abstract animated graph widget. Provides double-buffering,
       basic event handlers, and a common method for dealing with channels.
       """
    def __init__(self, size=None, channels=[], pollInterval=10):
        gtk.DrawingArea.__init__(self)
        if size:
            self.set_size_request(*size)
        self.channels = channels

        # Handle configure (resize) and expose events
        self.connect("expose_event", self.gtkExposeEvent)
        self.connect("configure_event", self.gtkConfigureEvent)
        self.set_events(gtk.gdk.EXPOSURE_MASK)

        # Set up a timer that gets called frequently to handle
        # updating the graph if necessary.
        self.gtkTimeout = gtk.timeout_add(pollInterval, self.timerHandler)
        self.lastUpdateTime = None

        # Until we've been mapped onto the screen and configured by gtk,
        # our width and height are undefined
        self.width = self.height = None

    def initStyle(self):
        """Setup colors important for this widget, but not specific
           to any one input channel.
           """
        self.bgGc = self.get_style().light_gc[gtk.STATE_NORMAL]
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

### The End ###

#!/usr/bin/env python
""" thermapplet.py

This is a Gnome applet to display the latest readings and graphs
from all sensors on a remote therm server. Requires PyGTK and Twisted.

To install, put GNOME_ThermApplet.server in /usr/lib/bonobo/servers.
Either put thermapplet.py somewhere in your path, or edit the .server
file's location attribute to point to it.

"""
#
# Wireless therm system
# Copyright (C) 2004-2005 Micah Dowty
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#

import pygtk
pygtk.require('2.0')

from twisted.internet import gtk2reactor
gtk2reactor.install()

from twisted.internet import defer, reactor
from twisted.web import client
import sys, os, random
import gnome.applet, gtk


class ThermSource:
    """An abstraction for a remote therm source, accessable over HTTP"""
    def __init__(self, server, name):
        self.server = server
        self.name = name

    def __str__(self):
        words = self.name.replace("_", " ").split()
        return " ".join([ word.capitalize() for word in words ])

    def getLatest(self, key, returnType=float):
        """Return one key from the latest packet on this source, via a Deferred"""
        result = defer.Deferred()
        client.getPage(self.server.url + "/source/%s/latest/%s" % (
            self.name, key)).addCallback(self._getLatest, result, returnType).addErrback(result.errback)
        return result

    def _getLatest(self, content, result, returnType):
        value = content.strip()
        try:
            value = returnType(value)
        except ValueError:
            value = None
        result.callback(value)

    def getGraph(self, graphType, interval='day', width=None, height=None):
        """Download a graph, optionally scaled into a thumbnail, and return the filename"""
        filename = "/tmp/therm-%d-%d" % (os.getpid(), abs(hash(random.random())))

        url = self.server.url + "/source/%s/graph/%s:%s" % (
            self.name, graphType, interval)
        if width and height:
            url += "/scale/%dx%d" % (width, height)
        elif width:
            url += "/scale/%dx" % width
        elif height:
            url += "/scale/x%d" % height

        result = defer.Deferred()
        client.downloadPage(url, filename).addCallback(
            self._getGraph, filename, result).addErrback(result.errback)
        return result

    def _getGraph(self, value, filename, result):
        result.callback(filename)


def degCtoF(degC):
    """Convert degrees centigrade to fahrenheit"""
    return degC * 9.0 / 5.0 + 32


class ThermServer:
    """An abstraction for our mod_python based therm server, running on
       an arbitrary URL. This can retrieve individual therm sources, or iterate
       over all available sources.
       """
    def __init__(self, url):
        while url[-1] == '/':
            url = url[:-1]
        self.url = url

    def iterSources(self):
        """Return, via a deferred, a list of all ThermSources"""
        result = defer.Deferred()
        client.getPage(self.url + "/source").addCallback(
            self._iterSources, result).addErrback(result.errback)
        return result

    def _iterSources(self, content, result):
        results = []
        for line in content.split("\n"):
            line = line.strip()
            if line:
                results.append(ThermSource(self, line))
        result.callback(results)


class ColumnSet:
    """A set of columns shared by multiple therm widgets"""
    def __init__(self, columns=('name', 'average', 'graph')):
        self.columns = columns
        self.sizeGroups = [gtk.SizeGroup(gtk.SIZE_GROUP_HORIZONTAL) for column in columns]


class ThermWidget(gtk.HBox):
    """A widget that shows the status of one therm source. Multiple therm
       widgets can be arranged into a table sharing one ColumnSet.
       """
    def __init__(self, source, columnSet, graphHeight=24):
        gtk.HBox.__init__(self, spacing=8)
        self.source = source
        self.columnSet = columnSet
        self.graphHeight = graphHeight
        self.widgets = []

        for index, name in enumerate(self.columnSet.columns):
            widget = getattr(self, "init_" + name)()
            self.widgets.append(widget)
            self.columnSet.sizeGroups[index].add_widget(widget)
            self.pack_start(widget)

        self.update()

    def update(self):
        """Start an asynchronous update for this widget"""
        for index, name in enumerate(self.columnSet.columns):
            getattr(self, "update_" + name)(self.widgets[index])

    def init_name(self):
        widget = gtk.Label(str(self.source))
        widget.set_alignment(0, 0.5)
        return widget

    def init_average(self):
        return gtk.Label()

    def init_graph(self):
        return gtk.Image()

    def update_name(self, widget):
        pass

    def update_average(self, widget):
        self.source.getLatest('average').addCallback(self._update_average, widget)

    def _update_average(self, temperature, widget):
	if temperature is None:
            widget.set_text("No data")
            return

        degrees = "\302\260"
        degF = "%.01f%sF" % (degCtoF(temperature), degrees)
        degC = "%.01f%sC" % (temperature, degrees)

        widget.set_markup("<big><b>%s</b> / %s</big>" % (degF, degC))

    def update_graph(self, widget):
        self.source.getGraph('temperature', height=self.graphHeight
                             ).addCallback(self._update_graph, widget)

    def _update_graph(self, filename, widget):
        try:
            widget.set_from_file(filename)
        finally:
            os.unlink(filename)


class ThermMenu(gtk.Menu):
    """A widget showing a menu of all therm sources"""
    def __init__(self, server, selectionCallback, updateInterval=60*10):
        gtk.Menu.__init__(self)
        self.server = server
        self.columns = ColumnSet()
        self.widgets = []
        self.selectionCallback = selectionCallback
        self.updateInterval = updateInterval

        self.retrievingSources = gtk.MenuItem("Retrieving sources...")
        self.add(self.retrievingSources)
        self.updateSources()

    def updateSources(self):
        self.retrievingSources.show()
        self.server.iterSources().addCallback(self.setSources)
        reactor.callLater(self.updateInterval, self.updateSources)

    def updateContent(self):
        for widget in self.widgets:
            widget.update()

    def setSources(self, sources):
        self.retrievingSources.hide()
        widgets = []
        for source in sources:
            menuItem = gtk.MenuItem()
            menuItem.connect_object("activate", self.selectionCallback, source)

            widget = ThermWidget(source, self.columns, graphHeight=48)
            widgets.append(widget)
            menuItem.add(widget)

            menuItem.show_all()
            self.append(menuItem)
        self.widgets = widgets


class ThermApplet:
    def __init__(self, applet, iid, url="http://navi.cx/therm", updateInterval=60):
        self.applet = applet
        self.server = ThermServer(url)

        self.rootMenu = gtk.MenuItem()
        self.currentWidget = None
        self.changeSource(ThermSource(self.server, 'outside'))
        self.rootMenu.show()

        self.evBox = gtk.EventBox()
        self.evBox.add(self.rootMenu)
        self.evBox.connect("button-press-event", self.clicked)
        self.applet.add(self.evBox)

        self.menu = ThermMenu(self.server, self.changeSource)
        self.rootMenu.set_submenu(self.menu)

        self.updateInterval = updateInterval
        self.updateCallback()

    def clicked(self, widget, event):
        if event.button == 1:
            self.menu.popup(None, self.rootMenu, self.positionMenu, event.button, event.time)

    def positionMenu(self, menu):
        widget = self.evBox
        origin = widget.window.get_origin()
        return (origin[0] + widget.allocation.x,
                origin[1] + widget.allocation.y + widget.allocation.height,
                1)

    def updateCallback(self):
        self.menu.updateContent()
        if self.currentWidget:
            self.currentWidget.update()
        reactor.callLater(self.updateInterval, self.updateCallback)

    def changeSource(self, source):
        if self.currentWidget is not None:
            self.rootMenu.remove(self.currentWidget)
        self.currentWidget = ThermWidget(source, ColumnSet(), graphHeight=24)
        self.currentWidget.show_all()
        self.rootMenu.add(self.currentWidget)


def ThermApplet_factory(applet, iid):
    ThermApplet(applet, iid)
    applet.show_all()
    return gtk.TRUE

if __name__ == "__main__":
    # As suggested by http://www.pygtk.org/articles/applets_arturogf/x106.html
    if len(sys.argv) == 2 and sys.argv[1] == "run-in-window":
        main_window = gtk.Window(gtk.WINDOW_TOPLEVEL)
        main_window.set_title("Python Applet")
        main_window.connect("destroy", gtk.mainquit)
        app = gnome.applet.Applet()
        ThermApplet_factory(app, None)
        app.reparent(main_window)
        main_window.show_all()
        reactor.run()
    else:
        reactor.startRunning()
        reactor.simulate()
        gnome.applet.bonobo_factory("OAFIID:GNOME_ThermApplet_Factory",
                                    gnome.applet.Applet.__gtype__,
                                    "thermapplet", "0", ThermApplet_factory)

### The End ###

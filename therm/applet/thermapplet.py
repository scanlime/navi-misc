#!/usr/bin/env python
#
# Gnome applet to show therm readings in real time.
# Requires PyGTK 2 and gnome-python.
#
# Still quite experimental. The outdoor temperature
# display usually works, but the dropdown menu that
# is supposed to show all temperature readings
# doesn't work yet.
#
# --Micah Dowty <micah@picogui.org>
#
import xmlrpclib, socket
import gtk
import gnome.applet

class ThermCollector:
    """A therm client that uses XML-RPC to collect data from thermserver,
       and integrates with the GTK main loop. When new data is available, a list
       of client functions are notified.
       """
    def __init__(self, url="http://navi.picogui.org:4510"):
        self.server = xmlrpclib.ServerProxy(url)
        self.descriptions = self.server.getDescriptions()
        self.clients = []
        # Kickstart the endless cycle of retrieving data and waiting for the next interval
        self.timerHandler()

    def timerHandler(self):
        """Called by schedule() when we supposedly have new data available. Read it
           and schedule another timeout when the next readings should be in.
           """
        try:
            # Yay, we can get data
            self.averages = self.server.getAverages()
            self.connected = True

            times = self.server.getTimes()
            secondsLeft = times['averagePeriod'] + times['periodStart'] - times['time']
            fudgeFactor = 2
            gtk.timeout_add(int((secondsLeft + fudgeFactor) * 1000), self.timerHandler)
        except socket.error:
            # Noooo... accept failure
            self.connected = False
            # Try again soon...
            gtk.timeout_add(60 * 1000, self.timerHandler)

        # Success or not, notify clients that something happened
        for client in self.clients:
            client()
        return gtk.FALSE

    def add(self, client):
        """Add a client to be called when data is available or there is an error"""
        self.clients.append(client)
        # Give it an initial update
        client()


class ThermLabel(gtk.Label):
    """A gtk label that shows the value of a particular therm, updating automatically"""
    def __init__(self, collector, thermId):
        gtk.Label.__init__(self)
        self.thermId = thermId
        self.collector = collector
        collector.add(self.callback)

    def callback(self):
        if self.collector.connected:
            # UTF-8 for the degrees symbol
            degrees = "\302\260"
            self.set_text("%.01f%sF" % (self.collector.averages[self.thermId], degrees))
        else:
            self.set_text("No Data")


class ThermMenu(gtk.MenuItem):
    """A menu item displaying a default sensor, leading to a menu with all
       other sensors when clicked.
       """
    def __init__(self, collector, defaultTherm):
        gtk.MenuItem.__init__(self)

        label = ThermLabel(collector, defaultTherm)
        label.show()
        self.add(label)

        menu = gtk.Menu()
        therms = collector.descriptions.keys()
        therms.sort()
        for therm in therms:
            label = ThermLabel(collector, therm)
            label.show()
            item = gtk.MenuItem()
            item.add(label)
            item.show()
            menu.add(item)

        self.set_submenu(menu)

def ThermApplet_factory(applet, iid):
    collector = ThermCollector()
    applet.add(ThermMenu(collector, 'porch'))
    applet.show_all()
    return gtk.TRUE

gnome.applet.bonobo_factory("OAFIID:GNOME_ThermApplet_Factory",
                             gnome.applet.Applet.__gtype__,
                             "thermapplet", "0", ThermApplet_factory)

### The End ###

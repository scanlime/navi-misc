#!/usr/bin/env python

import gtk
import dbus.glib
from sirc import ui
from sirc.common import server
from telepathy.interfaces import *

factory = server.ServerFactory()
server = factory('irc')
print 'connecting'
server.Connect()
print 'done connecting'
win = ui.Window()
win.show_all()
print 'done initializing'
gtk.main()
print server.GetStatus()
print server.conn.get_valid_interfaces()
server.Disconnect()

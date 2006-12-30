#!/usr/bin/env python

import gtk
from sirc import ui
from sirc.common import server
from telepathy.interfaces import *

factory = server.ServerFactory()
server = factory('irc')
server.Connect()
win = ui.Window()
win.show_all()
gtk.main()
print server.GetStatus()
print server.conn.get_valid_interfaces()
server.Disconnect()

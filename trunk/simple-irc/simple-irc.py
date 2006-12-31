#!/usr/bin/env python

import gtk
import dbus.glib
from sirc import ui
#from sirc.common import server
from telepathy.interfaces import *

#factory = server.ServerFactory()
#server = factory('irc')
#server.Connect()
win = ui.Window()
win.show_all()
gtk.main()
#server.Disconnect()

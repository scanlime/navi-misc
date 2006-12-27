#!/usr/bin/env python

import gtk
from SimpleIRC import ui, connection

factory = connection.ConnectionFactory()
connection = factory()

win = ui.Window()
win.show_all()
gtk.main()

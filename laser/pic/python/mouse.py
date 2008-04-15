#!/usr/bin/env python
import pyrcpod, gtk
from laser import Projector

dev = pyrcpod.devices[0].open()
lp = Projector(dev)

def leave_notify_event(widget, event):
    lp.laser = False
    lp.enablePin.deassert()

def motion_notify_event(widget, event):
    if event.is_hint:
        x, y, state = event.window.get_pointer()
    else:
        x = event.x; y = event.y
        state = event.state

    lp.laser = True
    lp.enablePin.assert_()
    lp.x = x / 2
    lp.y = y / 2

    return gtk.TRUE

win = gtk.Window()
win.connect("destroy", gtk.mainquit)

drawing_area = gtk.DrawingArea()
drawing_area.set_size_request(512, 512)
win.add(drawing_area)
win.set_resizable(gtk.FALSE)
drawing_area.show()

drawing_area.connect("motion_notify_event", motion_notify_event)
drawing_area.connect("leave_notify_event", leave_notify_event)
drawing_area.set_events(gtk.gdk.EXPOSURE_MASK |
                        gtk.gdk.LEAVE_NOTIFY_MASK |
                        gtk.gdk.POINTER_MOTION_MASK |
                        gtk.gdk.POINTER_MOTION_HINT_MASK)

win.show()
gtk.main()

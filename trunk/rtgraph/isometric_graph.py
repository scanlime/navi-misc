#!/usr/bin/env python
#
# A simple example of the IsometricVectorGraph widget
#
# -- Micah Dowty <micah@picogui.org>
#

import gtk
import rtgraph

win = gtk.Window(gtk.WINDOW_TOPLEVEL)

graph = rtgraph.PolarVectorGraph()
graph.channels = [
    PolarFunction(math.sin, 1, (0,0,1)),
    PolarFunction(math.cos, 1.1, (1,0,0)),
    PolarFunction(lambda t: 1, 4, (0,0.8,0)),
    ]
graph.show()

frame = gtk.AspectFrame()
frame.add(graph)
frame.show()

win.add(frame)
win.set_border_width(8)
win.show()
win.connect("destroy", gtk.mainquit)
gtk.main()

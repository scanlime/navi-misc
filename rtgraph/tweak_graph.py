#!/usr/bin/env python
#
# Demonstrates using Tweak to control python attributes in
# real-time. In this case, the attributes being controlled
# are graph channels being drawn by an HScrollLineGraph.
#
# -- Micah Dowty <micah@picogui.org>
#
import gtk
import time, math
import rtgraph, Tweak

win = gtk.Window(gtk.WINDOW_TOPLEVEL)
vbox = gtk.VBox()
vbox.show()
win.add(vbox)
win.set_border_width(5)

graph = rtgraph.HScrollLineGraph(channels=[
    rtgraph.Channel(0, color=(1,0,0)),
    rtgraph.Channel(0, color=(0,0.8,0)),
    rtgraph.Channel(0, color=(0,0,1)),
    ])
graph.show()
frame = gtk.Frame()
frame.add(graph)
frame.show()
vbox.pack_start(frame)

tweaker = Tweak.List([
    Tweak.Quantity(graph.channels[0], 'value', name="Red"),
    Tweak.Quantity(graph.channels[1], 'value', name="Green"),
    Tweak.Quantity(graph.channels[2], 'value', name="Blue"),
    ])
tweaker.show()
vbox.pack_start(tweaker, gtk.FALSE)

win.show()
win.connect("destroy", gtk.mainquit)
gtk.main()

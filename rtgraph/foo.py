#!/usr/bin/env python
import gtk
import rtgraph

win = gtk.Window(gtk.WINDOW_TOPLEVEL)
vbox = gtk.VBox()
vbox.show()
win.add(vbox)
win.set_border_width(5)

for i in xrange(4):

    graph = rtgraph.HScrollGraph()
    graph.show()

    frame = gtk.Frame()
    frame.set_label("Graph #%d" % (i+1))
    frame.add(graph)
    frame.show()

    vbox.pack_end(frame)

win.show()
gtk.main()

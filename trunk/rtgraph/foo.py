#!/usr/bin/env python
import gtk
import rtgraph, time, math


class SineChannel(rtgraph.Channel):
    def getValue(self):
        return sin(time.time() % (math.pi*2)) * 0.5 + 0.5


win = gtk.Window(gtk.WINDOW_TOPLEVEL)
vbox = gtk.VBox()
vbox.show()
win.add(vbox)
win.set_border_width(5)

for i in xrange(4):

    graph = rtgraph.HScrollGraph()
    graph.channels = [SineChannel()]
    graph.show()

    frame = gtk.Frame()
    frame.set_label("Graph #%d" % (i+1))
    frame.add(graph)
    frame.show()

    vbox.pack_end(frame)

win.show()
gtk.main()

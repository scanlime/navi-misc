#!/usr/bin/env python
import gtk
import rtgraph, time, math


class SineChannel(rtgraph.Channel):
    def __init__(self, frequency, amplitude, color):
        rtgraph.Channel.__init__(self, color=color)
        self.frequency = frequency
        self.amplitude = amplitude

    def getValue(self):
        return math.sin((time.time() * self.frequency) % (math.pi*2)) * 0.5 * self.amplitude + 0.5


win = gtk.Window(gtk.WINDOW_TOPLEVEL)
vbox = gtk.VBox()
vbox.show()
win.add(vbox)
win.set_border_width(5)

for name, rate, channels in [
    ("Sub-ether phase", 80, [SineChannel(2, 0.8, (0,0,1)),
                             rtgraph.Channel(0.5, (0,0,0))]),
    ("Tachyon variance", 30, [SineChannel(1, 1, (0,0,1)),
                              SineChannel(1.01, 1, (0,0.5,0)),
                              SineChannel(10, 0.4, (1,0,0))]),
    ]:

    graph = rtgraph.HScrollLineGraph(scrollRate=rate)
    graph.channels = channels
    graph.show()

    frame = gtk.Frame()
    frame.set_label(name)
    frame.add(graph)
    frame.show()

    vbox.pack_end(frame)

win.show()
gtk.main()

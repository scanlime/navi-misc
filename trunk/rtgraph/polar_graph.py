#!/usr/bin/env python
#
# A simple example of the PolarVectorGraph widget
#
# -- Micah Dowty <micah@picogui.org>
#

import gtk
import rtgraph, time, math


class PolarFunction(rtgraph.Channel):
    def __init__(self, f, s, color):
        rtgraph.Channel.__init__(self, color=color)
        self.f = f
        self.s = s

    def getValue(self):
        theta = (self.s * time.time()) % (math.pi*2)
        return (self.f(theta) * 0.5 + 0.5,
                theta)


win = gtk.Window(gtk.WINDOW_TOPLEVEL)

graph = rtgraph.PolarVectorGraph()
graph.channels = [
    PolarFunction(math.sin, 1, (0,0,1)),
    PolarFunction(math.cos, 1.1, (1,0,0)),
    PolarFunction(lambda t: 1, 4, (0,0.8,0)),
    ]
graph.show()

win.add(graph)
win.show()
win.connect("destroy", gtk.mainquit)
gtk.main()

#!/usr/bin/env python
import rtgraph
import time, math, random
import gtk

# Reference for t=0
tInitial = time.time()

class FunctionChannel(rtgraph.Channel):
    def __init__(self, fString):
        rtgraph.Channel.__init__(self, name=fString)
        self.fString = fString

    def getValue(self):
        # Evaluate the function, with some useful modules in its
        # namespace along with our independent variable 't'.
        global tInitial
        ns = {}
        ns.update(math.__dict__)
        ns.update(random.__dict__)
        ns['t'] = time.time() - tInitial
        return eval(self.fString, ns)


channels = [
    FunctionChannel("sin(t) * 0.5 + 0.5"),
    FunctionChannel("tan(t)"),
    FunctionChannel("sin(t)"),
    FunctionChannel("random()"),
    FunctionChannel("random() * 0.2 + 0.4"),
    FunctionChannel("0.5"),
    ]

win = gtk.Window(gtk.WINDOW_TOPLEVEL)
cg = rtgraph.ChannelGraph(channels)
cg.show()
win.add(cg)
win.set_border_width(8)
win.show()
gtk.main()

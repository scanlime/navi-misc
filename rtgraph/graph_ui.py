#!/usr/bin/env python
#
# Example for the GraphUI component,
# displays a scrolling line graph, with a GraphUI
# that allows selecting from several available
# channels.
#
# -- Micah Dowty <micah@picogui.org>
#

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
    FunctionChannel("0"),
    FunctionChannel("sin(t)"),
    FunctionChannel("cos(t)"),
    FunctionChannel("sin(10*t)"),
    FunctionChannel("sin(5*t) * 0.5"),
    FunctionChannel("random() - 0.5"),
    FunctionChannel("t % 1"),
    ]

win = rtgraph.GraphUIWindow(channels, rtgraph.HScrollLineGraph(range=(-1,1)))
gtk.main()


#!/usr/bin/env python
#
# Example for the GraphUI and FunctionChannel components.
# Displays a scrolling line graph, with a GraphUI
# that allows selecting from several available
# channels.
#
# -- Micah Dowty <micah@picogui.org>
#

import rtgraph, gtk

functions = [
    "0",
    "sin(t)",
    "cos(t)",
    "sin(10*t)",
    "sin(5*t) * 0.5",
    "random() - 0.5",
    "t % 1",
    ]

varDict = {'t': rtgraph.TimeVariable()}
channels = [rtgraph.FunctionChannel(f, vars=varDict) for f in functions]
win = rtgraph.GraphUIWindow(channels, rtgraph.HScrollLineGraph(range=(-1,1)))
win.connect("destroy", gtk.mainquit)
gtk.main()


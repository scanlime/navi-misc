#!/usr/bin/env python
#
# A simple example of the PolarVectorGraph widget
#
# -- Micah Dowty <micah@picogui.org>
#

import rtgraph, gtk

functions = [
    "(1, t % (pi*2))",
    ]

varDict = {'t': rtgraph.TimeVariable()}
channels = [rtgraph.FunctionChannel(f, vars=varDict) for f in functions]
win = rtgraph.GraphUIWindow(channels, rtgraph.PolarVectorGraph())
win.connect("destroy", gtk.mainquit)
gtk.main()

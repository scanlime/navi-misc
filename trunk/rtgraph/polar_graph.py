#!/usr/bin/env python
#
# A simple example of the PolarVectorGraph widget
#
# -- Micah Dowty <micah@picogui.org>
#

import rtgraph, gtk

functions = [
    "(1, t % (pi*2))",
    "(sin(t), t+1)",
    "(cos(t), t+2)",
    "(sin(t*20)*0.3 + 0.5, t+3)",
    ]

varDict = {'t': rtgraph.TimeVariable()}
channels = [rtgraph.FunctionChannel(f, vars=varDict) for f in functions]
win = rtgraph.GraphUIWindow(channels, rtgraph.PolarVectorGraph())
win.connect("destroy", gtk.mainquit)
win.set_default_size(500,300)
win.show()
gtk.main()

#!/usr/bin/env python
#
# A simple example of the PolarVectorGraph widget
#
# -- Micah Dowty <micah@picogui.org>
#

import gtk
import rtgraph

functions = [
    "(1, t % (pi*2))",
    "(sin(t), t+1)",
    "(cos(t), t+2)",
    "(sin(t*20)*0.3 + 0.5, t+3)",
    ]

varDict = {'t': rtgraph.TimeVariable()}
channels = [rtgraph.FunctionChannel(f, vars=varDict) for f in functions]
win = rtgraph.GraphUIWindow(channels, rtgraph.PolarVectorGraph(), defaultSize=(500,300))
win.connect("destroy", gtk.mainquit)
gtk.main()

#!/usr/bin/env python
#
# A simple example of the IsometricVectorGraph widget
#
# -- Micah Dowty <micah@picogui.org>
#

import rtgraph, gtk

functions = [
    "(sin(t)*0.5+0.5, cos(t)*0.5+0.5, 0)",
    "(sin(t)*0.5+0.5, 0, cos(t)*0.5+0.5)",
    "(0, sin(t)*0.5+0.5, cos(t)*0.5+0.5)",
    ]

varDict = {'t': rtgraph.TimeVariable()}
channels = [rtgraph.FunctionChannel(f, vars=varDict) for f in functions]
win = rtgraph.GraphUIWindow(channels, rtgraph.IsometricVectorGraph(), defaultSize=(500,600))
win.connect("destroy", gtk.mainquit)
gtk.main()

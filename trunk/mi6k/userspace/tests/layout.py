#!/usr/bin/env python

# A copy of the VFD code from the LED Board v2 project, hacked up a bit
# to work with the USB VFD controller as a real-world test application.
#

import sys; sys.path[0] += '/..'
import mi6k, vfdwidgets

vfd = mi6k.Device().vfd
vfd.powerOn()
surface = vfdwidgets.Surface(20, 2)

surface.widgets.append(vfdwidgets.Text("Top", (0, -1)))
surface.widgets.append(vfdwidgets.Text("--", (-1, 1)))
surface.widgets.append(vfdwidgets.Text("--", (-1, 1)))
surface.widgets.append(vfdwidgets.Text("Boing", (1, 1)))
surface.widgets.append(vfdwidgets.Text("*", (0, 1)))

vfd.writeLines(surface.draw())

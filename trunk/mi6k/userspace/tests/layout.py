#!/usr/bin/env python

# A copy of the VFD code from the LED Board v2 project, hacked up a bit
# to work with the USB VFD controller as a real-world test application.
#

import sys; sys.path[0] += '/..'
import time
import mi6k, vfdwidgets

vfd = mi6k.Device().vfd
vfd.powerOn()
vfd.setBrightness(0.1)
surface = vfdwidgets.Surface(20, 2)

surface.widgets.append(vfdwidgets.LoopingScroller("Wibble wibble wibble wibble splat", priority=10))
surface.widgets.append(vfdwidgets.LoopingScroller("Whee", (0, 1)))
surface.widgets.append(vfdwidgets.Text("Boingski", (2, 1)))
surface.widgets.append(vfdwidgets.Text("*", (1, 1), background='~'))

osd = vfdwidgets.Text('OSD Message goes here, or something',
                      gravity=(0, 10), priority=100, ellipses='...')
surface.widgets.append(osd)

while 1:
    osd.visible = not osd.visible
    surface.layout()
    for i in xrange(50):
        surface.update()
        vfd.writeLines(surface.draw())


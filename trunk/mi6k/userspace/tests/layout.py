#!/usr/bin/env python

# A copy of the VFD code from the LED Board v2 project, hacked up a bit
# to work with the USB VFD controller as a real-world test application.
#

import sys; sys.path[0] += '/..'
import time
import mi6k
from vfdwidgets import *

vfd = mi6k.Device().vfd
vfd.powerOn()
vfd.setBrightness(0.1)
surface = Surface(20, 2)

surface.add(LoopingScroller("Wibble wibble wibble wibble splat", priority=10))
surface.add(LoopingScroller("Whee", (0, 1)))
surface.add(Clock(gravity=(2, 1)))
surface.add(Text(vfd.allocCharacter([
    [ 0, 0, 0, 0, 0 ],
    [ 0, 0, 0, 0, 0 ],
    [ 0, 1, 1, 1, 0 ],
    [ 0, 1, 1, 1, 0 ],
    [ 0, 1, 1, 1, 0 ],
    [ 0, 0, 0, 0, 0 ],
    [ 0, 0, 0, 0, 0 ],
    ]), (1, 1), background='~'))

ellipsesChar = vfd.allocCharacter([
    [ 0, 0, 0, 0, 0 ],
    [ 0, 0, 0, 0, 0 ],
    [ 0, 0, 0, 0, 0 ],
    [ 0, 0, 0, 0, 0 ],
    [ 0, 0, 0, 0, 0 ],
    [ 0, 0, 0, 0, 0 ],
    [ 1, 0, 1, 0, 1 ] ])

surface.add(Text('OSD Message goes here, or something',
                 gravity=(0, 10),
                 priority=100,
                 ellipses=ellipsesChar),
            lifetime=10)

while 1:
    surface.update()
    vfd.writeLines(surface.draw())


#!/usr/bin/env python
#
# Show the position of a joystick using the first two D/A
# channels on the rcpod-db50 as X and Y axes.
#
# Requires evdev.py, available at:
# http://navi.cx/svn/misc/trunk/python/evdev/
#
# Give the Linux event device for the joystick you want to use
# on the command line.
#

import pyrcpod
import evdev
import sys

js = evdev.Device(sys.argv[1])
rcpod = pyrcpod.devices[0].open(pyrcpod.RcpodDB50)

while 1:
    js.poll()
    rcpod.analogOutput(*[ i * 0.5 + 0.5 for i in
                          js['ABS_X'], -js['ABS_Y'] ])

#!/usr/bin/env python
#
# This is a command line tool using the vfdwidgets and mi6k
# modules to display simple formatted messages on the VFD.
#

import time
import mi6k
from vfdwidgets import *
from optparse import OptionParser

parser = OptionParser()
parser.add_option("-f", "--fade", dest="fade", type="float",
                  help="Fade from full to minimum brightness over a "
		       "specified period of time.",
		  metavar="SECONDS")
parser.add_option("-b", "--brightness", dest="brightness",
                  metavar="LEVEL", default=0.1, type="float",
                  help="The VFD brightness level, between 0 and 1.")
parser.add_option("-a", "--align", dest="align",
                  metavar="X", default=0.0, type="float",
                  help="The horizontal alignment for text. 0 is left "
		       "justified, 1 is right justified, and 0.5 is "
		       "centered. Any value between 0 and 1 can be used. "
		       "The default is left justified.")

options, args = parser.parse_args()
vfd = mi6k.Device().vfd
vfd.powerOn()
surface = Surface(20, 2)

ellipsesChar = vfd.allocCharacter([
    [ 0, 0, 0, 0, 0 ],
    [ 0, 0, 0, 0, 0 ],
    [ 0, 0, 0, 0, 0 ],
    [ 0, 0, 0, 0, 0 ],
    [ 0, 0, 0, 0, 0 ],
    [ 0, 0, 0, 0, 0 ],
    [ 1, 0, 1, 0, 1 ] ])

# Create VFD widgets for each line of text we're given, and
# auto-ellipsize using our custom character above.
for i, line in enumerate(args):
    line = line.replace("...", ellipsesChar)
    surface.add(Text(line, align=(options.align, 0.5),
                     gravity=(0, i-1000),
                     ellipses=ellipsesChar))

vfd.setBrightness(options.brightness)
surface.update()
vfd.writeLines(surface.draw())

if options.fade:
    for i in xrange(10):
        vfd.setBrightness(1.0 - i*0.1)
        vfd.flush()
        time.sleep(options.fade * 0.1)

### The End ###

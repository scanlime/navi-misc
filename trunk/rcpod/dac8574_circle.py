#!/usr/bin/env python
#
# Generates a circle using the first two D/A channels
# on the rcpod-db50 as X and Y axes.
#

import pyrcpod
import pyrcpod.i2c
import pyrcpod.i2c.ti
from math import sin, cos

rcpod = pyrcpod.devices[0].open(pyrcpod.RcpodDB50)

theta = 0
while 1:
    rcpod.analogOutput(cos(theta) * 0.5 + 0.5,
                       sin(theta) * 0.5 + 0.5)
    theta += 0.05

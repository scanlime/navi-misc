#!/usr/bin/env python
import os, time
from math import *

f = os.open("/dev/usb/rwand0", os.O_RDWR)
t = 0
while 1:
    frame = [1 << int(sin(t + i * 0.2) * 3.99 + 4) for i in xrange(60)]
    os.write(f, "".join([chr(b) for b in frame]))
    t += 0.7
    time.sleep(0.01)

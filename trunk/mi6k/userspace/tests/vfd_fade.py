#!/usr/bin/env python
import time, mi6k
from math import *

vfd = mi6k.Device().vfd

startTime = time.time()
try:
    while True:
        t = time.time() - startTime
        vfd.setBrightness(sin(t * 10) * 0.5 + 0.5)
	vfd.flush()
	time.sleep(0.01)
finally:
    vfd.setBrightness(1)

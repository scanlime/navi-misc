#!/usr/bin/env python
import time, mi6k
from math import *

m = mi6k.Device()

startTime = time.time()
try:
    while True:
        t = time.time() - startTime
        m.lights.set(sin(t * 20) * 0.4 + 0.6,
                  blue  = exp((t % 2) * -10))
        time.sleep(0.01)
finally:
    m.lights.set(0,0)

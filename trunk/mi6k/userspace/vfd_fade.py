#!/usr/bin/env python
import time
from math import *

f = open("/dev/usb/mi6k0", "w")

def setBrightness(l):
    f.write("\x19\x30\xFF" + chr(int((1-l) * 0x07)))
    f.flush()

startTime = time.time()
try:
    while True:
        t = time.time() - startTime
        setBrightness(sin(t * 10) * 0.5 + 0.5)
        time.sleep(0.01)
finally:
    setBrightness(1)

#!/usr/bin/env python
import time, struct
from math import *
from fcntl import ioctl

f = open("/dev/usb/mi6k0", "w")

def setLEDs(white, blue):
    ioctl(f, 0x3601, struct.pack("HH",
                                 min(1, max(0, white)) * 0xFFFF,
                                 min(1, max(0, blue )) * 0xFFFF))

startTime = time.time()
try:
    while True:
        t = time.time() - startTime
        setLEDs(sin(t * 20) * 0.4 + 0.6,
                blue  = exp((t % 2) * -10))
        time.sleep(0.01)
finally:
    setLEDs(0,0)

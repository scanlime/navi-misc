#!/usr/bin/env python
import time, struct
from math import *
from fcntl import ioctl

f = open("/dev/usb/mi6k0", "w")

startTime = time.time()
while True:
    t = time.time() - startTime

    blue  = exp((t % 2) * -10)
    white = sin(t * 20) * 0.4 + 0.6

    ioctl(f, 0x3601, struct.pack("HH",
                                 min(1, max(0, white)) * 0xFFFF,
                                 min(1, max(0, blue )) * 0xFFFF))
    time.sleep(0.01)

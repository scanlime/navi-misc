#!/usr/bin/env python
import time
from math import sin
from fcntl import ioctl

f = open("/dev/usb/widget", "w")

startTime = time.time()
while True:
    t = time.time() - startTime
    blue  = sin(t * 5) * 0.4 + 0.6
    white = sin(t * 40) * 0.4 + 0.6
    ioctl(f, 2, (int(blue * 0x3FF) << 16) | int(white * 0x3FF))
    time.sleep(0.01)

#!/usr/bin/env python
import time
from math import *
from fcntl import ioctl

f = open("/dev/usb/widget", "w")

startTime = time.time()
while True:
    t = time.time() - startTime

    blue  = exp((t % 2) * -10)
    white = sin(t * 20) * 0.4 + 0.6

    ioctl(f, 2,
          (int(min(1, max(0, blue)) * 0x3FF) << 16) |
          int(min(1, max(0, white)) * 0x3FF))
    time.sleep(0.01)

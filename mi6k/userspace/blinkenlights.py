#!/usr/bin/env python
import time
from math import sin
from fcntl import ioctl

f = open("/dev/usb/widget", "w")

t = 0
while True:
    l = sin(t) * 0.4 + 0.6
    ioctl(f, 2, int(l * 0x3FF) << 16)
    t += 0.05
    time.sleep(0.01)

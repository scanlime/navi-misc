#!/usr/bin/env python
import sys
from fcntl import ioctl
f = open("/dev/usb/widget", "w")
ioctl(f, 0x3600, int(sys.argv[1]))

#!/usr/bin/env python
import sys
from fcntl import ioctl
f = open("/dev/usb/widget", "w")
ioctl(f, 1, int(sys.argv[1]))

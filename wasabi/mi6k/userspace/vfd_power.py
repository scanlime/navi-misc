#!/usr/bin/env python
import sys
from fcntl import ioctl
f = open("/dev/usb/mi6k0", "w")
ioctl(f, 0x3600, int(sys.argv[1]))

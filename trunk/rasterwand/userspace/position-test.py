#!/usr/bin/env python
import os
f = os.open("/dev/usb/rwand0", os.O_RDWR)
while 1:
    os.write(f, "".join([chr(i) for i in xrange(60)]))

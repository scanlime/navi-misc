#!/usr/bin/env python
import sys, mi6k
vfd = mi6k.Device().vfd

if int(sys.argv[1]):
    vfd.powerOn()
else:
    vfd.powerOff()

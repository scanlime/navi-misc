#!/usr/bin/env python
from efs import FieldSensor
import time

efs = FieldSensor()
while 1:
    efs.resetParams()
    print "Reset"
    for i in xrange(5):
        time.sleep(0.2)
        print efs.readPacket()

    efs.initScan()
    print "scan"
    for i in xrange(5):
        time.sleep(0.2)
        print efs.readPacket()

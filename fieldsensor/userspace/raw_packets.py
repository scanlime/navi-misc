#!/usr/bin/env python
from efs import FieldSensor
import time

efs = FieldSensor()
while 1:
    print efs.readPacket()
    time.sleep(0.2)

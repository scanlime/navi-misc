#!/usr/bin/env python
import time, struct

efs = open("/dev/usb/efs0")

def read_averages(efs):
    packet = struct.unpack("i" * 9, efs.read(4*9))
    if packet[8]:
        return [packet[i] / float(packet[8]) for i in xrange(8)]

while 1:
    print read_averages(efs)
    time.sleep(0.01)

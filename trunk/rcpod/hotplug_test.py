#!/usr/bin/env python
#
# Demonstrates detecting the addition and removal of USB
# devices at runtime. This repeatedly scans the USB bus
# for changes, displaying any differences in the device
# lists observed.
#

import pyrcpod, time

oldList = pyrcpod.devices[:]
while 1:
    pyrcpod.scanForDevices()
    newList = pyrcpod.devices[:]

    for oldDevice in oldList:
        if not oldDevice in newList:
            print "Device removed: %r" % oldDevice

    for newDevice in newList:
        if not newDevice in oldList:
            print "Device added: %r" % newDevice

    oldList = newList
    time.sleep(0.25)



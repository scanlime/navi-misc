#!/usr/bin/env python
#
# Test the driver's poll() function by waiting for input changes
# using select() and acting on them by switching channels.
#

from select import select

class VideoDetector:
    def __init__(self, device="/dev/usb/uvswitch0"):
        self.dev = open(device, "r");
        self.activeList = []
        self.read()

    def read(self):
        newActiveList = map(int, self.dev.readline().strip().split(" "))
        for channel in newActiveList:
            if not channel in self.activeList:
                self.channelOn(channel)
        for channel in self.activeList:
            if not channel in newActiveList:
                self.channelOff(channel)
        self.activeList = newActiveList

    def channelOn(self, channel):
        print "Channel %d activated" % channel

    def channelOff(self, channel):
        print "Channel %d deactivated" % channel

adc  = VideoDetector()

while True:
    i, o, x = select([adc.dev], [], [], 1)
    if i:
        print i
        adc.read()
    else:
        print "poing"


#!/usr/bin/env python
#
# Tests RS-485 communication between two rcpod-485 boards
#
# -- Micah Dowty <micah@picogui.org>
#

from rcpod_test import *
import unittest
import pyrcpod
import random
import time


class Dual485Test(unittest.TestCase):
    """A TestCase subclass that opens two rcpod-485 devices"""
    def setUp(self):
        if len(pyrcpod.devices) != 2:
            raise IOError("This test requires exactly two rcpod devices");
        self.rcpods = [dev.open(pyrcpod.Rcpod485) for dev in pyrcpod.devices]

    def tearDown(self):
        # Reset and close all rcpods
        for rcpod in self.rcpods:
            rcpod.reset()
            rcpod.close()


class CommTest(Dual485Test):
    """Sends a packet back and forth between two rcpod devices,
       at the specified baud rate. Each instance of this class
       represents one packet, baud rate, and transmission direction
       to test at.
       """
    def __init__(self, data, source, destination, baud):
        self.data = data
        self.source = source
        self.destination = destination
        self.baud = baud
        Dual485Test.__init__(self)

    def runTest(self):
        for rcpod in self.rcpods:
            rcpod.serialInit(self.baud)

        source = self.rcpods[self.source]
        destination = self.rcpods[self.destination]

        destination.serialRxStart()
        source.serialTx(self.data)
        self.assertEqual(destination.serialRxProgress(), len(self.data))
        self.assertEqual(destination.serialRxProgress(), len(self.data))
        self.assertEqual(destination.serialRxRead(type(self.data)), self.data)
        self.assertEqual(destination.serialRxRead(), [])

    def __str__(self):
        return "sending from rcpod%d to rcpod%d at %d baud" % (
            self.source, self.destination, self.baud)


class CommSuite(unittest.TestSuite):
    """A test suite that automatically builds a list of
       CommTest instances to test all baud rates, device
       combinations, and test packets.
       """
    def __init__(self, tests=()):
        unittest.TestSuite.__init__(self, tests)
        self.addCommTests()

    def addCommTests(self):
        # Start out by testing single-byte packets that act as corner cases
        testPackets = [[byte] for byte in testBytes]

        # Add a few random maximum-length packets
        for i in xrange(4):
            testPackets.append([random.randint(0,255)
                                for j in xrange(pyrcpod.device.RCPOD_SCRATCHPAD_SIZE)])

        for packet in testPackets:
            for baud in (300, 9600, 57600, 115200):
                for source, destination in [(0,1), (1,0)]:
                    self.addTest(CommTest(packet, source, destination, baud))


if __name__ == '__main__':
    main(defaultTest="CommSuite")

### The End ###

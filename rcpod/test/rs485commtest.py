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


class Dual485Test(unittest.TestCase):
    """A TestCase subclass that opens two rcpod-485 devices"""
    def setUp(self):
        if len(pyrcpod.devices) != 2:
            raise Exception("This test requires exactly two rcpod devices");
        self.rcpods = [dev.open(pyrcpod.Rcpod485) for dev in pyrcpod.devices]

    def tearDown(self):
        # Reset and close all rcpods
        for rcpod in self.rcpods:
            rcpod.reset()
            rcpod.close()


class CommTest(Dual485Test):
    """Sends packets back and forth between two rcpod devices"""

    def testBytePackets(self):
        """tests communications with various one-byte packets that form corner cases"""
        for byte in testBytes:
            self.tryPacket([byte])

    def testLongPacket(self):
        """tests communications with maximum-size packets"""
        self.tryPacket([random.randint(0,255)
                        for i in xrange(self.rcpods[0].scratchpadSize)])

    def setBaud(self, baud):
        """Initialize both rcpods' serial ports, setting the baud rate"""
        for rcpod in self.rcpods:
            rcpod.serialInit(baud)

    def tryPacket(self, data):
        """Given a chunk of data to test with, send from each rcpod,
           using the other rcpod to receive and validate the data.
           Tests at multiple baud rates.
           """
        for baud in (110, 300, 1200, 2400, 4800, 9600, 19200, 57600, 115200):
            self.setBaud(baud)

            for source, destination in [
                (self.rcpods[0], self.rcpods[1]),
                (self.rcpods[1], self.rcpods[0]),
                ]:
                destination.serialRxStart(destination.scratchpadSize)
                source.serialTx(data)
                returned = destination.serialRxFinish(type(data))
                self.assertEqual(data, returned, "%s -> %s (at %d baud)" %
                                 (data, returned, baud))

if __name__ == '__main__':
    main()

### The End ###

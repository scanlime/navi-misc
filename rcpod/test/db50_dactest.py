#!/usr/bin/env python
#
# Tests output and readback for the 8 D/A channels on the rcpod-db50 board
#
# -- Micah Dowty <micah@picogui.org>
#

from rcpod_test import *
import pyrcpod
import random


class DacTest(RcpodTestCase):
    """Tests output and readback of all DAC channels"""
    devClass = pyrcpod.RcpodDB50

    def testAllChannels(self):
        """Test updating all four channels of the DAC"""
        for dac in self.rcpod.dac1, self.rcpod.dac2:
            for trial in xrange(10):
                pattern = [random.randint(0, 0xFFFF) for i in xrange(4)]
                dac.write(*pattern)
                result = [dac.read(i, int) for i in xrange(4)]
                self.assertEqual(pattern, result)

    def testSingleChannel(self):
        """Test single-channel updates"""
        for dac in self.rcpod.dac1, self.rcpod.dac2:
            for channel in xrange(4):
                for trial in xrange(10):
                    pattern = random.randint(0, 0xFFFF)
                    v = [None] * 4
                    v[channel] = pattern
                    dac.write(*v)
                    result = dac.read(channel, int)
                    self.assertEqual(pattern, result)

if __name__ == '__main__':
    main()

### The End ###

#!/usr/bin/env python
#
# Test the CX-1 critical decoder, via the rcpod-485 board
#
# -- Micah Dowty <micah@picogui.org>
#

from rcpod_test import *


class CriticalDecoderTest(RcpodTestCase):
    devClass = pyrcpod.Rcpod485

    def testPing(self):
        """verify that a ping packet is echoed correctly"""
        self.assertEqual(self.rcpod.mnetSend([1]), [1])


if __name__ == '__main__':
    main()

### The End ###

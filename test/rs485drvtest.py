#!/usr/bin/env python
#
# Tests the RS-485 line driver chip on the rcpod-485 board.
# Requires that all RS-485 jumpers on the rcpod-485 are closed.
#
# -- Micah Dowty <micah@picogui.org>
#

from rcpod_test import *


class LineDriverTest(RcpodTestCase):
    """Tests that verify the firmware and API consistency
       without actually using any of the I/O hardware.
       """

    def setUp(self):
        """In addition to the usual rcpod setup, save pin descriptors for
           receive, transmit, and transmit enable, and set the proper pin
           directions.
           """
        RcpodTestCase.setUp(self)
        # The serial data is active-low
        self.tx = self.rcpod.rc6.negate()
        self.rx = self.rcpod.rc7.negate()
        self.txe = self.rcpod.rd4
        self.tx.output().assert_()
        self.rx.input().assert_()
        self.txe.output().assert_()

        # Start out with the transmitter disabled, and tx in the idle state
        self.tx.deassert()
        self.txe.deassert()

    def testIdleState(self):
        """check that the line is in the idle state"""
        self.assertEqual(self.rx.test(), False)

    def testTransmitDisabled(self):
        """check that the receiver stays in idle state when we transmit with the driver disabled"""
        self.tx.assert_()
        self.assertEqual(self.rx.test(), False)

    def testTransmitEnabled(self):
        """check that the receiver becomes active when we transmit with the driver enabled"""
        self.txe.assert_()
        self.tx.assert_()
        self.assertEqual(self.rx.test(), True)


if __name__ == '__main__':
    main()

### The End ###

#!/usr/bin/env python
#
# Test the CX-1 critical decoder, via the rcpod-485 board
# Hardware setup is as follows:
#
#  Power:
#    - CDH1 pin  1 -> External 5v supply +
#    - CDH1 pin  2 -> External 5v supply -
#    - CDH3 pin 34 -> VSS
#
#  Comm:
#    - CDH1 pin  8 -> RS-485 +
#    - CDH1 pin  9 -> RS-485 -
#
#  Relays:
#    - CDH3 pin  1 -> RA0
#    - CDH3 pin  2 -> RA1
#    - CDH3 pin  3 -> RA2
#    - CDH3 pin 10 -> RA3
#    - CDH3 pin 20 -> RA4
#
# -- Micah Dowty <micah@picogui.org>
#

from rcpod_test import *

# CD command numbers
CMD_DIGOUT = 7

class CriticalDecoderTest(RcpodTestCase):
    devClass = pyrcpod.Rcpod485

    def setUp(self):
        RcpodTestCase.setUp(self)
        # Make PORTA and PORTE pins digital rather than the default of analog
        self.rcpod.poke('adcon1', 0xFF)

    def testPing(self):
        """verify that a ping packet is echoed correctly"""
        self.assertEqual(self.rcpod.mnetSend([1]), [1])

    def verifyOutput(self, cdPinNum, rcpodPinName):
        """Tests the connection between a given critical decoder
           pin and the given rcpod pin.
           """
        pin = self.rcpod.pins[rcpodPinName]
        for state in (0, 1, 0):
            command = [CMD_DIGOUT, cdPinNum, state]
            self.assertEqual(self.rcpod.mnetSend(command), command)
            self.assertEqual(pin.test(), state)

    def testRelays(self):
        """Test relay I/O lines"""
        self.verifyOutput(0,  'ra0') # FC relay, pin 1
        self.verifyOutput(1,  'ra1') # MNET relay, pin 2
        self.verifyOutput(2,  'ra2') # TX relay, pin 3
        self.verifyOutput(19, 'ra4') # MW relay, pin 20


if __name__ == '__main__':
    main()

### The End ###

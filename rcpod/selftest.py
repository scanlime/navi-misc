#!/usr/bin/env python
"""Usage: selftest.py [options] [test] [...]

 Uses Python's "unittest" framework to perform a self-test on the rcpod.
 This also serves as an example for performing tests on other hardware
 with the rcpod and unittest.

 Pass a test suite name on the command line to run that test suite. Command
 line options provided by unittest can be listed with the "-h" flag.
 NOTE that most tests should NOT be run with external hardware connected
 to the rcpod. Using hardware other than what is described in the test
 descriptions below will produce incorrect test results and could damage
 the rcpod and/or the external hardware.

 Useful test sets provided by this module:

   safe    : All tests that can be run without setting any of the rcpod's
             I/O ports to output mode. This won't affect any hardware plugged
             into the rcpod, but can't test much. Tests all rcpod devices
             found on the USB bus.

   single  : Tests all API and hardware features possible with a single rcpod.
             This can verify API sanity in most cases, but only partially
             verifies hardware sanity. Note that this will modify the I/O
             port states, and it should NOT be run with any external hardware
             hooked up to the rcpod. Tests all rcpod devices found on the USB
             bus.

   dual    : Requires two rcpod units, with all I/O pins (excluding power,
             USB, and oscillator) connected to each other. Also runs the equivalent
             of a 'single' test on both rcpods. This uses each rcpod to test the I/O
             features on the other.

 -- Micah Dowty <micah@picogui.org>
"""

import pyrcpod, unittest, sys


# A set of byte values used to test several things.
# This pattern includes all bits off, all bits on, the two
# 'checkerboard' bit patterns, and tests for MSB/LSB alignment.
testBytes = (0x00, 0xFF, 0xAA, 0x55, 0x01, 0x02, 0x80, 0x40)


class SimpleRcpodTestCase(unittest.TestCase):
    """A TestCase subclass that, for each available rcpod device, opens
       the device, runs the test case, and closes the device.
       """
    def __call__(self, result=None):
        for dev in pyrcpod.devices:
            self.rcpod = dev.open()
            unittest.TestCase.__call__(self, result)
            self.rcpod.close()


class safe(SimpleRcpodTestCase):
    """Tests that verify the firmware and API consistency
       without actually using any of the I/O hardware.
       """

    def testPokePeek(self):
        """Tests whether poke and peek work on 1 byte, using the scratchpad"""
        for byte in testBytes:
            self.rcpod.poke('scratchpad', byte)
            result = self.rcpod.peek('scratchpad')
            self.assertEqual(byte, result)


if __name__ == '__main__':
    # If no arguments were given, unittest usually runs the default test
    # case. We don't set up a default test case, since we want the user
    # to have read the doc string before picking a test to run. Print
    # it and exit if there are no command line arguments.
    if len(sys.argv) > 1:
        unittest.main()
    else:
        print __doc__


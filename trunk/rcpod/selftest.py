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

   safe
   ----

    * All tests that can be run without setting any of the rcpod's
      I/O ports to output mode. This won't affect any hardware plugged
      into the rcpod, but can't test much.

   single
   ------

    * Tests all API and hardware features possible with a single rcpod.
      This can verify API sanity in most cases, but only partially
      verifies hardware sanity.

    * Note that this will modify the I/O port states, and it
      MUST NOT be run with any external hardware hooked up to the rcpod.

    * Also runs all tests covered by 'safe'

   dual
   ----

    * Requires two rcpod units, with all I/O pins (excluding power,
      USB, and oscillator) connected to each other. This uses each rcpod
      to test the I/O features on the other.

    * Also runs all tests covered by 'single' (on each rcpod)


 -- Micah Dowty <micah@picogui.org>
"""

import pyrcpod, unittest, sys, random


# A set of byte values used to test several things.
# This pattern includes all bits off, all bits on, the two
# 'checkerboard' bit patterns, and tests for MSB/LSB alignment.
testBytes = (0x00, 0xFF, 0xAA, 0x55, 0x01, 0x02, 0x80, 0x40)

# A list of pin names to run I/O tests on
testPins = (
    'ra0', 'ra1', 'ra2', 'ra3', 'ra4', 'ra5',
    'rb0', 'rb1', 'rb2', 'rb3', 'rb4', 'rb5', 'rb6', 'rb7',
    'rc0', 'rc1', 'rc2',                      'rc6', 'rc7',
    'rd0', 'rd1', 'rd2', 'rd3', 'rd4', 'rd5', 'rd6', 'rd7',
    're0', 're1', 're2',
    )


class SimpleRcpodTestCase(unittest.TestCase):
    """A TestCase subclass that opens the first available
       rcpod device and closes it afterwards.
       """
    def setUp(self):
        self.rcpod = pyrcpod.devices[0].open()

    def tearDown(self):
        self.rcpod.close()


class safe(SimpleRcpodTestCase):
    """Tests that verify the firmware and API consistency
       without actually using any of the I/O hardware.
       """

    def testPokePeek(self):
        """poke and peek work on 1 byte, using the scratchpad"""
        address = 'scratchpad'
        for byte in testBytes:
            self.rcpod.poke(address, byte)
            result = self.rcpod.peek(address)
            self.assertEqual(byte, result)

    def testScratchpad(self):
        """testing all bytes of the scratchpad"""
        for address in xrange(*self.rcpod.scratchpadRange):
            for byte in (0x00, 0xFF):
                self.rcpod.poke(address, byte)
                result = self.rcpod.peek(address)
                self.assertEqual(byte, result)

    def testPokePeekBlock(self):
        """poke and peek the entire scratchpad, as a block"""
        address = 'scratchpad'
        testPattern = [random.randint(0, 255) for i in xrange(*self.rcpod.scratchpadRange)]
        self.rcpod.poke(address, testPattern)
        result = self.rcpod.peek(address, len(testPattern))
        self.assertEqual(testPattern, result)

    def testPokePeekBlockSizes(self):
        """poke and peek with block sizes that are corner cases for librcpod"""
        address = 'scratchpad'
        for size in (0, 1, 2, 4, 5, 7, 8, 9):
            testPattern = [random.randint(0, 255) for i in range(*self.rcpod.scratchpadRange)[:size]]
            self.rcpod.poke(address, testPattern)
            result = self.rcpod.peek(address, len(testPattern), retType=list)
            self.assertEqual(testPattern, result)

    def testResetValues(self):
        """verify that relevant registers have the proper power-on-reset values"""
        # Note that this can't test PORT*, since reading them reads the current pin
        # states rather than the output driver states.

        # Test the tristate registers for the first three ports.
        # The other two can't be reliably tested, since on PIC16C745 hardware they're
        # not implemented.
        self.assertEqual(self.rcpod.peek('trisa', 3), [0x3F, 0xFF, 0xC7])

        # Other miscellaneous registers that should be initialized...
        self.assertEqual(self.rcpod.peek('t1con'), 0)
        self.assertEqual(self.rcpod.peek('t2con'), 0)
        self.assertEqual(self.rcpod.peek('ccp1con'), 0)
        self.assertEqual(self.rcpod.peek('ccp2con'), 0)
        self.assertEqual(self.rcpod.peek('adcon0'), 0)
        self.assertEqual(self.rcpod.peek('adcon1'), 0)
        self.assertEqual(self.rcpod.peek('rcsta'), 0)
        self.assertEqual(self.rcpod.peek('txsta'), 2)

    def testPinDescInstances(self):
        """verify that all pins have valid Pin instances"""
        for pinName in testPins:
            pin = getattr(self.rcpod, pinName)
            self.assert_(isinstance(pin, pyrcpod.device.Pin))

    def testPinResetDirections(self):
        """verify, using pin descriptors, that all pins are initialized as inputs"""
        for pinName in testPins:
            pin = getattr(self.rcpod, pinName)
            self.assertEqual(pin.input().test(), True, "%s is not True" % pin.input())
            self.assertEqual(pin.output().test(), False, "%s is an False" % pin.output())


if __name__ == '__main__':
    # If no arguments were given, unittest usually runs the default test
    # case. We don't set up a default test case, since we want the user
    # to have read the doc string before picking a test to run. Print
    # it and exit if there are no command line arguments.
    if len(sys.argv) > 1:
        unittest.main()
    else:
        print __doc__


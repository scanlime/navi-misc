#!/usr/bin/env python
#
# A self-test for the rcpod API and hardware. Uses python's
# unittest package. This should NOT be run with any external
# hardware attached, it will corrupt the test results and could
# damage the rcpod and/or the external hardware.
#
# -- Micah Dowty <micah@picogui.org>
#

import random
from rcpod_test import *


class SelfTest(RcpodTestCase):
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
        """verify that the set of pin descriptors for this device looks sane"""
        if self.rcpod.model == "PIC16C745":
            self.assertEqual(len(self.rcpod.pins), 19)
        elif self.rcpod.model == "PIC16C765":
            self.assertEqual(len(self.rcpod.pins), 30)
        else:
            self.fail("Unknown model number")

        # Now make sure all pin instances are installed as attributes of rcpod, and
        # that all are valid pin instances with different pin descriptor values
        seenValues = []
        for name, pin in self.rcpod.pins.iteritems():
            self.assertEqual(getattr(self.rcpod, name), pin, "Pin not installed")
            self.assert_(isinstance(pin, pyrcpod.device.Pin), "Not a pin instance")
            self.assert_(pin.value not in seenValues, "Duplicate pin value")
            seenValues.append(pin.value)

    def testPinResetDirections(self):
        """verify, using pin descriptors, that all pins are initialized as inputs"""
        for pin in self.rcpod.pins.itervalues():
            self.assertEqual(pin.input().test(), True, "%s is not True" % pin.input())
            self.assertEqual(pin.output().test(), False, "%s is an False" % pin.output())

    def testPinOutputs(self):
        """verify, using pin descriptors, that all pins' values can be toggled as outputs"""
        for name, pin in self.rcpod.pins.iteritems():
            pin.output().assert_()
            pin.assert_()
            self.assertEqual(pin.test(), True, "Can't pull %s high" % pin)
            pin.deassert_()
            self.assertEqual(pin.test(), False, "Can't pull %s low" % pin)

    def testPinDescOutput(self):
        """test pin descriptor assertion by peek'ing port status"""
        for name, originalPin in self.rcpod.pins.iteritems():

            # Test with all ports initially set to 0 and to 1
            for initialValue in (0x00, 0xFF):

                # Test for both acitve-high and active-low pin descriptors
                for negated in (False, True):
                    if negated:
                        pin = originalPin.negate()
                    else:
                        pin = originalPin

                    # Make all ports output, pulled low
                    for reg in ('trisa', 'trisb', 'trisc', 'trisd', 'trise'):
                        self.rcpod.poke(reg, 0x00)
                    for reg in ('porta', 'portb', 'portc', 'portd', 'porte'):
                        self.rcpod.poke(reg, initialValue)

                    # Assert this pin descriptor
                    pin.assert_()

                    # Ensure all ports are still output
                    for reg in ('trisa', 'trisb', 'trisc', 'trisd', 'trise'):
                        self.assertEquals(self.rcpod.peek(reg), 0x00, "%s not still set to 0x00 after asserting %s" % (reg, pin))

                    # Verify all port contents
                    for reg in ('porta', 'portb', 'portc', 'portd', 'porte'):
                        expectedValue = initialValue
                        if reg[-1] == name[1]:
                            # Our pin is on this port, turn on/off its bit
                            bit = int(name[2:])
                            if negated:
                                expectedValue &= ~(1 << bit)
                            else:
                                expectedValue |= 1 << bit
                        regValue = self.rcpod.peek(reg)
                        self.assertEquals(regValue, expectedValue,
                                          "%s has the wrong value (0x%02X, not 0x%02X) after asserting %s" %
                                          (reg, regValue, expectedValue, pin))

    def testPinDescTris(self):
        """test input/output assertion by peek'ing port tristate registers"""
        for name, originalPin in self.rcpod.pins.iteritems():

            # Test with all tristates initially set to 0 and to 1
            for initialValue in (0x00, 0xFF):

                # Test for both input and output pin descriptors
                for input in (False, True):
                    if input:
                        pin = originalPin.input()
                    else:
                        pin = originalPin.output()

                    # Set tristates to the initial value
                    for reg in ('trisa', 'trisb', 'trisc', 'trisd', 'trise'):
                        self.rcpod.poke(reg, initialValue)

                    # Assert this pin descriptor
                    pin.assert_()

                    for reg in ('trisa', 'trisb', 'trisc', 'trisd', 'trise'):
                        expectedValue = initialValue
                        if reg[-1] == name[1]:
                            # Our pin is on this port, turn on/off its bit
                            bit = int(name[2:])
                            if input:
                                expectedValue |= 1 << bit
                            else:
                                expectedValue &= ~(1 << bit)
                        regValue = self.rcpod.peek(reg)
                        self.assertEquals(regValue, expectedValue,
                                          "%s has the wrong value (0x%02X, not 0x%02X) after asserting %s" %
                                          (reg, regValue, expectedValue, pin))

    def testAnalogAllSanity(self):
        """show that analogReadAll returns the right number of values and that they are all within range"""
        values = self.rcpod.analogReadAll()
        self.assertEqual(type(values), type([]))
        self.assertEqual(len(values), 8)
        for value in values:
            self.assert_(type(value), int)
            self.assert_(value >= 0 and value <= 255)

    def testAnalogChannelSanity(self):
        """show that analogReadChannel returns the right number of values and that they are all within range"""
        for channel in range(8):
            value = self.rcpod.analogReadChannel(channel)
            self.assert_(type(value), int)
            self.assert_(value >= 0 and value <= 255)

        # Make sure an exception is raised on invalid channel numbers
        for channel in (-1, 8, 256):
            self.assertRaises(ValueError, self.rcpod.analogReadChannel, channel)


if __name__ == '__main__':
    main()

### The End ###

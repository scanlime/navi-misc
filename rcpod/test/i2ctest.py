#!/usr/bin/env python
#
# A self-test for the rcpod's I2C features. This expects
# a TC74 device at address 5 and a 24XX-series I2C EEPROM
# of at least 32 bytes at address 0.
#
# -- Micah Dowty <micah@picogui.org>
#

import time, random
from rcpod_test import *
import pyrcpod.i2c


class I2CMasterTest(RcpodTestCase):
    def setUp(self):
        RcpodTestCase.setUp(self)
        self.bus = pyrcpod.i2c.Bus(self.rcpod.rb0, self.rcpod.rb1)
        self.mem = pyrcpod.i2c.Device(self.bus, 0x50 | 0)
        self.therm = pyrcpod.i2c.Device(self.bus, 0x48 | 5)
        self.bogus = pyrcpod.i2c.Device(self.bus, 0x42)

    def testWrite0(self):
        """Test RCPOD_CTRL_I2C_WRITE0"""
        self.mem.busWrite([])
        self.therm.busWrite([])
        self.assertRaises(pyrcpod.i2c.I2CError, self.bogus.busWrite, [])

    def testWrite1(self):
        """Test RCPOD_CTRL_I2C_WRITE1"""
        # Init the TC74 first and wait for it to sample
        self.therm.busWrite([0x01, 0x00])
        time.sleep(1)

        # Select the config register, make sure data is ready
        self.therm.busWrite([0x01])
        self.assertEqual(self.therm.busRead(1), [0x40])

        # Read the temperature, make sure it looks reasonable
        # (you are running this indoors, aren't you?)
        # The valid temperature range should *not* include 0 or 64,
        # since the config register could contain either of those.
        self.therm.busWrite([0x00])
        temp = self.therm.busRead(1)[0]
        if temp & 0x80:
            temp -= 256
        self.failUnless(temp > 0, "Temperature suspiciously cold")
        self.failUnless(temp < 50, "Temperature suspiciously hot")

    def testWrite234(self):
        """Test RCPOD_CTRL_I2C_WRITE2 through RCPOD_CTRL_I2C_WRITE4"""
        pattern = [random.randint(0, 255) for i in xrange(12)]

        # Write the test pattern above into the EEPROM using 3 and 4 byte writes
        self.mem.busWrite([0x00, 0x00, pattern[0]])
        time.sleep(0.1)
        self.mem.busWrite([0x00, 0x01, pattern[1]])
        time.sleep(0.1)
        self.mem.busWrite([0x00, 0x02, pattern[2]])
        time.sleep(0.1)
        self.mem.busWrite([0x00, 0x03, pattern[3]])
        time.sleep(0.1)
        self.mem.busWrite([0x00, 0x04, pattern[4],  pattern[5]])
        time.sleep(0.1)
        self.mem.busWrite([0x00, 0x06, pattern[6],  pattern[7]])
        time.sleep(0.1)
        self.mem.busWrite([0x00, 0x08, pattern[8],  pattern[9]])
        time.sleep(0.1)
        self.mem.busWrite([0x00, 0x0A, pattern[10], pattern[11]])
        time.sleep(0.1)

        # Set the read address using a 2-byte write then read back the pattern
        self.mem.busWrite([0x00, 0x00])
        self.assertEqual(pattern, self.mem.busRead(len(pattern)))

    def testRead8(self):
        """Test RCPOD_CTRL_I2C_READ8"""
        # Write an 8-byte test pattern
        pattern = [random.randint(0, 255) for i in xrange(8)]
        self.mem.busWrite([0x00, 0x00] + pattern)
        time.sleep(0.1)

        # Read it back in sizes from 1 to 8 bytes
        for len in xrange(1, 9):
            self.mem.busWrite([0x00, 0x00])
            self.assertEqual(pattern[:len], self.mem.busRead(len))

    def testWrite1Read1(self):
        """Test RCPOD_CTRL_I2C_W1READ8 with a 1-byte read"""
        # Init the TC74 first and wait for it to sample
        self.therm.busWrite([0x01, 0x00])
        time.sleep(1)

        # Point at the config register to make sure the following write works
        self.therm.busWrite([0x01])

        temp = self.therm.busWriteRead([0x00], 1)[0]
        if temp & 0x80:
            temp -= 256
        self.failUnless(temp > 0, "Temperature suspiciously cold")
        self.failUnless(temp < 50, "Temperature suspiciously hot")


if __name__ == '__main__':
    main()

### The End ###

#!/usr/bin/env python
#
# Simple I2C demo. Expects a TC74 temperature sensor to
# be connected with clock on RB0, and data+pullup on RB1.
# The TC74 address should be given on the command line.
#

import pyrcpod, sys
rcpod = pyrcpod.devices[0].open()

addr = int(sys.argv[1])
io = pyrcpod.I2CDevice(rcpod.rb0, rcpod.rb1, 0x48 | addr)

# Out of shutdown
io.write([0x01, 0x00])

while 1:
    # Point at the temperature register and read 1 byte
    b = io.writeRead([0x00], 1)[0]

    # Sign-extend the 8 bit value
    if b & 0x80:
        b -= 256

    print "%d C" % b


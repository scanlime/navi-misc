#!/usr/bin/env python
#
# Simple I2C demo. Expects a MAX7300 to be connected with
# clock on RB0, and data+pullup on RB1.
#

import pyrcpod
rcpod = pyrcpod.devices[0].open()
io = pyrcpod.I2CDevice(rcpod.rb0, rcpod.rb1, 0x42)

# Out of shutdown
io.write([0x04, 0x01])

# Start out with the first 8 outputs high
io.write([0x4C, 0xFF])

# Set those 8 ports as outputs
io.write([0x0B, 0x55])
io.write([0x0C, 0x55])

pattern = 0xFE
while 1:
    pattern = ((pattern >> 7) | (pattern << 1)) & 0xFF
    io.write([0x4C, pattern])



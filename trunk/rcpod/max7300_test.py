#!/usr/bin/env python
#
# Simple I2C demo. Expects a MAX7300 to be connected with
# clock on RB0, and data+pullup on RB1.
#

import time
import pyrcpod.i2c.maxim
rcpod = pyrcpod.devices[0].open()
bus = pyrcpod.i2c.Bus(rcpod.rb7, rcpod.rb6)
io = pyrcpod.i2c.maxim.MAX7300(bus, 0)

io.resume()

# Start out with the first 8 outputs high
io.busWrite([0x4C, 0xFF])

# Set those 8 ports as outputs
io.busWrite([0x0B, 0x55])
io.busWrite([0x0C, 0x55])
io.busWrite([0x0D, 0x55])
io.busWrite([0x0E, 0x55])

pattern = range(12, 22)

while 1:
    for port in pattern:
        io.pinLow(port)
        time.sleep(0.05)
    for port in pattern:
        io.pinHigh(port)
        time.sleep(0.05)

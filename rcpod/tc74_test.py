#!/usr/bin/env python
#
# Simple I2C demo. Expects a TC74 temperature sensor to
# be connected with clock on RB0, and data+pullup on RB1.
# The TC74 address should be given on the command line.
# This also acts as a simple rcpod I2C benchmark, since
# it displays the number of reads per second it can make.
#

import sys, time
import pyrcpod
import pyrcpod.i2c
import pyrcpod.i2c.microchip

rcpod = pyrcpod.devices[0].open()
bus = pyrcpod.i2c.Bus(rcpod.rb0, rcpod.rb1)
therm = pyrcpod.i2c.microchip.TC74(bus, int(sys.argv[1]))

startTime = time.time()
numReads = 0
lastDisplay = startTime

while 1:
    # Point at the temperature register and read 1 byte
    temp = therm.read()
    numReads += 1

    # Limit display rate to 5 times per second
    now = time.time()
    if now > lastDisplay + 1.0/5:
        lastDisplay = now
        rate = numReads / (now - startTime)
        print "%d C\t%.2f reads/sec" % (temp, rate)


#!/usr/bin/env python
#
# Simple I2C EEPROM. Expects a 24XX-series EEPROM of at least
# a few kilobytes to be connected with clock on RB0, and
# data+pullup on RB1, at atddress zero. This writes our README
# to the EEPROM, then reads it back.
#

import pyrcpod
import pyrcpod.i2c
import pyrcpod.i2c.generic

rcpod = pyrcpod.devices[0].open()
bus = pyrcpod.i2c.Bus(rcpod.rb0, rcpod.rb1)
mem = pyrcpod.i2c.generic.Memory(bus, 0)

def progress(p):
    print "%.02f%%" % (p * 100)

readme = open("README").read()

print "Writing..."
mem.memWrite(0, readme, progressCallback=progress)
print "Reading..."
print mem.memRead(0, len(readme), progressCallback=progress)

#!/usr/bin/env python
import pyrcpod, i2c

dev = pyrcpod.devices[0].open()
i2cb = i2c.i2c_bus(dev)
therm = i2c.therm(i2cb)

print therm.read(0x48, 0x00)	# address 1001 000 (A0), RTR

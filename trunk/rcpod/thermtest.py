#!/usr/bin/env python
import pyrcpod, i2c

dev = pyrcpod.devices[0].open()
i2cb = i2c.i2c_bus(dev)
therm = i2c.therm(i2cb)

while 1:
    config = therm.read(0x48, 0x01)[0]
    if (config >> 6) & 0x1:
        temp = therm.read(0x48, 0x00)[0]
        if (temp >> 7) & 0x1:
            temp = temp | 0xffffff00
	temp = temp * 9.0 / 5 + 32
        print temp

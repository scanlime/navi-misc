#!/usr/bin/env python
import pyrcpod, i2c

dev = pyrcpod.devices[0].open()
mbus = i2c.MultiBus(dev)
sampler = i2c.ThermSampler(mbus)

therms = [
    i2c.Therm(0, 0, "Living room"),
    i2c.Therm(1, 0, "Outside (living room window)"),
    ]    

while 1:
    sampler.update(therms)
    print therms

#    print therm.readTemp(0x48)
#	temp = temp * 9.0 / 5 + 32
#        print temp

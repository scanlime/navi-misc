#!/usr/bin/env python
import pyrcpod, i2c

dev = pyrcpod.devices[0].open()
mbus = i2c.MultiBus(dev)
sampler = i2c.ThermSampler(mbus)
therms = i2c.getTherms()

while 1:
    sampler.update(therms)
    print therms

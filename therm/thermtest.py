#!/usr/bin/env python
import i2c

sampler = i2c.ThermSampler()
therms = i2c.getTherms()

while 1:
    sampler.update(therms)
    print therms

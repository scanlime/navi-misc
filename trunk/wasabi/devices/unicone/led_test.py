#!/usr/bin/env python
import pyunicone, time

dev = pyunicone.UniconeDevice()
dev.configure("fpga/test/test.bit")

while 1:
    dev.i2cWrite(0x40, "boing")
    dev.setLed(1, 0.015)
#    time.sleep(0.5)

#!/usr/bin/env python
import PyUnicone.Device
import time

dev = PyUnicone.Device.UniconeDevice()
dev.configure("fpga/test/test.bit")

while 1:
    dev.setLed(1, 0.015)
    time.sleep(0.5)

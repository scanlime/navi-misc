#!/usr/bin/env python
import PyUnicone, time

dev = PyUnicone.UniconeDevice()
dev.configure("fpga/test/test.bit")

while 1:
    dev.setLed(1, 0.015)
    time.sleep(0.5)

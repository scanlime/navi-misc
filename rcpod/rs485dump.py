#!/usr/bin/env python
#
# Hex dump all data received over RS-485.
# Designed for the rcpod-485 board.
#
# -- Micah Dowty <micah@picogui.org>
#
import pyrcpod, time

dev = pyrcpod.devices[0].open(pyrcpod.Rcpod485)
dev.serialInit(9600)

dev.serialRxStart()

while True:
    packet = dev.serialRxRead()
    print " * " + " ".join(["%02X" % byte for byte in packet])
    time.sleep(1)

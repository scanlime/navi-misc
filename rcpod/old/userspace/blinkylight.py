#!/usr/bin/env python
#
# simple rcpod test program, toggles port C on and off
#

import rcpod, time
pic = rcpod.Device()

# Set up PORTC as output
pic.TRISC = 0

# Flash all of PORTC
while 1:
    pic.PORTC = 0x00
    time.sleep(0.05)
    pic.PORTC = 0xFF
    time.sleep(0.05)

#!/usr/bin/env python
#
# Initializes the first PWM module and fades an LED on and off
#

import rcpod, time
from math import sin
pic = rcpod.Device()

# Set up PORTC as output
pic.TRISC = 0

# Maximum PWM period for both CCP modules
pic.PR2 = 0xFF

# Put the CCP1 module in PWM mode
pic.CCP1CON = 0x0C

# Timer 2 on, prescaler of 16
pic.T2CON = 0x06

startTime = time.time()
while True:
    time.sleep(0.01)
    t = time.time() - startTime

    # Calculate a brightness value in the range [0,1]
    luma = sin(t * 4.5) * 0.5 + 0.5

    # The brightness control is 10-bit, stored in CCP1CON[5:4] and CCPR1L
    pwm = int(luma * 1023)
    pic.CCP1CON = 0x0C | ((pwm & 0x03) << 4)
    pic.CCPR1L = pwm >> 2

#!/usr/bin/env python
import pyrcpod, time, random
dev = pyrcpod.devices[0].open()

# LEDs are all active-low outputs on PORTB. Manipulate them
# all at the same time by poking values to the PORTB register.
dev.poke("PORTB", 0xFF)
dev.poke("TRISB", 0x00)

# Coil output is on RC0, active high
coil = dev.rc0
coil.output().assert_()

# Interruption sensor is on RC1, active high
isensor = dev.rc1

def coilStart(dutyCycle, period):
    h = dutyCycle * period
    l = period - h
    while not isensor.test():
        coil.assert_()
        time.sleep(h)
        coil.deassert()
        time.sleep(l)

def coilPush():
    while 1:
        while not isensor.test():
            pass
        while isensor.test():
            pass
        while not isensor.test():
            pass
        time.sleep(0.010)
        coil.assert_()
        dev.poke("PORTB", 0xFE)
        print "tick"
        time.sleep(0.010)
        dev.poke("PORTB", 0xFF)
        coil.deassert()

def ledBoing():
    l = [~0x01, ~0x02, ~0x04, ~0x08, ~0x10, ~0x20, ~0x40, ~0x80]
    m = l[1:]
    m.reverse()
    l = l + m
    for i in xrange(1000):
        for led in l:
            dev.poke("PORTB", led)

try:
    coilStart(0.5, 0.122)
    #coilPush()
    ledBoing()
finally:
    dev.reset()

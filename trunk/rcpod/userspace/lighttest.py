#!/usr/bin/env python
#
# This shows a very odd quality of windowed PICs with A/D converters..
# Tie pin 2 to +5v and leave all other GPIOs floating. Then shine a light
# into the chip's window and watch the meter respond :)
#

import rcpod

pic = rcpod.Device()
sampler = rcpod.AnalogSampler(pic)

def sampleLight():
    # Sample channel 0 once to charge the ADC's hold capacitor
    sampler.readChannel(0)

    # Read channel 7 (not connected to a pin, to minimize electrical interference)
    # a few times, letting the capacitor discharge. Save the last reading.
    for i in xrange(8):
        v = sampler.readChannel(7)

    # Invert the value, since more light causes the capacitor to discharge slower
    return [1-v]


rcpod.ChannelGraph(resolution=(64,320)).run(sampleLight)


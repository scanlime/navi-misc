#!/usr/bin/env python
#
# Test the pic's A/D converters by sampling all channels repeatedly.
# Draws a bargraph of all ADC channels using pygame
#

import rcpod

pic = rcpod.Device()
sampler = rcpod.AnalogSampler(pic)
rcpod.ChannelGraph().run(sampler.readAll)

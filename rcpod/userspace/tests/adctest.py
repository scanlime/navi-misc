#!/usr/bin/env python
#
# Test the pic's A/D converters by sampling all channels repeatedly
#

import rcpod, time
pic = rcpod.Device()

# Initialize the ADC speed and turn it on
pic.ADCON0 = 0x81

# Repeatedly scan all channels...
while True:
    results = [0] * 8
    for channel in xrange(8):

        # Select a channel and start the conversion
        pic.ADCON0 = 0x85 | (channel << 3)

        # Wait for the conversion to finish
        while pic.ADCON0 & 0x04:
            pass

        # Store result
        results[channel] = pic.ADRES

    # Show results for all channels
    print results

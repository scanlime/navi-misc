#!/usr/bin/env python
#
# Test the pic's A/D converters by sampling all channels repeatedly
#

import rcpod, time
pic = rcpod.Device()

# Reset all I/O ports to inputs
pic.TRISA = 0xFF
pic.TRISB = 0xFF
pic.TRISC = 0xFF
pic.TRISD = 0xFF
pic.TRISE = 0xFF

# Initialize the ADC speed and turn it on
pic.ADCON0 = 0x81

# All analog channels are really analog rather than digital
pic.ADCON1 = 0

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

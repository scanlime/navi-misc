#!/usr/bin/env python
#
# Change the power mode. Pass, as the sole argument, either:
#    on     - Forces the device to be on
#    off    - Forces it off
#    switch - Uses the power switch
#    auto   - On when the device is open
#

import rasterwand, sys

p = rasterwand.Device().params
p.power_mode = rasterwand.powerModes[sys.argv[1]]

### The End ###

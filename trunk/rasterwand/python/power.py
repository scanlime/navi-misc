#!/usr/bin/env python
#
# Change the power mode. Pass either "off", "auto", or "on"
# on the command line. "auto" uses the device's power switch.
#

import rasterwand, sys

p = rasterwand.Device().params
p.power_mode = rasterwand.powerModes[sys.argv[1]]

### The End ###

#!/usr/bin/env python
#
# GUIfied LED test proggie
#
import mi6k, time
from BZEngine.UI import Tweak
m = mi6k.Device()

Tweak.Window( Tweak.Quantity(m.lights, 'blue'),
              Tweak.Quantity(m.lights, 'white') )

Tweak.run()

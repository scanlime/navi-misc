#!/usr/bin/env python
#
# GUIfied LED test proggie
#
import mi6k, Tweak, time
m = mi6k.MI6K()

Tweak.Window( Tweak.Quantity(m.lights, 'blue'),
              Tweak.Quantity(m.lights, 'white') )

class test:
    def run(self):
        while True:
            time.sleep(1)
Tweak.run(test())

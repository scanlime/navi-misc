#!/usr/bin/env python

import rcpod, time
from BZEngine.UI import Tweak
pic = rcpod.Device()

pic.TRISC = 0
pic.PR2 = 0xFF
pic.CCPR1L = 0
pic.CCP1CON = 0x0C
pic.T2CON = 0x05

Tweak.Window( Tweak.Quantity(pic, 'CCPR1L', range=(0,255)),
              Tweak.Quantity(pic, 'PR2', range=(0,255)) )

class test:
    def run(self):
        while True:
            time.sleep(1)
Tweak.run(test())

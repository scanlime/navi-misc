#!/usr/bin/env python
import Tweak
from math import *
from laser import Projector
import threading, time


class Circle:
    def __init__(self):
        self.xmag = 30
        self.ymag = 8
        self.xbias = 128
        self.ybias = 172
        self.rate = 0.1
        self.delay = 0.01
        self.phase = 0

    def run(self):
        import pyrcpod
        dev = pyrcpod.devices[0].open()
        lp = Projector(dev)
        try:
            lp.laser = True
            l = lp.laserPin.negate()
            t = 0
            while 1:
                l.assert_()
                l = l.negate()
                lp.x = sin(t) * self.xmag + self.xbias
                lp.y = cos(t+self.phase) * self.ymag + self.ybias
                t = (t + self.rate) % (pi*2)
                time.sleep(self.delay)
        finally:
            lp.laser = False
            lp.enablePin.deassert()


c = Circle()

Tweak.Window(
    Tweak.Quantity(c, 'xmag', range=(0,255)),
    Tweak.Quantity(c, 'ymag', range=(0,255)),
    Tweak.Quantity(c, 'xbias', range=(0,255)),
    Tweak.Quantity(c, 'ybias', range=(0,255)),
    Tweak.Quantity(c, 'delay', range=(0,0.1)),
    Tweak.Quantity(c, 'rate', range=(0,2)),
    Tweak.Quantity(c, 'phase', range=(-3,3)),
    )

Tweak.run(c)

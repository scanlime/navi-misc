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

    def run(self):
        import pyrcpod
        dev = pyrcpod.devices[0].open()
        lp = Projector(dev)
        try:
            lp.laser = True
            t = 0
            while 1:
                lp.x = sin(t) * self.xmag + self.xbias
                lp.y = cos(t) * self.ymag + self.ybias
                t = (t + self.rate) % (pi*2)
                time.sleep(self.delay)
        finally:
            lp.laser = False


c = Circle()

Tweak.Window(
    Tweak.Quantity(c, 'xmag', range=(0,255)),
    Tweak.Quantity(c, 'ymag', range=(0,255)),
    Tweak.Quantity(c, 'xbias', range=(0,255)),
    Tweak.Quantity(c, 'ybias', range=(0,255)),
    Tweak.Quantity(c, 'delay', range=(0,0.1)),
    Tweak.Quantity(c, 'rate', range=(0,2)),
    )

Tweak.run(c)

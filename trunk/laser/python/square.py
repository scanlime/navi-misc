#!/usr/bin/env python
import Tweak
from math import *
from laser import Projector
import threading, time


class Square:
    def __init__(self):
        self.xmag = 10
        self.ymag = 5
        self.xbias = 176
        self.ybias = 157
        self.rate = 1
        self.delay = 0

    def line(self, lp, x1, y1, x2, y2):
        if x2 > x1:
            dx = 1
        else:
            dx = -1
        if y2 > y1:
            dy = 1
        else:
            dy = -1

        while dx or dy:
            lp.x = x1
            lp.y = y1
            x1 += dx * self.rate
            y1 += dy * self.rate
            if dx > 0 and x1 > x2 or dx < 0 and x1 < x2:
                dx = 0
                x1 = x2
            if dy > 0 and y1 > y2 or dy < 0 and y1 < y2:
                dy = 0
                y1 = y2
            time.sleep(self.delay)
        lp.laser = False
        lp.laser = True

    def run(self):
        import pyrcpod
        dev = pyrcpod.devices[0].open()
        lp = Projector(dev)
        try:
            lp.laser = True
            t = 0
            while 1:
                self.line(lp,
                          self.xbias - self.xmag, self.ybias - self.ymag,
                          self.xbias + self.xmag, self.ybias - self.ymag)
                self.line(lp,
                          self.xbias + self.xmag, self.ybias - self.ymag,
                          self.xbias + self.xmag, self.ybias + self.ymag)
                self.line(lp,
                          self.xbias + self.xmag, self.ybias + self.ymag,
                          self.xbias - self.xmag, self.ybias + self.ymag)
                self.line(lp,
                          self.xbias - self.xmag, self.ybias + self.ymag,
                          self.xbias - self.xmag, self.ybias - self.ymag)
        finally:
            lp.laser = False
            lp.enablePin.deassert()


c = Square()

Tweak.Window(
    Tweak.Quantity(c, 'xmag', range=(0,255)),
    Tweak.Quantity(c, 'ymag', range=(0,255)),
    Tweak.Quantity(c, 'xbias', range=(0,255)),
    Tweak.Quantity(c, 'ybias', range=(0,255)),
    Tweak.Quantity(c, 'delay', range=(0,0.1)),
    Tweak.Quantity(c, 'rate', range=(0,10)),
    )

Tweak.run(c)

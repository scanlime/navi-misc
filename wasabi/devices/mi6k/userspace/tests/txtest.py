#!/usr/bin/env python
#
# Tiny IR transmitter test
#
import time, struct
from BZEngine.UI import Tweak

class test:
    def __init__(self):
        self.f = open("/dev/lirc", "w")
        self.pulse = 100
        self.space = 100

    def run(self):
        while True:
            self.transmit(self.pulse, self.space)

    def transmit(self, *a):
        for i in xrange(16):
            for v in a:
                self.f.write(struct.pack("i", int(v)))
        self.f.flush()

t = test()
Tweak.Window( Tweak.Quantity(t, 'pulse', (0,10000)),
              Tweak.Quantity(t, 'space', (0,10000)) )
Tweak.run(t)

#!/usr/bin/env python
#
# Tiny IR transmitter test
#
import time, Tweak

class test:
    def __init__(self):
        self.f = open("/dev/lirc", "w")
        self.pulse = 100
        self.space = 100

    def run(self):
        while True:
            #self.transmit(10000,5000, 500,500, 500,500, 100,500)
            self.transmit(self.pulse, self.space)

    def transmit(self, *a):
        for i in xrange(16):
            for v in a:
                self.f.write(chr(int(v) & 0xFF) + chr(int(v) >> 8) + chr(0) + chr(0))
        self.f.flush()
        time.sleep(0.1)

t = test()
Tweak.Window( Tweak.Quantity(t, 'pulse', (0,10000)),
              Tweak.Quantity(t, 'space', (0,10000)) )
Tweak.run(t)

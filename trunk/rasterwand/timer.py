#!/usr/bin/env python
import pyrcpod, time
dev = pyrcpod.devices[0].open()

sensor = dev.rc1

l = []

for i in xrange(1000):
    l.append((time.time(), sensor.test()))

prevState = None
lt = l[0][0]
for t, state in l:
    if state != prevState:
        prevState = state
        print t-lt, state
        lt = t

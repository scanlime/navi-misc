#!/usr/bin/env python
#
# Really simple test, so far only for the C wrappers in rcpod.lib

import rcpod, time

dev = rcpod.lib.rcpod_InitSimple()

start = time.time()
nsamples = 0
try:
    while True:
        # Perform an analog reading, using the array helpers
        results = rcpod.lib.uchar_array(8)
        rcpod.lib.rcpod_AnalogReadAll(dev, results)
        l = [rcpod.lib.uchar_array_get(results, i) for i in range(8)]
        nsamples += 1

        print "*" * l[0]

finally:
    end = time.time()
    print "%f samples/second" % (nsamples / (end-start))

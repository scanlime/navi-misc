#!/usr/bin/env python
#
# Really simple test, so far only for the C wrappers in rcpod.lib

import rcpod

dev = rcpod.lib.rcpod_InitSimple()

while True:
    # Perform an analog reading, using the array helpers
    results = rcpod.lib.uchar_array(8)
    rcpod.lib.rcpod_AnalogReadAll(dev, results)
    l = [rcpod.lib.uchar_array_get(results, i) for i in range(8)]

    print "*" * l[0]

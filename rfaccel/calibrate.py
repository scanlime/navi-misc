#!/usr/bin/env python
import sys, time
import rfaccel
from Numeric import *
import optimize, psyco
psyco.full()

class DataCollector(rfaccel.PacketReceiver):
    """Collects calibration data periodically, storing (x,y,z) tuples"""
    def __init__(self, device, interval=0.01):
        self.vectors = []
        self.lastTimestamp = 0
        self.interval = interval
        rfaccel.PacketReceiver.__init__(self, device)

    def receivedRawAccel(self, vect):
        now = time.time()
        if now < self.lastTimestamp + self.interval:
            return
        self.lastTimestamp = now
        self.vectors.append((vect['x'], vect['y'], vect['z']))
        print "%d samples. Latest: %r" % (
            len(self.vectors), self.vectors[-1])

def calibrate(vectors, gravity=9.81):
    vectors = asarray(vectors)
    return (calibrateAxis(vectors[:,0], gravity) +
            calibrateAxis(vectors[:,1], gravity) +
            calibrateAxis(vectors[:,2], gravity))

def calibrateAxis(samples, gravity, percentile=5.0):
    samples = list(samples)
    samples.sort()

    # Discard upper and lower extremes
    p = int(percentile / 100.0 * len(samples))
    minimum = samples[p]
    maximum = samples[-(1+p)]

    # Solve for:
    #  maximum * gain + bias =  gravity
    #  minimum * gain + bias = -gravity

    gain = 2.0 * gravity / (maximum - minimum)
    bias = -0.5 * (maximum + minimum) * gain
    return (gain, bias)


if __name__ == "__main__":
    collector = DataCollector(sys.argv[1])
    try:
        while 1:
            collector.waitForData()
    except KeyboardInterrupt:
        pass
    print calibrate(collector.vectors)
    

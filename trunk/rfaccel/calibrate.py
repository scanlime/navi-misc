#!/usr/bin/env python
import sys, time
import rfaccel
from Numeric import *
import optimize, psyco
psyco.full()

class DataCollector(rfaccel.PacketReceiver):
    """Collects calibration data periodically, storing (x,y,z) tuples"""
    def __init__(self, device, interval=0.05):
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


def errorMetric(params, vectors, gravity=9.81):
    #
    # Calibration:
    #  (x * Xgain + Xbias)^2 + (y * Ygain + Ybias)^2 + (z * Zgain + Zbias)^2 = g^2
    #
    e = sum((((vectors[:,0] * params[0] + params[1])**2 +
              (vectors[:,1] * params[2] + params[3])**2 + 
              (vectors[:,2] * params[4] + params[5])**2) -
             gravity**2)**2)
    print params, e
    return e

def calibrate(vectors):
    x0 = (1.0, 0.0,
          1.0, 0.0,
          1.0, 0.0)
    return optimize.fmin(errorMetric, x0, args=(asarray(vectors),),
                         maxiter = 1000000,
                         maxfun = 100000,
                         xtol=1e-14,
                         ftol=1e-14,
                         )

if __name__ == "__main__":
    collector = DataCollector(sys.argv[1])
    try:
        while 1:
            collector.waitForData()
    except KeyboardInterrupt:
        pass
    print calibrate(collector.vectors)
    

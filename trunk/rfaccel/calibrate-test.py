#!/usr/bin/env python
import sys, time, math
import rfaccel

calibration = [ 0.0001814,
                0.00490136,
                -0.00018937,
                0.00246719,
                -0.00015337,
                0.00315704]

class Foo(rfaccel.PacketReceiver):
    def __init__(self, device):
        rfaccel.PacketReceiver.__init__(self, device)

    def receivedRawAccel(self, vect):
        accel = ( vect['x'] * calibration[0] + calibration[1],
                  vect['y'] * calibration[2] + calibration[3],
                  vect['z'] * calibration[4] + calibration[5] )
        mag = math.sqrt(accel[0]**2 + accel[1]**2 + accel[2]**2)
        print mag, accel

if __name__ == "__main__":
    x = Foo(sys.argv[1])
    try:
        while 1:
            x.waitForData()
    except KeyboardInterrupt:
        pass

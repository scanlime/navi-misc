#!/usr/bin/env python

class Projector(object):
    def __init__(self, rcpod):
        self.rcpod = rcpod
        self.xPin = rcpod.rc1
        self.yPin = rcpod.rc2
        self.enablePin = rcpod.rb7
        self.laserPin = rcpod.rb6

        self.xPin.output().assert_()
        self.yPin.output().assert_()
        self.enablePin.output().assert_()
        self.laserPin.output().assert_()

        # Set up PWM, run it as slow as we can
        # without causing an audible high pitch tone
        rcpod.poke("CCP1CON", 0x0C)
        rcpod.poke("CCP2CON", 0x0C)
        rcpod.poke("PR2", 0xFF)
        rcpod.poke("T2CON", 0x7C)

        # Start out centered and enable the coil drivers
        self.x = 0x80
        self.y = 0x80
        self.enablePin.assert_()

    def setX(self, value):
        self.rcpod.poke("CCPR2L", value)
        self._x = value

    def getX(self):
        return self._x

    x = property(getX, setX)

    def setY(self, value):
        self.rcpod.poke("CCPR1L", value)
        self._y = value

    def getY(self):
        return self._y

    y = property(getY, setY)

    def setLaser(self, value):
        if value:
            self.laserPin.assert_()
        else:
            self.laserPin.deassert()

    def getLaser(self):
        return self.laserPin.test()

    laser = property(getLaser, setLaser)


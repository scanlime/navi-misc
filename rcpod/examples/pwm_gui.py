#!/usr/bin/env python
#
# A graphical interface to the RCPOD's PWM controllers
#
import Tweak, pyrcpod

class PWM(object):
    def __init__(self, rcpod, reset=True):
        self.rcpod = rcpod
        if reset:
            self.reset()

    def reset(self):
        self.dutyCycle1 = 0
        self.dutyCycle2 = 0
        self.prescaler = 0
        self.period = 255

        self.rcpod.poke('ccp1con', 0x0F)
        self.rcpod.poke('ccp2con', 0x0F)

        self.rcpod.rc1.output().assert_()
        self.rcpod.rc2.output().assert_()

    def setPrescaler(self, value):
        value = int(value + 0.5)
        if value > 3:
            value = 3
        if value < 0:
            value = 0
        self.rcpod.poke('t2con', 0x04 | value)

    def getPrescaler(self):
        return self.rcpod.peek('t2con') & 0x03

    def setPeriod(self, value):
        self.rcpod.poke('pr2', value)

    def getPeriod(self):
        return self.rcpod.peek('pr2')

    def setDutyCycle1(self, value):
        self.rcpod.poke('ccpr1l', int(value * 255.0 + 0.5))

    def getDutyCycle1(self):
        return self.rcpod.peek('ccpr1l') / 255.0

    def setDutyCycle2(self, value):
        self.rcpod.poke('ccpr2l', int(value * 255.0 + 0.5))

    def getDutyCycle2(self):
        return self.rcpod.peek('ccpr2l') / 255.0

    prescaler = property(getPrescaler, setPrescaler)
    period = property(getPeriod, setPeriod)
    dutyCycle1 = property(getDutyCycle1, setDutyCycle1)
    dutyCycle2 = property(getDutyCycle2, setDutyCycle2)


if __name__ == "__main__":
    dev = pyrcpod.devices[0].open()
    pwm = PWM(dev)

    pwm.prescaler = 0

    Tweak.Window(
        Tweak.Quantity(pwm, 'prescaler', (0, 3), stepSize=1, pageSize=1),
        Tweak.Quantity(pwm, 'period', (0, 255), stepSize=1, pageSize=16),
        Tweak.Quantity(pwm, 'dutyCycle1'),
        Tweak.Quantity(pwm, 'dutyCycle2'),
        )
    Tweak.run()

### The End ###

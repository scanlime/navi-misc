#!/usr/bin/env python
import gtk, rtgraph, time, math, random

class TimeStepper:
    def __init__(self):
        self._then = None

    def step(self):
        now = time.time()
        if self._then:
            dt = now - self._then
        else:
            dt = 0
        self._then = now
        return dt


class Constant(rtgraph.Channel):
    """This channel always returns a constant value"""
    def __init__(self, value):
        rtgraph.Channel.__init__(self, name=str(value))
        self.value = value


class SineWave(rtgraph.Channel):
    def __init__(self, name=None, frequency=1):
        rtgraph.Channel.__init__(self, name=name)
        self.frequency = frequency
        self.theta = random.uniform(0, math.pi*2)
        self.timer = TimeStepper()

    def getValue(self):
        self.theta += 2 * math.pi * self.frequency * self.timer.step()
        return math.sin(self.theta)


class WindowComparator(rtgraph.Channel):
    """Outputs one of two values depending on whether
       the input lies between two given values or not.
       """
    def __init__(self, name, input, window, falseOutput=0, trueOutput=0.5):
        rtgraph.Channel.__init__(self, name=name)
        self.window = window
        self.input = input
        self.falseOutput = falseOutput
        self.trueOutput = trueOutput

    def getValue(self):
        v = self.input.getValue()
        if v >= self.window[0] and v <= self.window[1]:
            return self.trueOutput
        else:
            return self.falseOutput


class PulseTracker(rtgraph.Channel):
    """Algorithm for recovering a sinusoidal signal given a pulse
       that's high when the signal is within some small off-center window.
       """
    def __init__(self, name, pulse, oscillator):
        rtgraph.Channel.__init__(self, name=name)
        self.pulse = pulse
        self.oscillator = oscillator

        # 'c' is the sine value at the leading edge of the window when the
        # sine's derivative is positive. This is the same as the smaller of the
        # two window boundaries.
        self.c = -0.1

        # Pulse edge-detector and period measurement
        self.pulseTimer = TimeStepper()
        self.lastStep = 0
        self.lastPulseVal = None

        # The sign of the derivative of the true angle,
        # inferred from the asymmetry of 'c'.
        self.direction = 1

        # Another timer to sample the period of the original oscillation
        self.periodTimer = TimeStepper()

    def getValue(self):
        self.oscVal = self.oscillator.getValue()
        pulseVal = self.pulse.getValue() > 0

        # Detect edges in the pulse input
        if pulseVal != self.lastPulseVal:
            if pulseVal:
                self.leadingEdge()
            else:
                self.fallingEdge()
            self.lastPulseVal = pulseVal

        return self.oscVal

    def leadingEdge(self):
        # Our input pulse is off-center, so even pulses and odd pulses
        # will be notably different. Compare this pulse time to the last
        # one to determine which direction we're going.
        dt = self.pulseTimer.step()
        if dt > self.lastStep:
            if self.c > 0:
                self.direction = 1
            else:
                self.direction = -1
        else:
            if self.c > 0:
                self.direction = -1
            else:
                self.direction = 1
        self.lastStep = dt

        # If we're heading positive, we synchronize to the leading edge
        if self.direction > 0:
            self.sync()

    def fallingEdge(self):
        # If we're heading negative, sync to the falling edge
        if self.direction < 0:
            self.sync()

    def sync(self):
        # Synchronize phase and frequency
        if self.direction > 0:
            # Sync the period only on +1 slope
            period = self.periodTimer.step()
            if period:
                self.oscillator.frequency = 1 / period

            self.oscillator.theta = math.asin(self.c)
        else:
            self.oscillator.theta = math.pi - math.asin(self.c)

        # Synchronize the window position
        self.c += (self.oscVal - self.c) * 0.1
        print self.c


def simulate():
    trueAngle = SineWave("True Angle", 0.5)
    iSensor = WindowComparator("Interruption Sensor", trueAngle, (-0.5, -0.4))
    recoveredAngle = PulseTracker("Recovered Angle", iSensor, SineWave())

    channels = [trueAngle,
                iSensor,
                recoveredAngle,
                ]
    win = rtgraph.GraphUIWindow(channels, rtgraph.HScrollLineGraph(range=(-1,1),
                                                                   pollInterval=2),
                                visibilityDefault = True)
    win.connect("destroy", gtk.mainquit)
    gtk.main()

if __name__ == "__main__":
    simulate()

### The End ###

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
       when the signal is within some small off-center window.
       This is a simple form of digital phase-locked loop.
       """
    # Tolerance for differences in sync window durations. Should be slightly
    # larger than the upper bound on timer noise.
    tolerance = 0.01

    def __init__(self, name, pulse, oscillator):
        rtgraph.Channel.__init__(self, name=name)
        self.pulse = pulse
        self.oscillator = oscillator
        self.reset()

    def reset(self):
        self.pulseTimer = TimeStepper()
        self.locked = False
        self.stepBuffer = []
        self.lastPulseVal = None

    def getValue(self):
        self.oscVal = self.oscillator.getValue()
        pulseVal = self.pulse.getValue() > 0

        # Detect edges in the pulse input, store
        # the last 4 intervals.
        if pulseVal != self.lastPulseVal:
            self.lastPulseVal = pulseVal
            dt = self.pulseTimer.step()
            if dt > 0:
                self.stepBuffer = self.stepBuffer[-3:] + [dt]
                self.sync()

        if self.locked:
            return self.oscVal

    def sync(self):
        # By examining our current step buffer, infer the period and phase
        # of the original signal.
        step = self.stepBuffer

        if len(step) < 4:
            # Not enough info to sync yet
            return

        if step[0] > step[1] or step[2] > step[3]:
            # We're outside of the sync window, wait 'til we're back in
            return

        # Our two sync window samples should be about the same. If not, we
        # probably missed a sample and need to start over.
        if abs(step[0] - step[2]) > self.tolerance:
            self.reset()

        # Adjust the oscillator period by adding all step times
        self.oscillator.frequency = 1 / sum(step)

        # Adjust the oscillator phase. First calculate how much time
        # has elapsed since the origin of this period.
        if step[1] < step[3]:
            # The origin is in the center of step[1]
            t = step[1]/2 + step[2] + step[3]
        else:
            # The origin is in the center of step[2]
            t = step[3]/2

        # Then calculate the current theta using this time and our period
        self.oscillator.theta = -math.pi/2 + 2*math.pi * self.oscillator.frequency * t

        # Yay, we should be synchronized now
        self.locked = True


def simulate():
    trueAngle = SineWave("True Angle", 1)
    iSensor = WindowComparator("Interruption Sensor", trueAngle, (-0.5, -0.4))
    recoveredAngle = PulseTracker("Recovered Angle", iSensor, SineWave(None, 0))

    channels = [trueAngle,
                iSensor,
                recoveredAngle,
                ]
    win = rtgraph.GraphUIWindow(channels, rtgraph.HScrollLineGraph(range=(-1,1)),
                                visibilityDefault = True)
    win.connect("destroy", gtk.mainquit)
    gtk.main()

if __name__ == "__main__":
    simulate()

### The End ###

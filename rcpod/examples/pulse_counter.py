#!/usr/bin/env python
#
# This is a pulse counter with GUI, that uses the RCPOD's hardware
# timer/counter and times the integration period on the host.
# This implementation uses some cpu-hungry polling- it used to use
# threads, which proved to work fine under Linux, but were unacceptably
# slow with libusb-win32.
#
import pyrcpod, time
import gtk.glade, os

class PulseCounter(object):
    """A counter for pulses arriving on RC0 / T1CKI"""
    def __init__(self, rcpod):
        self.rcpod = rcpod
        self.numSamples = 0

    def sample(self, duration, prescale=0):
        """Sample the counter for approximately the specified
           number of seconds.
           """
        timer_off = 0x06 | (prescale << 4)
        timer_on = timer_off | 0x01

        # Stop the timer and reset it
        self.rcpod.poke('t1con', timer_off)
        self.rcpod.poke('tmr1l', 0)
        self.rcpod.poke('tmr1h', 0)

        # Clear the overflow flag
        self.rcpod.poke('pir1', 0)

        # Start timing, bracketing the start time
        t1 = time.clock()
        self.rcpod.poke('t1con', timer_on)
        t2 = time.clock()

        while 1:
            t3 = time.clock()
            if t3 > t1 + duration:
                break
            while gtk.events_pending():
                gtk.main_iteration()

        # Start timing, bracketing the end time
        self.rcpod.poke('t1con', timer_off)
        t4 = time.clock()

        self.counts = self.rcpod.peek('tmr1l') | (self.rcpod.peek('tmr1h') << 8)
        self.overflow = bool(self.rcpod.peek('pir1') & 0x01)

        self.minDuration = t3 - t2
        self.maxDuration = t4 - t1

        self.iTimeCenter = (self.minDuration + self.maxDuration) / 2.0
        self.iTimeError = (self.maxDuration - self.minDuration) / 2.0

        self.numSamples += 1

    def sampleFrequency(self, duration):
        self.sample(duration)
        minFrequency = self.counts / (self.iTimeCenter + self.iTimeError)
        maxFrequency = self.counts / (self.iTimeCenter - self.iTimeError)
        
        self.frequency = (minFrequency + maxFrequency) / 2.0
        self.frequencyError = (maxFrequency - minFrequency) / 2.0


class SamplerWindow:
    def __init__(self, dev, gladeFile="data/pulse_counter.glade"):
        self.dev = dev
        self.xml = gtk.glade.XML(gladeFile)
        self.window = self.xml.get_widget("window")
        self.window.show_all()

    def run(self):
        self.counter = PulseCounter(self.dev)
        while 1:
            self.counter.sampleFrequency(self.getSamplingDelay())
            self.showResults()

    def getSamplingDelay(self):
        return self.xml.get_widget("sampling_delay").get_adjustment().get_value()

    def showResults(self):
        count = str(self.counter.counts)
        if self.counter.overflow:
            count = "%s (overflow)" % count

        for widget, value in (
            ("pulse_count", count),
            ("integration_time", "%.04f" % self.counter.iTimeCenter),
            ("integration_time_error", "%.04f" % self.counter.iTimeError),
            ("frequency", "%.01f" % self.counter.frequency),
            ("frequency_error", "%.01f" % self.counter.frequencyError),
            ):
            self.xml.get_widget(widget).set_text(value) 

if __name__ == "__main__":
    w = SamplerWindow(pyrcpod.devices[0].open(reset=False))
    w.window.connect("destroy", gtk.main_quit)
    w.run()

### The End ###

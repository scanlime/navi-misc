#!/usr/bin/env python
import pyrcpod, time, threading
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
        t1 = time.time()
        self.rcpod.poke('t1con', timer_on)
        t2 = time.time()

        time.sleep(duration)

        # Start timing, bracketing the end time
        t3 = time.time()
        self.rcpod.poke('t1con', timer_off)
        t4 = time.time()

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


class SamplerThread(threading.Thread):
    def __init__(self, availableDevice, synchronize):
        self.availableDevice = availableDevice
        self.duration = 0.1
        self.synchronize = synchronize
        threading.Thread.__init__(self)

    def run(self):
        self.running = True
        self.dev = self.availableDevice.open(reset=False)
        self.counter = PulseCounter(self.dev)
        while self.running:
            self.counter.sampleFrequency(self.duration)
        self.dev.close()


class SamplerWindow:
    def __init__(self, dev, gladeFile="data/pulse_counter.glade"):
        self.xml = gtk.glade.XML(gladeFile)
        self.window = self.xml.get_widget("window")
        self.thread = SamplerThread(dev, self.synchronize)

    def start(self):        
        w.window.show_all()
        self.thread.start()
        gtk.timeout_add(100, self.synchronize)

    def synchronize(self):
        self.thread.duration = self.xml.get_widget("sampling_delay").get_adjustment().get_value()
        if hasattr(self.thread, 'counter') and self.thread.counter.numSamples > 0:

            count = str(self.thread.counter.counts)
            if self.thread.counter.overflow:
                count = "%s (overflow)" % count

            for widget, value in (

                ("pulse_count", count),
                ("integration_time", "%.04f" % self.thread.counter.iTimeCenter),
                ("integration_time_error", "%.04f" % self.thread.counter.iTimeError),
                ("frequency", "%.01f" % self.thread.counter.frequency),
                ("frequency_error", "%.01f" % self.thread.counter.frequencyError),

                ):
                self.xml.get_widget(widget).set_text(value)
        return True


if __name__ == "__main__":
    w = SamplerWindow(pyrcpod.devices[0])
    w.window.connect("destroy", gtk.main_quit)

    try:
        # Windows just hangs in gtk.threads_init()
        if os.name != 'nt':
            gtk.threads_init()

        w.start()
        gtk.main()
    finally:
        w.thread.running = False

### The End ###

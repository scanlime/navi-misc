#!/usr/bin/env python
#
# A daemon that takes therm readings, averages them,
# and provides an XML-RPC interface to retrieve them.
#
# Run with the '-f' option to fork into the background.
#
# -- Micah Dowty <micah@picogui.org>
#

import i2c, os, sys, time
from SimpleXMLRPCServer import SimpleXMLRPCServer
from threading import Thread


class ThermServer(SimpleXMLRPCServer):
    """An XML-RPC server providing interfaces for measuring current thermometer data."""
    def __init__(self, samplerThread):
        SimpleXMLRPCServer.__init__(self, ("", 4510))
        self.sampler = samplerThread
        self.register_function(self.getAverages)
        self.register_function(self.getTimes)
        self.register_function(self.setAveragePeriod)
        self.register_function(self.getDescriptions)

    def getAverages(self):
        """Return average temperature data for all therms.
           Results in a dictionary mapping therm ID to temperature in degrees fahrenheit.
           """
        return self.sampler.averages

    def getDescriptions(self):
        """Return a dictionary mapping therm IDs to their description strings"""
        d = {}
        for therm in self.sampler.therms:
            d[therm.id] = therm.description
        return d

    def getTimes(self):
        """Returns the current time, averaging period, and the time the current averaging
           period started.
           """
        return {
            'averagePeriod': self.sampler.averagePeriod,
            'periodStart': self.sampler.periodStart,
            'time': time.time(),
            }

    def setAveragePeriod(self, t):
        """Change the current average period, in seconds. Takes effect immediately.
           Returns the old average period.
           """
        old = self.sampler.averagePeriod
        self.sampler.averagePeriod = t
        return old


class SamplerThread(Thread):
    """A thread which samples the therms and creates a running average of their values"""
    def __init__(self):
        Thread.__init__(self)
        self.sampler = i2c.ThermSampler()
        self.therms = i2c.getTherms()

        # Default averaging period, 1 minute
        self.averagePeriod = 60
        self.periodStart = 0

        # Save a reference to our blinkable LED
        self.led = self.sampler.rcpod.rc2
        self.led.output().assert_()

        self.averages = {}
        self.running = True
        self.start()

    def run(self):
        while True:
            self.periodStart = time.time()

            while time.time() < (self.periodStart + self.averagePeriod):
                self.sampler.update(self.therms)

                # Blink our LED as an indication of happiness and life
                self.led.assert_()
                self.led.negate().assert_()

                if not self.running:
                    return

            for therm in self.therms:
                average = therm.getAverage()
                if average is not None:
                    average = i2c.toFahrenheit(average)
                    self.averages[therm.id] = average
                else:
                    try:
                        # No data? nuke our stored averages for this sensor
                        del self.averages[therm.id]
                    except KeyError:
                        pass


def daemonize(pidfile=None):
    """Daemonize this process with the UNIX double-fork trick.
       Writes the new PID to the provided file name if not None.
       """
    pid = os.fork()
    if pid > 0:
        sys.exit(0)
    os.setsid()
    os.umask(0)
    pid = os.fork()
    if pid > 0:
        if pidfile:
            open(pidfile, "w").write(str(pid))
        sys.exit(0)

def main():
    # The -f option forks into the background
    if len(sys.argv) > 1 and sys.argv[1] == '-f':
        daemonize("thermserver.pid")
        sys.stderr = open("thermserver.log", "w")

    sampler = SamplerThread()
    try:
        server = ThermServer(sampler)
        server.serve_forever()
    finally:
        sampler.running = False

if __name__ == '__main__':
    main()

### The End ###

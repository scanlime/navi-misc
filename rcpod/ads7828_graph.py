#!/usr/bin/env python
#
# An example UI for graphing the input channels
# of a single ADS7828 12-bit I2C A/D converter.
# This assumes the following connections:
#   - SCL to RB0
#   - SDA to RB1, with 4.7k pullup
#
# The I2C address can be given on the command line. Defaults to 0
#
# Requires pygtk 2 and the rtgraph package from the
# Subversion repository at:
#
#  http://navi.picogui.org/svn/misc/trunk/rtgraph/
#
# -- Micah Dowty <micah@navi.cx>
#

from __future__ import division
import gtk, rtgraph, threading, time, sys
import pyrcpod
import pyrcpod.i2c


class AnalogUpdaterThread(threading.Thread):
    def __init__(self, channels, address):
        threading.Thread.__init__(self)
        self.channels = channels
        self.rcpod = pyrcpod.devices[0].open()
        self.bus = pyrcpod.i2c.Bus(self.rcpod.rb0, self.rcpod.rb1)
        self.adc = pyrcpod.i2c.ti.ADS7828(self.bus, address)
        self.running = True
        self.start()

    def run(self):
        while self.running:
            for channel in range(8):
                try:
                    self.channels[channel].value = self.adc.read(channel)
                except pyrcpod.i2c.I2CError:
                    print "I2C Error"

def main():
    if len(sys.argv) > 1:
        address = int(sys.argv[1])
    else:
        address = 0

    channels = [rtgraph.Channel(name="CH%d" % i) for i in xrange(8)]
    analogThread = AnalogUpdaterThread(channels, address)

    graph = rtgraph.HScrollLineGraph(range=(0.0, 1.0), scrollRate=10)
    win = rtgraph.GraphUIWindow(channels, graph, valueUpdateInterval=500)
    win.connect("destroy", gtk.mainquit)

    try:
        gtk.threads_init()
        gtk.main()
    finally:
        analogThread.running = False

if __name__ == "__main__":
    main()

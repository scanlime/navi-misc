#!/usr/bin/env python
#
# An example UI for graphing the input channels
# of a single ADS1112 16-bit I2C A/D converter.
# This assumes the following connections:
#   - SCL to RD0
#   - SDA to RD1, with 4.7k pullup
#   - A0 and A1 tied to ground
#
# Requires pygtk 2 and the rtgraph package from the
# Subversion repository at:
#
#  http://navi.picogui.org/svn/misc/trunk/rtgraph/
#
# -- Micah Dowty <micah@navi.cx>
#

from __future__ import division
import gtk, rtgraph, threading, time
import pyrcpod
import pyrcpod.i2c


class AnalogUpdaterThread(threading.Thread):
    def __init__(self, channels):
        threading.Thread.__init__(self)
        self.channels = channels
        self.rcpod = pyrcpod.devices[0].open()
        self.bus = pyrcpod.i2c.Bus(self.rcpod.rd0, self.rcpod.rd1)
        self.adc = pyrcpod.i2c.ti.ADS1112(self.bus, 0)
        self.running = True
        self.start()

    def run(self):
        while self.running:
            for channel in range(1):
                self.channels[channel].value = self.adc.read(channel)

def main():
    channels = [
        rtgraph.Channel(name="AIN0 - AIN1"),
        rtgraph.Channel(name="AIN2 - AIN3"),
        rtgraph.Channel(name="AIN0 - AIN3"),
        rtgraph.Channel(name="AIN1 - AIN3"),
        ]
    analogThread = AnalogUpdaterThread(channels)

    graph = rtgraph.HScrollLineGraph(range=(-1.0, 1.0), scrollRate=10)
    win = rtgraph.GraphUIWindow(channels, graph, valueUpdateInterval=500)
    win.connect("destroy", gtk.mainquit)

    try:
        gtk.threads_init()
        gtk.main()
    finally:
        analogThread.running = False

if __name__ == "__main__":
    main()

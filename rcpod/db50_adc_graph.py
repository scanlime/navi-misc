#!/usr/bin/env python
#
# A gtk UI for graphing ADC readings. This version
# is for the rcpod-db50 board, and supports its
# multiplexed high-voltage analog inputs.
#
# Requires pygtk 2 and the rtgraph package from the
# Subversion repository at:
#
#  http://navi.picogui.org/svn/misc/trunk/rtgraph/
#
# -- Micah Dowty <micah@picogui.org>
#

from __future__ import division
import gtk, rtgraph, threading, pyrcpod, time

class AdcChannel(rtgraph.Channel):
    def strValue(self):
        """Represent this channel's value as a voltage"""
        if self.value is not None:
            return "%.02f V" % self.value

class AnalogUpdaterThread(threading.Thread):
    def __init__(self, channels):
        threading.Thread.__init__(self)
        self.channels = channels
        self.pinChannelMap = {}
        self.rcpod = pyrcpod.devices[0].open(pyrcpod.RcpodDB50)
        self.running = True
        self.start()

    def run(self):
        while self.running:
            for pin, value in self.rcpod.analogScan().iteritems():
                try:
                    c = self.pinChannelMap[pin]
                except KeyError:
                    if pin <= 10:
                        fmt = "Pin %d (0-5V only)"
                    else:
                        fmt = "Pin %d (0-28V)"
                    c = AdcChannel(name=fmt % pin)
                    self.pinChannelMap[pin] = c
                    self.channels.append(c)
                c.value = value

def main():
    channels = []
    analogThread = AnalogUpdaterThread(channels)

    # Wait for our channel list to be populated
    while not channels:
        time.sleep(0.1)

    graph = rtgraph.HScrollLineGraph(range=(0.0, 30.0), scrollRate=10)
    win = rtgraph.GraphUIWindow(channels, graph, valueUpdateInterval=500)
    win.connect("destroy", gtk.mainquit)

    try:
        gtk.threads_init()
        gtk.main()
    finally:
        analogThread.running = False

if __name__ == "__main__":
    main()

#!/usr/bin/env python
#
# A gtk UI for graphing ADC readings.
# Requires pygtk 2 and the rtgraph package from the
# Subversion repository at:
#
#  http://navi.picogui.org/svn/misc/trunk/rtgraph/
#
# -- Micah Dowty <micah@picogui.org>
#

import gtk, rtgraph, threading, pyrcpod, time


class AnalogUpdaterThread(threading.Thread):
    def __init__(self, channels):
        threading.Thread.__init__(self)
        self.channels = channels
        self.rcpod = pyrcpod.devices[0].open()
        self.running = True
        self.start()

    def run(self):
        while self.running:
            readings = self.rcpod.analogReadAll()
            for i in xrange(len(readings)):
                self.channels[i].value = readings[i]

            # Very tiny delay, just so the scheduler doesn't
            # let this thread hog all the CPU.
            time.sleep(0.0001)


def main():
    channels = [rtgraph.Channel(name="Analog channel %d" % i) for i in xrange(8)]
    graph = rtgraph.HScrollLineGraph(range=(0,255))
    win = rtgraph.GraphUIWindow(channels, graph)
    win.connect("destroy", gtk.mainquit)

    analogThread = AnalogUpdaterThread(channels)
    try:
        gtk.threads_init()
        gtk.main()
    finally:
        analogThread.running = False


if __name__ == "__main__":
    main()

#!/usr/bin/env python
#
# A simple CPU meter app using HScrollAreaGraph.
# Demonstrates HScrollAreaGraph, and setting custom graph colors.
# The implementation of CPUActivityChannel here only works with Linux.
#
# -- Micah Dowty <micah@picogui.org>
#

from __future__ import division
import gtk, rtgraph, re
import graph

class CPUActivityChannel(graph.Channel):
    lastStatValues = None
    activity = None

    def readCpuStat(self):
        """Reads the 'cpu' line in /proc/stat"""
        f = open("/proc/stat")
        for line in f.xreadlines():
            if line.startswith("cpu "):
                cpu, user, nice, sys, idle = re.split("\s+", line.strip())
                break
        f.close()
        return (long(user) + long(nice + sys), long(idle))

    def getValue(self):
        newStatValues = self.readCpuStat()

        if self.lastStatValues:
            active = newStatValues[0] - self.lastStatValues[0]
            idle = newStatValues[1] - self.lastStatValues[1]
            if active + idle > 0:
                self.activity = active / (active + idle)

        self.lastStatValues = newStatValues
        return self.activity


graph = graph.HScrollAreaGraph(
    scrollRate = 4,
    pollInterval = 200,
    size       = (128,48),
    gridSize   = 8,
    channels   = [CPUActivityChannel(color=(1, 1, 1))],
    bgColor    = (0, 0, 0.3),
    gridColor  = (0, 0, 0.5),
    )
graph.show()

frame = gtk.Frame()
frame.add(graph)
frame.set_shadow_type(gtk.SHADOW_OUT)
frame.show()

win = gtk.Window(gtk.WINDOW_TOPLEVEL)
win.set_title("CPU Load")
win.set_decorated(gtk.FALSE)
win.stick()
win.add(frame)
win.show()
win.connect("destroy", gtk.mainquit)

gtk.main()

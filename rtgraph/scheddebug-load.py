#!/usr/bin/env python
#
# Utility to graph per-CPU load averages calculated by Linux's scheduler.
# Requires /proc/sched_debug
#
# -- Micah Dowty <micah@navi.cx>
#

import gtk
import rtgraph
import math

def parseSchedDebug():
    """Parse /proc/sched_debug. Returns a list with one entry for each
       CPU. Each entry is a dictionary mapping debug variables to integer values.
       """
    cpus = []
    currentCpu = None

    for line in open("/proc/sched_debug"):
        if line.startswith('cpu#'):
            currentCpu = {}
            cpus.append(currentCpu)

        elif len(line) > 4 and line[2] == '.':
            line = line[3:]
            key, value = line.split(':')
            currentCpu[key.strip()] = int(value.strip())

    return cpus


class SchedDebugChannel(rtgraph.Channel):
    def __init__(self, viewer, cpu, name):
        self.viewer = viewer
        self.cpu = cpu
        self.name = name
        self.lastGeneration = 0
        rtgraph.Channel.__init__(self, 0, name=name)

    def getValue(self):
        if self.lastGeneration != self.viewer.generation:
            self.viewer.update()

        self.lastGeneration = self.viewer.generation
        return self.viewer.cpus[self.cpu][self.name]


class LoadViewer:
    def __init__(self):
        self.generation = 0
        self.update()

        # Populate the UI
        self.win = gtk.Window(gtk.WINDOW_TOPLEVEL)
        cpus = self.createCPUs(len(self.cpus))
        self.win.add(cpus)
        self.win.show()

    def update(self):
        self.generation += 1
        self.cpus = parseSchedDebug()

    def createCPUs(self, numCpus):
        hbox = gtk.HBox(False, 1)
        for cpu in xrange(numCpus):

            channels = [
                SchedDebugChannel(self, cpu, 'cpu_load[%d]' % i)
                for i in range(5)
                ]

            vbox = gtk.VBox(False, 1)
            label = gtk.Label()
            label.set_markup("<b>cpu%d</b>" % cpu)
            label.show()
            vbox.pack_start(label, False)

            ui = rtgraph.GraphUI(channels,
                                 rtgraph.HScrollLineGraph(range = (0, 10000),
                                                          size = (400, 300),
                                                          pollInterval = 100,
                                                          scrollRate = 10),
                                 valueUpdateInterval = 500,
                                 visibilityDefault = True,
                                 showTweakControls = False)
            ui.show()
            vbox.pack_start(ui)
            vbox.show()

            hbox.pack_start(vbox)
        hbox.show()
        return hbox


if __name__ == "__main__":
    ui = LoadViewer()
    ui.win.set_title("Scheduler CPU Load")
    ui.win.connect('destroy', gtk.main_quit)
    gtk.main()

#!/usr/bin/env python
#
# Utility to graph Linux scheduler statistics in real-time.
# See http://eaglet.rain.com/rick/linux/schedstat/
#
# -- Micah Dowty <micah@navi.cx>
#

import gtk
import rtgraph
import math

class SchedCPU:
    statsGeneration = 0

    def __init__(self, name):
        self.name = name
        self.stats = {}
        self.domains = []


class SchedDomain:
    statsGeneration = 0

    def __init__(self, name, cpumask):
        self.name = name
        self.cpumask = cpumask
        self.stats = {}


class SchedstatParser:
    formats = {
        '14': {
            'cpu': [
                'yld_both_empty', 'yld_act_empty', 'yld_exp_empty',
                'yld_cnt', 'sched_switch', 'sched_cnt', 'sched_goidle',
                'ttwu_cnt', 'ttwu_local', 'rq_cpu_time', 'rq_run_delay',
                'rq_pcnt'
            ],
            'domain': [
                'idle_cnt', 'idle_balanced', 'idle_failed', 'idle_imbalance',
                'idle_gained', 'idle_hot_gained', 'idle_nobusyq', 'idle_nobusyg',
                'busy_cnt', 'busy_balanced', 'busy_failed', 'busy_imbalance',
                'busy_gained', 'busy_hot_gained', 'busy_nobusyq', 'busy_nobusyg',
                'newidle_cnt', 'newidle_balanced', 'newidle_failed', 'newidle_imbalance',
                'newidle_gained', 'newidle_hot_gained', 'newidle_nobusyq',
                'newidle_nobusyg', 'alb_cnt', 'alb_failed', 'alb_pushed', 'sbe_cnt',
                'sbe_balanced', 'sbe_pushed', 'sbf_cnt', 'sbf_balanced', 'sbf_pushed',
                'ttwu_wake_remote', 'ttwu_move_affine', 'ttwu_move_balance',
            ],
        },
    }

    def __init__(self):
        self._cpuMemo = {}
        self._domainMemo = {}

    def parse(self):
        """Parse the current schedstats. Returns a list of SchedCPU instances.
           The returned CPU and Domain instances will match those returned by
           previous invokations, if applicable.
           """
        cpus = []
        currentCPU = None
        format = None

        for line in open("/proc/schedstat"):
            tokens = line.split()

            if tokens[0] == 'version':
                format = self.formats.get(tokens[1])

            elif tokens[0].startswith('cpu'):
                if tokens[0] in self._cpuMemo:
                    currentCPU = self._cpuMemo[tokens[0]]
                else:
                    currentCPU = self._cpuMemo[tokens[0]] = SchedCPU(tokens[0])

                values = map(int, tokens[1:])
                assert len(format['cpu']) == len(values)
                currentCPU.stats = dict(zip(format['cpu'], values))
                currentCPU.statsGeneration += 1
                cpus.append(currentCPU)

            elif tokens[0].startswith('domain'):
                key = id(currentCPU), tokens[0]
                if key in self._domainMemo:
                    currentDomain = self._domainMemo[key]
                else:
                    currentDomain = self._domainMemo[key] = SchedDomain(
                        tokens[0], int(tokens[1]))
                    currentCPU.domains.append(currentDomain)

                values = map(int, tokens[2:])
                assert len(format['domain']) == len(values)
                currentDomain.stats = dict(zip(format['domain'], values))
                currentDomain.statsGeneration += 1

        return cpus


class CounterChannel(rtgraph.Channel):
    """Channel for an incrementing stat counter. Always reports the
       diff since the last time it was read. Note that reading this
       counter has side-effects!

       This counter can automatically trigger updates which may affect
       multiple counters. If this counter is being polled and the
       provided stats object has no new data, we'll call "updater".
       """
    def __init__(self, obj, name, updater=None):
        self.obj = obj
        self.name = name
        self.updater = updater
        self.previous = obj.stats[name]
        self.lastGeneration = 0
        rtgraph.Channel.__init__(self, 0, name=name)

    def getValue(self):
        if self.updater and self.lastGeneration == self.obj.statsGeneration:
            self.updater()

        next = self.obj.stats[self.name]
        prev = self.previous
        self.previous = next
        self.lastGeneration = self.obj.statsGeneration
        diff = next - prev

        # Logarithmic scale
        if diff > 0:
            return math.log10(diff * 10)
        else:
            return 0


class SchedstatViewer:
    def __init__(self):
        """Perform the initial parse to determine the available CPUs and
           Domains, then construct a UI to match.
           """
        self.parser = SchedstatParser()

        # Populate the UI
        self.win = gtk.Window(gtk.WINDOW_TOPLEVEL)
        cpus = self.createCPUs(self.parser.parse())
        self.win.add(cpus)
        self.win.show()

    def createCPUs(self, cpuList):
        hbox = gtk.HBox(False, 1)
        for cpu in cpuList:
            vbox = gtk.VBox(False, 1)
            vbox.pack_start(self.createGraph(cpu))
            for domain in cpu.domains:
                vbox.pack_start(self.createGraph(domain))
            vbox.show()
            hbox.pack_start(vbox)
        hbox.show()
        return hbox

    def createGraph(self, obj, skipUnusedCounters=True):
        obj.channels = []
        obj.channelDict = {}

        names = obj.stats.keys()
        names.sort()
        for name in names:
            if not skipUnusedCounters or obj.stats[name] != 0:
                channel = CounterChannel(obj, name, updater=self.parser.parse)
                obj.channels.append(channel)
                obj.channelDict[name] = channel

        box = gtk.VBox(False, 1)

        label = gtk.Label()
        label.set_markup("<b>%s</b>" % obj.name)
        label.show()
        box.pack_start(label, False)

        ui = rtgraph.GraphUI(obj.channels,
                             rtgraph.HScrollLineGraph(range = (0, 10),
                                                      size = (400, 300),
                                                      pollInterval = 100,
                                                      scrollRate = 10),
                             visibilityDefault = True,
                             showTweakControls = False)
        ui.show()
        box.pack_start(ui)
        box.show()
        return box


if __name__ == "__main__":
    ui = SchedstatViewer()
    ui.win.set_title("Linux Scheduler Stats")
    ui.win.connect('destroy', gtk.main_quit)
    gtk.main()

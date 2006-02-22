#!/usr/bin/env python
#
# Collect CPU usage stats per-process, binned by the name
# of the process' binary. Dumps a CSV to stdout when you
# ctrl-C it.
#
# -- Micah Dowty <micah@navi.cx>
#
import os, time, sys, csv

class ProcessStat:
    def __init__(self, pid):
        self._stat = open("/proc/%d/stat" % pid).read().split()
        self.pid = pid
        self.exeName = self._stat[1]
        self.cpuTime = int(self._stat[13]) + int(self._stat[14])

def iterProcesses():
    for pid in os.listdir("/proc"):
        try:
            yield ProcessStat(int(pid))
        except ValueError, OSError:
            continue

class CpuTimeAccumulator:
    def __init__(self):
        self._prevStats = {}
        self.times = {}

    def collect(self):
        """Collect a new round of process stats, and compare
           them to the previous round. For any processes that appear
           to still be around, collect CPU time stats.
           """
        newStats = {}
        for stat in iterProcesses():
            oldStat = self._prevStats.get(stat.pid)
            if oldStat and stat.exeName == oldStat.exeName:
                self.collectDelta(stat.exeName,
                                  stat.cpuTime - oldStat.cpuTime)
            newStats[stat.pid] = stat
        self._prevStats = newStats

    def collectDelta(self, name, delta):
        if delta:
            self.times[name] = delta + self.times.get(name, 0)

    def top(self):
        """Return a list of (name, time) tuples, sorted by
           decreasing CPU time.
           """
        items = self.times.items()
        items.sort(lambda a,b: cmp(b[1], a[1]))
        return items

if __name__ == "__main__":
    acc = CpuTimeAccumulator()
    dataPoints = []
    epoch = time.time()

    try:
        while True:
            time.sleep(0.5)
            now = time.time() - epoch
            acc.collect()
            
            # Collect a snapshot of process times at each iteration
            dataPoints.append((now, dict(acc.times)))

    except KeyboardInterrupt:
        pass

    # Make a consistently ordered list of process names for the
    # graph heading. In this case, sort them by decreasing
    # CPU times so the interesting items end up on the left side
    # of the graph.
    processNames = [name for name, cpuTime in acc.top()]
    output = csv.writer(sys.stdout)
    output.writerow(['time'] + processNames)

    # Now output a row for each sample we took earlier
    prevSample = {}
    prevTime = 0
    for timestamp, sample in dataPoints:
        row = [timestamp]

        for name in processNames:
            deltaCpu = sample.get(name, 0) - prevSample.get(name, 0)
            deltaTime = timestamp - prevTime
            row.append(deltaCpu / deltaTime)

        prevTime, prevSample = timestamp, sample
        output.writerow(row)

    

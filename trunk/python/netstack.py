#!/usr/bin/env python
"""netstack

A stack of graphs and other information displays that collect
data locally and from other machines on the network. Currently
only targeted at Linux.

  netstack distributed monitoring applet
  Copyright (C) 2003 Micah Dowty <micahjd@users.sourceforge.net>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
"""

from __future__ import division
import gtk, rtgraph, threading, time, popen2


class MeterThread(threading.Thread):
    """A thread that measures some quantity and reports it
       to provided callbacks when it is available. Callbacks
       receive one parameter, a dictionary mapping readings
       to values.
       """
    def __init__(self):
        threading.Thread.__init__(self)
        self.callbacks = []
        self.running = True
        self.start()

    def stop(self):
        self.running = False

    def postData(self, data):
        """Call all callbacks with a dictionary of new data"""
        for callback in self.callbacks:
            callback(data)


class CPUMeter(MeterThread):
    """Abstract base for classes that measures CPU activity,
       and calls the provided list of functions with a dict
       holding the completed measurements. This dict will
       contain the keys 'user', 'nice', 'sys', and 'idle', and
       values between 0 and 1.
       """
    def __init__(self, updatePeriod=1):
        MeterThread.__init__(self)
        self.oldStats = None
        self.updatePeriod = updatePeriod

    def update(self, line):
        """Subclasses should call this with the 'cpu' line from /proc/stat"""
        newStats = line[3:].strip().split(" ")
        if self.oldStats:
            user  = int(newStats[0]) - int(self.oldStats[0])
            nice  = int(newStats[1]) - int(self.oldStats[1])
            sys   = int(newStats[2]) - int(self.oldStats[2])
            idle  = int(newStats[3]) - int(self.oldStats[3])
            total = user + nice + sys + idle

            self.postData({
                'user': user / total,
                'nice': nice / total,
                'sys': sys / total,
                'idle': idle / total,
                })
        self.oldStats = newStats


class LocalCPUMeter(CPUMeter):
    """Measures local CPU activity using Linux's /proc/stat"""
    def run(self):
        while self.running:
            f = open("/proc/stat")
            self.update(f.readline())
            f.close()
            time.sleep(self.updatePeriod)


class RemoteCPUMeter(CPUMeter):
    """Measures local CPU activity on a remote Linux machine, via ssh"""
    def __init__(self, hostspec, updatePeriod=1):
        self.ssh = popen2.Popen3('ssh "%s" "while true; do head -n1 /proc/stat; sleep %s; done"' %
                                 (hostspec, updatePeriod))
        CPUMeter.__init__(self, updatePeriod)

    def run(self):
        while self.running:
            self.update(self.ssh.fromchild.readline())


class RemoteMaildirMeter(MeterThread):
    """Counts available email messages in one or more directories on a remote machine, via ssh"""
    def __init__(self, hostspec, dirs={'new': '~/.maildir/new', 'read': '~/.maildir/cur'}, updatePeriod=5):
        # Store our mail categories in a definite order
        self.categories = dirs.keys()

        # Construct a command to count files in all directories
        # given, ordered to match self.categories
        dirCmds = " ".join(['`ls %s | wc -l`' % dirs[cat] for cat in self.categories])
        command = "ssh '%s' 'while true; do echo %s; sleep %s; done'" % (hostspec, dirCmds, updatePeriod)
        self.ssh = popen2.Popen3(command)

        self.updatePeriod = updatePeriod
        MeterThread.__init__(self)

    def run(self):
        while self.running:
            # Now each line we read contains numbers for self.categories, in order
            counts = self.ssh.fromchild.readline().strip().split(" ")
            d = {}
            for i in xrange(len(self.categories)):
                d[self.categories[i]] = counts[i]
            self.postData(d)


class View:
    """A collection of gtk widgets that views data collected from a Meter"""
    def __init__(self, meter, widget):
        self.meter = meter
        self.widget = widget
        self.expand = gtk.TRUE   # Whether or not to expand to fill extra space when packing this widget
        meter.callbacks.append(self.meterCallback)


class TextView(View):
    """A view that presents the meter's dictionary in plain text"""
    def __init__(self, meter):
        self.label = gtk.Label()
        self.label.show()
        frame = gtk.Frame()
        frame.add(self.label)
        frame.set_shadow_type(gtk.SHADOW_IN)
        View.__init__(self, meter, frame)

    def meterCallback(self, data):
        self.label.set_text("\n".join(["%s: %s" % item for item in data.items()]))


class CPUView(View):
    """A view that presents CPU activity as a stacked area graph"""
    def __init__(self,
                 meter,
                 size      = (96,48),
                 gridSize  = 8,
                 bgColor   = (0, 0, 0.3),
                 gridColor = (0, 0, 0.5),
                 userColor = (1, 1, 1),
                 niceColor = (0.5, 0.5, 0.5),
                 sysColor  = (0.5, 0.5, 1),
                 ):
        self.niceChannel = rtgraph.Channel(color=niceColor)
        self.userChannel = rtgraph.Channel(color=userColor)
        self.sysChannel = rtgraph.Channel(color=sysColor)

        self.graph = rtgraph.HScrollAreaGraph(
            scrollRate   = 1/meter.updatePeriod,
            pollInterval = int(meter.updatePeriod * 1000),
            size         = size,
            gridSize     = gridSize,
            channels     = [self.niceChannel, self.userChannel, self.sysChannel],
            bgColor      = bgColor,
            gridColor    = gridColor,
            )
        self.graph.show()

        frame = gtk.Frame()
        frame.add(self.graph)
        frame.set_shadow_type(gtk.SHADOW_IN)
        View.__init__(self, meter, frame)

    def meterCallback(self, data):
        # Each channel must include the height of channels below it for them to appear stacked
        self.niceChannel.value = data['nice'] + data['user'] + data['sys']
        self.userChannel.value = data['user'] + data['sys']
        self.sysChannel.value  = data['sys']


class Stack:
    """A list of View instances and labels, stacked vertically into a gtk VBox widget"""
    def __init__(self, *items):
        self.box = gtk.VBox()
        for item in items:

            if type(item) == str:
                # A string item, adds a label to the stack
                widget = gtk.Label(item)
                expand = gtk.FALSE

            elif item is None:
                # None, adds a separator
                widget = gtk.HSeparator()
                expand = gtk.FALSE

            else:
                # Something else, assume it's a View
                widget = item.widget
                expand = item.expand

            widget.show()
            self.box.pack_start(widget, expand)


class Window(Stack):
    """A stack represented in a top-level window"""
    def __init__(self, *items):
        Stack.__init__(self, *items)

        self.box.set_border_width(1)
        self.box.show()

        self.frame = gtk.Frame()
        self.frame.add(self.box)
        self.frame.set_shadow_type(gtk.SHADOW_OUT)
        self.frame.show()

        self.win = gtk.Window(gtk.WINDOW_TOPLEVEL)
        self.win.set_decorated(gtk.FALSE)
        self.win.stick()
        self.win.add(self.frame)
        self.win.show()
        self.win.connect("destroy", gtk.mainquit)


def main(configFunc):
    """Run the provided configuration function then the gtk main loop,
       terminating all threads cleanly on exit
       """
    try:
        gtk.threads_init()
        configFunc()
        gtk.main()
    finally:
        for thread in threading.enumerate():
            if isinstance(thread, MeterThread):
                thread.stop()
                thread.join()


def myStackConfig():
    """A sample configuration function. This could be put into a separate
       file to easily allow multiple configurations.
       """
    Window(
        "Local CPU:",
        CPUView(LocalCPUMeter()),
        "Navi CPU:",
        CPUView(RemoteCPUMeter("navi")),
        "Wasabi CPU:",
        CPUView(RemoteCPUMeter("wasabi")),
        "Mail:",
        TextView(RemoteMaildirMeter("navi")),
        )


if __name__ == '__main__':
    main(myStackConfig)

#!/usr/bin/env python
from efs import FieldSensor
import gtk, rtgraph, Tweak

class ParamTweaker(object):
    def __init__(self, efs, num, default):
        self.efs = efs
        self.num = num
        self.set(default)

    def set(self, value):
        for block in xrange(8):
            self.efs.setParam(block, self.num, value)
        self._value = value

    def get(self):
        return self._value

    value = property(get, set)


def main():
    channels = [rtgraph.Channel(name="Parameter block %d" % i) for i in xrange(8)]
    efs = FieldSensor()

    def poll_handler():
        averages = efs.readAverages()
        if averages:
            for i in xrange(8):
                channels[i].value = averages[i]
        gtk.timeout_add(10, poll_handler)
    gtk.timeout_add(10, poll_handler)

    graph = rtgraph.GraphUI(channels,
                            rtgraph.HScrollLineGraph(range=(0,255)))

    tweaker = Tweak.List([
        Tweak.Quantity(ParamTweaker(efs, 3, default=227), 'value', range=(0,255), name="Period"),
        Tweak.Quantity(ParamTweaker(efs, 4, default=226), 'value', range=(0,255), name="Phase"),
        ])

    win = gtk.Window(gtk.WINDOW_TOPLEVEL)
    vbox = gtk.VBox()
    win.add(vbox)
    win.set_border_width(5)
    vbox.pack_start(graph, gtk.TRUE)
    vbox.pack_start(tweaker, gtk.FALSE)
    win.set_default_size(400,400)
    win.show_all()
    win.connect("destroy", gtk.mainquit)
    gtk.main()

if __name__ == "__main__":
    main()

#!/usr/bin/env python
#
# Interactive parameter tweaker for Raster Wand.
# This lets you adjust all timing parameters, and graphs
# the wand period in real-time. Requires rtgraph.
#
# -- Micah Dowty <micah@navi.cx>
#
import rasterwand, gtk, Tweak, rtgraph


class PeriodChannel(rtgraph.Channel):
    _struct = "HHBBBB"

    def __init__(self, dev):
        rtgraph.Channel.__init__(self)
        self.dev = dev

    def getValue(self):
        return (self.dev.status.period - 39500) / 10000.0


def main():
    dev = rasterwand.Device()

    graph = rtgraph.HScrollLineGraph(channels=[
        PeriodChannel(dev),
        ])

    tweaker = Tweak.List([
        Tweak.Quantity(dev.params, 'display_center', range=(0,0xFFFF)),
        Tweak.Quantity(dev.params, 'display_width', range=(0,0xFFFF)),
        Tweak.Quantity(dev.params, 'coil_center', range=(0,0xFFFF)),
        Tweak.Quantity(dev.params, 'coil_width', range=(0,0xFFFF)),
        Tweak.Quantity(dev.params, 'duty_cycle', range=(0,0xFFFF)),
        Tweak.Quantity(dev.params, 'fine_adjust', range=(-1000,1000)),
        ])

    win = gtk.Window(gtk.WINDOW_TOPLEVEL)
    vbox = gtk.VBox()
    win.add(vbox)
    win.set_border_width(5)
    frame = gtk.Frame()
    frame.add(graph)
    vbox.pack_start(frame)
    vbox.pack_start(tweaker, gtk.FALSE)
    win.connect("destroy", gtk.mainquit)
    win.show_all()
    gtk.main()


if __name__ == "__main__":
    main()

### The End ###

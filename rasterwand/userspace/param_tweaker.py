#!/usr/bin/env python
#
# Interactive parameter tweaker for Raster Wand.
# This lets you adjust all timing parameters, and graphs
# the wand period in real-time. Requires rtgraph.
#
# -- Micah Dowty <micah@navi.cx>
#
import struct, os, fcntl
import gtk, Tweak, rtgraph


class Params:
    _fields = [
        'display_center',
        'display_width',
        'coil_center',
        'coil_width',
        'duty_cycle',
        'fine_adjust',
        'power_mode',
        'num_columns',
        ]

    _struct = "i" * 8

    def __init__(self, fd):
        self.__dict__['fd'] = fd
        self.get()

    def get(self):
        buffer = chr(0)*struct.calcsize(self._struct)
        f = struct.unpack(self._struct, fcntl.ioctl(self.fd, 0x3B01, buffer))
        self.__dict__.update(dict(zip(self._fields, f)))

    def put(self):
        f = [int(self.__dict__[n]) for n in self._fields]
        fcntl.ioctl(self.fd, 0x3B02, struct.pack(self._struct, *f))

    def __setattr__(self, key, value):
        self.__dict__[key] = value
        self.put()


class PeriodChannel(rtgraph.Channel):
    _struct = "HHBBBB"

    def __init__(self, fd):
        rtgraph.Channel.__init__(self)
        self.fd = fd

    def getValue(self):
        buffer = chr(0)*struct.calcsize(self._struct)
        f = struct.unpack(self._struct, fcntl.ioctl(self.fd, 0x3B03, buffer))
        return f[0] / 60000.0


def main():
    fd = os.open("/dev/usb/rwand0", os.O_RDWR)
    p = Params(fd)

    graph = rtgraph.HScrollLineGraph(channels=[
        PeriodChannel(fd),
        ])

    tweaker = Tweak.List([
        Tweak.Quantity(p, 'display_center', range=(0,0xFFFF)),
        Tweak.Quantity(p, 'display_width', range=(0,0xFFFF)),
        Tweak.Quantity(p, 'coil_center', range=(0,0xFFFF)),
        Tweak.Quantity(p, 'coil_width', range=(0,0xFFFF)),
        Tweak.Quantity(p, 'duty_cycle', range=(0,0xFFFF)),
        Tweak.Quantity(p, 'fine_adjust', range=(-800,800)),
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

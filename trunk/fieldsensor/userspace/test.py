#!/usr/bin/env python
import time, struct, fcntl
import gtk, rtgraph, Tweak


class FieldSensor:
    def __init__(self, devName="/dev/usb/efs0"):
        self.dev = open(devName)

    def readAverages(self):
        packet = struct.unpack("i" * 9, self.dev.read(4*9))
        if packet[8]:
            return [packet[i] / float(packet[8]) for i in xrange(8)]

    def setParam(self, block, param, value):
        fcntl.ioctl(self.dev, 0x3A01, struct.pack("iii", block, param, value))


class ParamTweaker(object):
    def __init__(self, efs, num):
        self.efs = efs
        self.num = num
        self._value = 0

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
        for i in xrange(8):
            channels[i].value = averages[i]
        gtk.timeout_add(10, poll_handler)
    gtk.timeout_add(10, poll_handler)

    graph = rtgraph.GraphUI(channels,
                            rtgraph.HScrollLineGraph(range=(0,255)),
                            valueUpdateInterval = 500)

    for block in xrange(8):
        efs.setParam(block, 0, block)    # EFS_PARAM_RESULT_NUM
        efs.setParam(block, 1, 0x03)     # EFS_PARAM_LC_PORT_XOR
        efs.setParam(block, 2, 0x81)     # EFS_PARAM_ADCON_INIT
        efs.setParam(block, 3, 0)        # EFS_PARAM_PERIOD
        efs.setParam(block, 4, 0)        # EFS_PARAM_PHASE
        efs.setParam(block, 5, 30)       # EFS_PARAM_NUM_HALF_PERIODS
        efs.setParam(block, 6, 0x00)     # EFS_PARAM_LC_TRIS_INIT
        efs.setParam(block, 7, 0x01)     # EFS_PARAM_LC_PORT_INIT

    tweaker = Tweak.List([
        Tweak.Quantity(ParamTweaker(efs, 3), 'value', range=(0,255), name="Period"),
        Tweak.Quantity(ParamTweaker(efs, 4), 'value', range=(0,255), name="Phase"),
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

#!/usr/bin/env python
import time, struct, gtk, rtgraph

efs = open("/dev/usb/efs0")
channels = [rtgraph.Channel(name="Parameter block %d" % i) for i in xrange(8)]

def read_averages(efs):
    packet = struct.unpack("i" * 9, efs.read(4*9))
    if packet[8]:
        return [packet[i] / float(packet[8]) for i in xrange(8)]

def poll_handler():
    averages = read_averages(efs)
    for i in xrange(8):
        channels[i].value = averages[i]
    gtk.timeout_add(10, poll_handler)

win = rtgraph.GraphUIWindow(channels,
                            rtgraph.HScrollLineGraph(range=(0,255)),
                            valueUpdateInterval = 500)
win.connect("destroy", gtk.mainquit)
gtk.timeout_add(10, poll_handler)
gtk.main()

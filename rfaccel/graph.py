#!/usr/bin/env python
import gtk, rtgraph, sys
import rfaccel

class PacketGrapher(rfaccel.PacketReceiver):
    def __init__(self, channels, device):
        self.channels = channels
        rfaccel.PacketReceiver.__init__(self, device)

    def receivedRawAccel(self, vect):
        self.channels[0].value = vect['x']
        self.channels[1].value = vect['y']
        self.channels[2].value = vect['z']
        self.channels[3].value = vect['neg_x']

def main():
    channels = [
        rtgraph.Channel(name="X"),
        rtgraph.Channel(name="Y"),
        rtgraph.Channel(name="Z"),
        rtgraph.Channel(name="-X"),
        ]

    graph = rtgraph.HScrollLineGraph(range=(0.0, 65535.0))
    win = rtgraph.GraphUIWindow(channels, graph, valueUpdateInterval=500)
    win.connect("destroy", gtk.main_quit)

    device = sys.argv[1]
    thread = rfaccel.ReceiverThread(PacketGrapher(channels, device))
    try:
        gtk.threads_init()
        gtk.main()
    finally:
        thread.running = False

if __name__ == "__main__":
    main()



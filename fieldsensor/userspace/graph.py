#!/usr/bin/env python
import gtk, rtgraph, sys, threading

class InputThread(threading.Thread):
    def run(self):
        self.running = True
        while self.running:
            values = sys.stdin.readline().strip().split(' ')

            for i, value in enumerate(values):
                self.channels[i].value = float(value)


def main():
    channels = [rtgraph.Channel (name="Parameter block %d" % i) for i in xrange (8)]

#    horizontal = rtgraph.Channel(name="Horizontal")
#    channels.append(horizontal)

    graph = rtgraph.HScrollLineGraph(range=(0.0, 255.0))
    win = rtgraph.GraphUIWindow(channels, graph, valueUpdateInterval=500)
    win.connect("destroy", gtk.mainquit)

    inputThread = InputThread()
    inputThread.channels = channels
    inputThread.start()
    try:
        gtk.threads_init()
        gtk.main()
    finally:
        inputThread.running = False

if __name__ == "__main__":
    main()

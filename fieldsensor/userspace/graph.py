#!/usr/bin/env python
import gtk, rtgraph, sys, threading

class InputThread(threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self)
        self.running = True

        self.inputs = [rtgraph.Channel (name="Parameter block %d" % i) for i in xrange (8)]
        self.horizontal = rtgraph.Channel(name="Horizontal")
        self.magnitude = rtgraph.Channel(name="Magnitude")

        self.channels = [self.horizontal, self.magnitude] + self.inputs

    def readInputs(self):
        values = sys.stdin.readline().strip().split(' ')
        for i, value in enumerate(values):
            self.inputs[i].value = float(value)

    def calculateFoo(self):
        self.horizontal.value = self.inputs[1].value - self.inputs[4].value
        self.magnitude.value = -sum([input.value for input in self.inputs]) / len(self.inputs)

    def run(self):
        while self.running:
            self.readInputs()
            self.calculateFoo()


def main():
    inputThread = InputThread()
    graph = rtgraph.HScrollLineGraph(range=(-1.0, 1.0))
    win = rtgraph.GraphUIWindow(inputThread.channels, graph, valueUpdateInterval=500)
    win.connect("destroy", gtk.mainquit)

    inputThread.start()
    try:
        gtk.threads_init()
        gtk.main()
    finally:
        inputThread.running = False

if __name__ == "__main__":
    main()

#!/usr/bin/env python
import gtk, rtgraph, sys, math
import rfaccel

calibration = (0.0028319861431870671, -94.271154734411084, 0.0025761554621848743, -78.892184873949589, 0.0025870253164556965, -79.845949367088622)

class PacketGrapher(rfaccel.PacketReceiver):
    def __init__(self, channels, device):
        self.channels = channels
        rfaccel.PacketReceiver.__init__(self, device)

    def receivedRawAccel(self, vect):
        accel = ( vect['x'] * calibration[0] + calibration[1],
                  vect['y'] * calibration[2] + calibration[3],
                  vect['z'] * calibration[4] + calibration[5] )
        mag = math.sqrt(accel[0]**2 + accel[1]**2 + accel[2]**2)

        self.channels[0].value = (accel[0] / 20.0,
                                  accel[1] / 20.0,
                                  accel[2] / 20.0)
        self.channels[1].value = (0,0, mag / 20.0)

def main():
    channels = [
        rtgraph.Channel(name="Vector"),
        rtgraph.Channel(name="Magnitude"),
        ]

    graph = rtgraph.IsometricVectorGraph()
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

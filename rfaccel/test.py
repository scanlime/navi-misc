#!/usr/bin/env python
import gtk, rtgraph, threading, struct

def sum(bytes):
    s = 0
    for byte in bytes:
        s = (s + ord(byte)) & 0xFF
    return s

def hexdump(bytes):
    return " ".join(["%02X" % ord(byte) for byte in bytes])

class ReceiverThread(threading.Thread):
    def __init__(self, channels):
        threading.Thread.__init__(self)
        self.channels = channels
        self.port = open("/dev/usb/tts/0", "rb")
        self.running = True
        self.start()

    def run(self):
        buffer = ''
        while self.running:
            # Keep a sliding buffer with the latest 11 bytes
            buffer = (buffer + self.port.read(1))[-11:]

            # Is this a valid packet?
            if buffer[0] != "U":
                continue
            if sum(buffer) != 0:
                continue
            self.receivedPacket(buffer)

    def receivedPacket(self, buffer):
        print hexdump(buffer)
        readings = struct.unpack(">HHHH", buffer[1:9])
        for i in xrange(len(readings)):
            self.channels[i].value = readings[i]

def main():
    channels = [
        rtgraph.Channel(name="Sensor 0"),
        rtgraph.Channel(name="Sensor 0"),
        rtgraph.Channel(name="Sensor 0"),
        rtgraph.Channel(name="Sensor 0"),
        ]

    graph = rtgraph.HScrollLineGraph(range=(0.0, 65535.0))
    win = rtgraph.GraphUIWindow(channels, graph, valueUpdateInterval=500)
    win.connect("destroy", gtk.main_quit)

    thread = ReceiverThread(channels)
    try:
        gtk.threads_init()
        gtk.main()
    finally:
        thread.running = False

if __name__ == "__main__":
    main()



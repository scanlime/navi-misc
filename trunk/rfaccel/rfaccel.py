#!/usr/bin/env python
import struct, threading

class PacketReceiver:
    def __init__(self, device):
        self.port = open(device, "rb")
        self.buffer = ""

    def waitForData(self):
        self.dataReceived(self.port.read(1))

    def dataReceived(self, data):
        for byte in data:
            # Keep a sliding buffer with the latest 11 bytes
            self.buffer = (self.buffer + byte)[-11:]

            # Is this a valid packet?
            if len(self.buffer) != 11:
                continue
            if self.buffer[0] != "U":
                continue
            if self.checksum(self.buffer) != 0:
                continue
            self.receivedPacket(self.buffer)

    def checksum(self, bytes):
        s = 0
        for byte in bytes:
            s = (s + ord(byte)) & 0xFF
        return s

    def receivedPacket(self, buffer):
        readings = struct.unpack(">HHHH", buffer[1:9])
        self.receivedRawAccel(dict(
            x = readings[0],
            z = readings[1],
            neg_x = readings[2],
            y = readings[3]))

    def receivedRawAccel(self, vect):
        pass


class ReceiverThread(threading.Thread):
    def __init__(self, receiver):
        threading.Thread.__init__(self)
        self.receiver = receiver
        self.running = True
        self.start()

    def run(self):
        while self.running:
            self.receiver.waitForData()

    def stop(self):
        self.running = False

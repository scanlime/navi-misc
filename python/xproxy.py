#!/usr/bin/env python
import socket, threading, time, struct


def connect():
    xserver = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    xserver.connect(("localhost", 6000))

    listener = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    for address in xrange(6001, 6010):
        try:
            listener.bind(("localhost", address))
        except:
            continue
        break
    listener.listen(1)
    print "Listening on :%d" % (address - 6000)
    xclient, client_address = listener.accept()

    print "Connected."
    return (xclient, xserver)


class ForwarderThread(threading.Thread):
    def __init__(self, fromSocket, toSocket, label=None):
        threading.Thread.__init__(self)
        self.fromSocket = fromSocket
        self.toSocket = toSocket
        self.label = label
        self.running = True

    def stop(self):
        self.running = False

    def hexdump(self, s):
        h = ""
        for byte in s:
            h += "%02X " % ord(byte)
        return h

    def show(self, packet):
        if self.label:
            print "%10s: %s" % (self.label, self.hexdump(packet))

    def filter(self, packet):
        return packet

    def run(self):
        while self.running:
            packet = self.fromSocket.recv(4096)
            if not packet:
                raise Exception("Disconnected")
            self.show(packet)
            self.toSocket.send(self.filter(packet))


class WidgeyFilter(ForwarderThread):
    def filter(self, packet):
        if ord(packet[0]) == 0x8F and ord(packet[1]) == 0x13:
            # xvShmPutImage
            fmt = "BBHIIIIIIHHHHHHHHHHBBH"
            putVideo = list(struct.unpack(fmt, packet[:52]))
            print putVideo
            putVideo[15] /= 2
            putVideo[16] /= 2
            packet = struct.pack(fmt, *putVideo) + packet[52:]
        return packet


def main():
    xclient, xserver = connect()
    try:
        WidgeyFilter(xclient, xserver, "request").start()
        ForwarderThread(xserver, xclient, "response").start()
        while 1:
            time.sleep(100)
    finally:
        for thread in threading.enumerate():
            if isinstance(thread, ForwarderThread):
                thread.stop()
                thread.join()

if __name__ == '__main__':
    main()

#!/usr/bin/env python

import os, random

class SerialBridge:
    def __init__(self, dev="/dev/ttyS0"):
        self.fd = os.open(dev, os.O_RDWR)

    def write(self, data, replyBytes=0):
        os.write(self.fd, chr(0x7E) + chr(len(data)) +
                 chr(replyBytes) + data)
        return os.read(self.fd, replyBytes)

    def refCRC(self, packet):
        return ord(self.write("\x03\x80\x01" +
                              ''.join([chr(i) for i in packet]), 1))

    def readTable(self):
        table = []
        for byte in xrange(32):
            for bit in xrange(8):
                packet = [0] * 32
                packet[byte] = 1<<(7-bit)
                table.append(self.refCRC(packet) ^ 0xFF)
        return table

def simCRC(packet, table):
    crc = 0xFF
    for byte in xrange(32):
        for bit in xrange(8):
            if packet[byte] & 1<<(7-bit):
                crc ^= table[byte*8 + bit]
    return crc

def bin(i, width=8):
    return ''.join([ "01"[i & (1<<(width-1-bit)) != 0]
                     for bit in xrange(width) ])

def showCRC(b, packet, table):
    binpacket = " ".join(["%02X" % byte for byte in packet])
    bincrc = bin(b.refCRC(packet))
    bincrcsim = bin(simCRC(packet, table))
    if bincrc == bincrcsim:
        status = "** Matched **"
    else:
        status = ""
    print "%s => %s (%s) %s" % (binpacket, bincrc, bincrcsim, status)


def main():
    b = SerialBridge()

    table = b.readTable()

    s = 0
    prev = 0xCD
    for entry in table:
        entry = (entry << s) | (entry >> (8-s)) & 0xFF
        diff = entry ^ prev
        #diff = (diff >> s) | (diff << (8-s)) & 0xFF
        prev = entry

        #print bin(diff)
        if diff & 0xFF:
            print "*",
        else:
            print " ",
        s = (s+1) & 7
        if s == 0:
            print

    for i in xrange(10):
        packet = [random.randint(0,255) for j in xrange(32)]
        showCRC(b, packet, table)

if __name__ == "__main__":
    main()

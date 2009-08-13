#!/usr/bin/env python
#
# Decode raw MMC commands from a binary dump of the CMD bitstream,
# stored in MSB-first order.
#
# -- Micah Dowty
#

import sys


class MMCDecoder:
    # Response length tables
    R1 = 6 + 32 + 7
    R2 = 6 + 127
    R3 = 6 + 32 + 7

    cmdLengths = {
        0: None,
        1: R3,
        2: R2,
        3: R1,
        6: R1,
        7: R1,
        8: R1,
        9: R2,
        10: R2,
        12: R1,
        13: R1,
        14: R1,
        15: None,
        19: R1,
        16: R1,
        17: R1,
        18: R1,
        23: R1,
        24: R1,
        25: R1,
        27: R1,
        28: R1,
        29: R1,
        30: R1,
        35: R1,
        36: R1,
        38: R1,
        42: R1,

        55: R1,  # ???
        }

    def __init__(self):
        self.state = self.s_idle
        self.t = None
        self.replyLen = None
        self.bitsLeft = None
        self.data = None

    def decodeFile(self, filename):
        self.decodeBytes(open(filename, 'rb').read())

    def decodeBytes(self, bytes):
        bits = range(8)
        bits.reverse()
        for byte in bytes:
            for bitN in bits:
                self.decodeBit((ord(byte) >> bitN) & 1)

    def decodeBit(self, bit):
        self.state(bit)

    def s_idle(self, bit):
        # Wait for start bit
        if not bit:
            self.state = self.s_tbit

    def s_tbit(self, bit):
        # Transmission bit
        self.t = bit
        self.state = self.s_databit
        self.data = 0

        if self.t:
            # Transmission
            self.bitsLeft = 6 + 32 + 7
        else:
            # Reply
            assert self.replyLen
            self.bitsLeft = self.replyLen

    def s_databit(self, bit):
        self.bitsLeft = self.bitsLeft - 1
        self.data = (self.data << 1) | bit
        if not self.bitsLeft:
            if self.t:
                self.command(self.data)
            else:
                self.reply(self.data)
            self.state = self.s_stopbit

    def s_stopbit(self, bit):
        if not bit:
            print "*** Stop bit missing!"
        self.state = self.s_idle

    def command(self, data):
        cmd = (data >> 39) & 0x3F
        arg = (data >> 7) & 0xFFFFFFFF
        crc7 = data & 0x7F
        self.replyLen = self.cmdLengths[cmd]

        sys.stdout.write("\n[CMD%2d] arg=%08x crc7=%02x" % (cmd, arg, crc7))

    def reply(self, data):
        if self.replyLen == self.R2:
            reply = "%032x crc7=%02x" % (self.data >> 7, self.data & 0x7F)
        elif self.replyLen == self.R1:
            reply = "%02x %08x crc7=%02x" % ((self.data >> 39) & 0x3F,
                                             (self.data >> 7) & 0xFFFFFFFF,
                                             self.data & 0x7F)
        else:
            assert False

        sys.stdout.write(" -- %s" % reply)


if __name__ == '__main__':
    MMCDecoder().decodeFile(sys.argv[1])

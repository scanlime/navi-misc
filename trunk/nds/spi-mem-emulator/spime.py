#!/usr/bin/env python
#
# spime.py - SPI Memory Emulator.
#
#   This is a client program for uploading/downloading/viewing memory on
#   the SPI EEPROM emulator board. Also usable as a Python module for
#   writing tools that directly interact with the emulator's buffer.
#
# Copyright (C) 2009 Micah Dowty
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#

import optparse
import serial
import struct
import re
import sys
import binascii
import cStringIO


class PacketPort:
    """Low-level serial port with packet framing semantics.
       This implements a 'byte stuffing' framing mechanism,
       based on RFC1622.

       This isn't actually as complicated as it looks..
       A lot of the string processing acrobatics here are
       so we can properly unescape received packets without
       any slow character-by-character loops in Python.
       """

    def __init__(self, device, baud=3000000):
        self.serial = serial.Serial(device, baud, timeout=1)
        self._rxBuffer = []
        self._escBuffer = ''

    def _escape(self, data):
        return data.replace('}', '}]').replace('~', '}^')

    def _unescape(self, data):
        chunks = data.split('}')
        for i in range(1, len(chunks)):
            chunks[i] = chr(0x20 ^ ord(chunks[i][0])) + chunks[i][1:]
        return ''.join(chunks)

    def write(self, packet):
        """Write one packet"""
        self.serial.write('~' + self._escape(packet))

    def _readBuffer(self, limit=None):
        """Examine the receive buffer, try to extract a packet from it.
           A packet is ready either if there is a single packet which
           meets our limit, or if there are multiple packets.

           If a packet is ready, returns it and removes it from the buffer.
           """
        if (len(self._rxBuffer) > 1 or
            (limit is not None and self._rxBuffer and len(self._rxBuffer[0]) >= limit)):

            packet = self._rxBuffer[0]
            del self._rxBuffer[0]
            return packet

    def _splitPackets(self, data):
        """Split some received data into packet fragments.
           Returns a list of strings which were separated by
           flags. Each string in the list will be unescaped.
           """

        # Apply any escapes we carried over from last time
        data = self._escBuffer + data
        self._escBuffer = ''

        # Double-escapes are a no-op
        data = re.sub('}+', '}', data)

        # Escapes prior to a flag are no-ops
        data = data.replace('}~', '~')

        # If there is a trailing escape, save it for later
        if data[-1] == '}':
            self._escBuffer = '}'
            data = data[:-1]

        # Now split on any flags, and unescape each chunk
        return map(self._unescape, data.split('~'))

    def read(self, limit=None):
        """Try to read one packet, with at most 'limit' bytes. If we
           time out without reading a packet, returns None.  If a
           packet is received successfully, returns the packet data
           (without the flag byte). If multiple packets are received,
           they are buffered for future read() calls.

           If the packet we receive is longer than 'limit', the rest
           of it is truncated.
           """
        while True:
            packet = self._readBuffer(limit)
            if packet:
                return packet

            # How much data have we already buffered?
            bufferedBytes = len(self._rxBuffer) and len(self._rxBuffer[0])

            # Guess how many bytes to read at a time.
            blockSize = max(1, (limit or 0) - bufferedBytes)

            block = self.serial.read(blockSize)
            if not block:
                # Timed out
                return None

            # Split on all flag bytes, unescape each chunk.
            chunks = self._splitPackets(block)

            # First chunk didn't start with a flag. Add it to the most
            # recent packet, if any
            if self._rxBuffer:
                self._rxBuffer[-1] += chunks[0]

            # Other chunks begin new packets
            self._rxBuffer.extend(chunks[1:])


def hexDump(src, dest, bytesPerLine=16, wordSize=4):
    """A pretty standard hex dumper routine.
       Dumps the stream 'src' to the stream 'dest'
       """
    addr = 0
    while 1:
        srcLine = src.read(bytesPerLine)
        if not srcLine:
            break

        # Address
        dest.write("%04X: " % addr)
        addr += len(srcLine)

        # Hex values
        for i in xrange(bytesPerLine):
            if i < len(srcLine):
                dest.write("%02X" % ord(srcLine[i]))
            else:
                dest.write("  ")
            if not (i+1) % wordSize:
                dest.write(" ")
        dest.write(" ")

        # ASCII representation
        for byte in srcLine:
            if ord(byte) >= 32 and ord(byte) < 128:
                dest.write(byte)
            else:
                dest.write(".")
        for i in xrange(bytesPerLine - len(srcLine)):
            dest.write(" ")
        dest.write("\n")



if __name__ == "__main__":
    p = PacketPort("/dev/ttyUSB0")

    p.write(struct.pack(">I", 0x000001)[1:4])
    hexDump(cStringIO.StringIO(p.read(3)), sys.stdout)

    p.write(struct.pack(">I", 0x400000)[1:4])
    hexDump(cStringIO.StringIO(p.read(3+0x10000)), sys.stdout)



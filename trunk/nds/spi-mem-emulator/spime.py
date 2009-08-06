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
        self.serial = serial.Serial(device, baud, timeout=2)
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


class ProtocolError(Exception):
    pass


class Packet:
    """Abstract base class for one protocol packet that the emulator understands.
       """
    _type = None
    _replyDataLen = 0

    def __init__(self, address=0):
        self.address = address & 0xFFFFF

    def __repr__(self):
        return "%s<0x%05x>" % (self.__class__.__name__, self.address)

    def _getHeader(self):
        return struct.pack(">I", self._type | self.address)[1:4]

    def send(self, port, data=''):
        """Send this packet on the provided port, optionally with some payload
           data, and wait for the response. Returns the response data, if any.
           If there is no response or the header is corrupted, raises a ProtocolError.
           """
        header = self._getHeader()
        port.write(header + data)

        reply = port.read(len(header) + self._replyDataLen)
        if not reply:
            raise ProtocolError("Timed out while sending %r" % self)
        if reply[:len(header)] != header:
            raise ProtocolError("Incorrect response header for %r" % self)

        replyData = reply[len(header):]
        if len(replyData) != self._replyDataLen:
            raise ProtocolError("Incorrect response length for %r" % self)

        return replyData


class Ping(Packet):
    """Ping packet. Tests our connection with the emulator, does nothing else."""
    _type = 0x000000

class Write(Packet):
    """Write a variable-length block of data into memory."""
    _type = 0x100000

class Read16(Packet):
    """Read 16 bytes"""
    _type = 0x200000
    _replyDataLen = 16

class Read1K(Packet):
    """Read 1 kilobyte"""
    _type = 0x300000
    _replyDataLen = 0x400

class Read64K(Packet):
    """Read 64 kilobytes"""
    _type = 0x400000
    _replyDataLen = 0x10000


def memRead(port, offset, length):
    """Read device memory. Automatically selects the proper
       packet length(s) to use.
       """
    data = []
    while length > 0:

        if length > 1024:
            block = Read64K(offset).send(port)
        elif length > 16:
            block = Read1K(offset).send(port)
        else:
            block = Read16(offset).send(port)

        block = block[:length]
        data.append(block)
        length -= len(block)
        offset += len(block)

    return ''.join(data)

def memWrite(port, offset, data):
    """Write device memory.
       This always sends a single kind of packet,
       but it's here for consistency with memRead.
       """
    Write(offset).send(port, data)


def hexDump(src, dest=sys.stdout, bytesPerLine=16, wordSize=4, addr=0):
    """A pretty standard hex dumper routine.
       Dumps the stream or string 'src' to the stream 'dest'
       """
    if isinstance(src, str):
        src = cStringIO.StringIO(src)
    while 1:
        srcLine = src.read(bytesPerLine)
        if not srcLine:
            break

        # Address
        dest.write("%05X: " % addr)
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
            if ord(byte) >= 32 and ord(byte) < 127:
                dest.write(byte)
            else:
                dest.write(".")
        for i in xrange(bytesPerLine - len(srcLine)):
            dest.write(" ")
        dest.write("\n")


def action_ping(port):
    Ping().send(port)
    sys.stderr.write("Connected to emulator.\n")

def action_dump(port, address, count):
    hexDump(memRead(port, address, count), addr=address)

def action_save(port, address, count, file):
    open(file, 'wb').write(memRead(port, address, count))
    sys.stderr.write("Saved %d bytes at 0x%05x to %r\n" % (count, address, file))

def action_load(port, address, file):
    data = open(file, 'rb').read()
    memWrite(port, address, data)
    sys.stderr.write("Loaded %d bytes at 0x%05x from %r\n" % (len(data), address, file))

def action_diff(port, address, count):
    sys.stderr.write("Polling %d bytes for differences, at 0x%05x ...\n"
                     % (count, address))
    prev = memRead(port, address, count)
    while True:
        next = memRead(port, address, count)
        if next != prev:
            for i, (a, b) in enumerate(zip(prev, next)):
                if a != b:
                    sys.stdout.write("%05x: %02x -> %02x\n"
                                     % (address + i, ord(a), ord(b)))
        prev = next

def action_clear(port):
    memWrite(port, 0, '\xFF' * 0x100000)
    sys.stderr.write("Memory cleared.\n")


def main():
    parser = optparse.OptionParser();
    parser.add_option("-p", "--port", dest="port",
                      help="Serial port name", metavar="PORT", default="/dev/ttyUSB0")
    parser.add_option("-c", "--count", dest="count", type="int", default="8192",
                      help="Number of bytes to operate on")
    parser.add_option("-a", "--address", dest="address", type="int", default="0",
                      help="Memory address to start at")

    parser.add_option("-d", "--dump", dest="dump", action="store_true",
                      help="Action: Hex dump memory")
    parser.add_option("-s", "--save", dest="save", metavar="FILE",
                      help="Action: Save memory to FILE")
    parser.add_option("-l", "--load", dest="load", metavar="FILE",
                      help="Action: Load memory from FILE")
    parser.add_option("--diff", dest="diff", action="store_true",
                      help="Action: Poll memory for changes, print differences")
    parser.add_option("--clear", dest="clear", action="store_true",
                      help="Action: Clear memory to 0xFF")

    (options, args) = parser.parse_args()

    port = PacketPort(options.port)
    action_ping(port)

    if options.dump:
        action_dump(port, options.address, options.count)

    if options.save:
        action_save(port, options.address, options.count, options.save)

    if options.load:
        action_load(port, options.address, options.load)

    if options.diff:
        action_diff(port, options.address, options.count)

    if options.clear:
        action_clear(port)

if __name__ == "__main__":
    main()

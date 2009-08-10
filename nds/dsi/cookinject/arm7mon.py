#!/usr/bin/env python
#
# arm7mon.py - ARM7 debug monitor, implemented using cookinject
#              and the SPI memory emulator.
#
#              To use this interactively, run Python and "from arm7mon import *".
#              Several aliases are provided so that commands are quick to type.
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

import spime
import struct
import random

port = spime.PacketPort("/dev/ttyUSB1")
spime.action_ping(port)

def command(cmd, arg0=0, arg1=0, arg2=0):
    spime.memWrite(port, 4, struct.pack("<III", arg0, arg1, arg2))
    spime.memWrite(port, 0, cmd)
    while spime.memRead(port, 0, 1) != '\0':
        pass

def readBytes(addr, size=0x40):
    data = []
    while size > 0:
        chunkSize = min(size, 0xF0000)
        command('R', 0x10, addr, chunkSize)
        data.append(spime.memRead(port, 0x10, chunkSize))
        addr += chunkSize
        size -= chunkSize
    return ''.join(data)

def writeBytes(addr, data):
    while data:
        chunkSize = min(len(data), 0xF0000)
        spime.memWrite(port, 0x10, data[:chunkSize])
        command('W', 0x10, addr, chunkSize)
        addr += chunkSize
        data = data[chunkSize:]

def read16(addr, size=0x40):
    command('r', 0x10, addr, size)
    return spime.memRead(port, 0x10, size)

def write16(addr, word):
    command('H', addr, word)

def write32(addr, word):
    command('L', addr, word)

def cont():
    command('C')

def fuzz(lowAddr, highAddr):
    while True:
        addr = random.randint(lowAddr, highAddr)
        width = random.choice((1, 2, 4))
        if width == 4:
            data = random.randint(0, 0xFFFFFFFF)
            print "%08x <- %08x" % (addr, data)
            write32(addr, data)
        elif width == 2:
            data = random.randint(0, 0xFFFF)
            print "%08x <- %04x" % (addr, data)
            write16(addr, data)
        else:
            data = random.randint(0, 0xFF)
            print "%08x <- %02x" % (addr, data)
            writeBytes(addr, chr(data))

def dump(addr, size):
    filename = "%08x.bin" % addr
    open(filename, 'wb').write(readBytes(addr, size))
    print "Dumped 0x%x bytes to %r" % (size, filename)


class MemRanges:
    """Represents a sparse list of memory ranges"""
    def __init__(self, bitmap=None):
        self._list = []
        if bitmap:
            self.fromPageBitmap(bitmap)

    def fromPageBitmap(self, bitmap):
        numPages = len(bitmap) * 8
        for page in xrange(0, numPages):
            if ord(bitmap[page >> 3]) & (1 << (page & 7)):
                self.add(page << 12, 0x1000)

    def add(self, addr, length):
        self._list.append([addr, length])

    def reduce(self):
        """Simplify the list of ranges
           """
        self._list.sort()
        newList = []
        bottom = 0
        top = 0
        for addr, length in self._list:
            end = addr + length
            if addr <= top:
                # Overlapping previous region. Combine.
                top = end
            else:
                # Not overlapping. Flush
                newList.append([bottom, top-bottom])
                bottom = addr
                top = end
        if top != bottom:
            # Flush last region
            newList.append([bottom, top-bottom])
        self._list = newList

    def iterRanges(self):
        self.reduce()
        return iter(self._list)

    def iterHoles(self):
        self.reduce()
        top = 0
        for addr, length in self._list:
            if addr != top:
                yield top, addr - top
            top = addr + length

    def fillHoles(self, sizeLimit):
        for addr, length in list(self.iterHoles()):
            if length < sizeLimit:
                self.add(addr, length)


def dumpAll(numPages=0x10000, minHoleSize=0x10000):
    """Dump all nonzero regions of memory to disk, putting
       each contiguous region (with not more than minHoleSize
       zeroes) in a separate file.

       By default we only scan up to 0x10000000, since (and I verified
       this once on the DSi) the upper four address bits are
       unused. All memory above 0x10000000 is zero.
       """
    print "Scanning memory..."

    # Ask the ARM7 hook to scan for groups of nonzero pages.
    command('B', 0x10, numPages)
    bitmap = spime.memRead(port, 0x10, numPages / 8)
    r = MemRanges(bitmap)
    r.fillHoles(minHoleSize)

    ranges = list(r.iterRanges())
    print "Found %d ranges:" % len(ranges)
    for addr, length in ranges:
        print "\t0x%08x - 0x%08x  (%d bytes)" % (addr, addr + length - 1, length)

    for addr, length in ranges:
        print "Dumping 0x%08x ..." % addr
        dump(addr, length)


hd = spime.hexDump
hdl = lambda s: hd(s, wordLE=True)
rb = readBytes
rh = read16
wb = writeBytes
wh = write16
wl = write32

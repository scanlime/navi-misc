""" rcpod.i2c.generic

Interfaces to generic I2C devices, those available from multiple
manufacturers. The most common example by far is the I2C EEPROM,
available in many sizes and form factors from many manufacturers.
"""
#
# Remote Controlled PIC of Doom
# Copyright (C) 2003-2004 Micah Dowty <micahjd@users.sourceforge.net>
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

from __future__ import division
import struct
from pyrcpod import i2c


class Memory(i2c.Device):
    """An I2C memory device. This will work out of the box
       on 24XX memories, and can be trivially subclassed
       for other types of memory.
       """

    # Base address on the I2C bus
    busAddressBase = 0x50

    # The 'struct' format string describing the addressing format
    # used by this device. Normal 24XX-series EEPROMs use 16-bit big endian
    # addresses.
    memAddressFormat = ">H"

    # The page size for writes- this is the maximum size of a single write
    # operation, and the alignment used for writes larger than one byte.
    # On normal EEPROM memories this must be 64 bytes. On devices that don't
    # need pages, this can be the size of the entire memory.
    # Currently, this must be a power of two!
    pageSize = 64

    # Maximum number of bytes read at once- 8 byte reads are more efficient
    # for the RCPOD to perform than any other size, due to its protocol.
    readBlockSize = 8

    def memRead(self, address, count, progressCallback=None):
        """Read a string of 'count' bytes starting at the given address.
           If a progressCallback is provided, it's called after each block
           read with the progress as a number between 0 and 1.
           """
        buffer = []
        remaining = count

        # We only have to write the address once to perform sequential reads
        self.busWrite(struct.pack(self.memAddressFormat, address))

        while remaining > 0:
            blockSize = min(remaining, self.readBlockSize)
            buffer.append(self.busRead(blockSize, str))
            remaining -= blockSize

            if progressCallback:
                progressCallback((count - remaining) / count)

        return ''.join(buffer)

    def memWrite(self, address, s, timeout=0.5, progressCallback=None):
        """Write a string to the given address
           If a progressCallback is provided, it's called after each block
           read with the progress as a number between 0 and 1.
           """
        originalLength = len(s)

        while s:
            # Figure out what page we're on, so we can
            # stop before we wrap around at its end
            pageStart = address & ~(self.pageSize-1)
            maxLength = self.pageSize - (address - pageStart)
            block = s[:maxLength]
            s = s[maxLength:]
            packedAddr = struct.pack(self.memAddressFormat, address)
            address += len(block)
            self.busWrite(packedAddr + block)
            self.pollUntilReady(timeout)

            if progressCallback:
                progressCallback((originalLength - len(s)) / originalLength)

### The End ###

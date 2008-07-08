"""
ILDA.py

Python module for dealing with the ILDA Image Data Transfer Format,
an interchange format for laser image frames.

Copyright (c) 2008 Micah Dowty

   Permission is hereby granted, free of charge, to any person
   obtaining a copy of this software and associated documentation
   files (the "Software"), to deal in the Software without
   restriction, including without limitation the rights to use, copy,
   modify, merge, publish, distribute, sublicense, and/or sell copies
   of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
   BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
   ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.

"""

from __future__ import division

import struct

# Format codes
FORMAT_3D = 0
FORMAT_2D = 1
FORMAT_COLOR_TABLE = 2

# Mapping from FORMAT_* codes to struct format strings
formatTable = (
    '>hhhH',
    '>hhH',
    '>BBB',
    )

# Header values
HEADER_MAGIC    = "ILDA\0\0\0"
HEADER_RESERVED = 0
HEADER_FORMAT   = ">7sB16sHHHBB"
HEADER_LEN      = struct.calcsize(HEADER_FORMAT)


class Table(object):
    """Container object for one ILDA table: either a frame (table of points)
       or a palette (table of colors).

       The 'items' list contains the data within this table. Each item
       is a tuple, corresponding to the raw values within that row of the
       table.

        2D frame: (x, y, status)
        3D frame: (x, y, z, status)
        Color:    (r, g, b)

       """
    def __init__(self, format=FORMAT_2D, name="",
                 length=0, number=0, total=0, scanHead=0):
        self.__dict__.update(locals())
        self.items = []

    def __repr__(self):
        return ("<ILDA.Table format=%d name=%r "
                "length=%d number=%d total=%d scanHead=%d>" %
                (self.format, self.name, self.length, self.number,
                 self.total, self.scanHead))

    def unpackHeader(self, data):
        magic, self.format, self.name, self.length, \
            self.number, self.total, self.scanHead, \
            reserved = struct.unpack(HEADER_FORMAT, data)

        if magic != HEADER_MAGIC:
            raise ValueError("Bad ILDA header magic. Not an ILDA file?")
        if reserved != HEADER_RESERVED:
            raise ValueError("Reserved ILDA field is not zero.")

    def packHeader(self):
        return struct.pack(HEADER_FORMAT, HEADER_MAGIC, self.format,
                           self.name, self.length, self.number,
                           self.total, self.scanHead, HEADER_RESERVED)

    def readHeader(self, stream):
        self.unpackHeader(stream.read(HEADER_LEN))
    
    def writeHeader(self, stream):
        stream.write(self.packHeader())

    def _getItemFormat(self):
        try:
            return formatTable[self.format]
        except IndexError:
            raise ValueError("Unsupported format code")

    def read(self, stream):
        """Read the header, then read all items in this table."""
        self.readHeader(stream)
        if self.length:
            fmt = self._getItemFormat()
            itemSize = struct.calcsize(fmt)
            self.items = [struct.unpack(fmt, stream.read(itemSize))
                          for i in xrange(self.length)]

    def write(self, stream):
        """Write the header, then write all items in this table."""
        self.writeHeader(stream)
        if self.length:
            fmt = self._getItemFormat()
            itemSize = struct.calcsize(fmt)
            stream.write(''.join([struct.pack(fmt, *item)
                                  for item in self.items]))

    def iterPoints(self):
        """Iterate over Point instances for each item in this table.
           Only makes sense if this is a 2D or 3D point table.
           """
        for item in self.items:
            p = Point()
            p.decode(item)
            yield p


class Point:
    """Abstraction for one vector point. The Table object, for
       completeness and efficiency, stores raw tuples for each
       point. This is a higher level interface that decodes the status
       bits and represents coordinates in floating point.
       """
    def __init__(self, x=0.0, y=0.0, z=0.0, color=0, blanking=False):
        self.__dict__.update(locals())

    def __repr__(self):
        return "<ILDA.Point (%s, %s, %s) color=%s blanking=%s>" % (
            self.x, self.y, self.z, self.color, self.blanking)

    def encode(self):
        status = self.color & 0xFF
        if self.blanking:
            status |= 1 << 14

        return (min(0x7FFF, max(-0x7FFF, self.x * 0x7FFF)),
                min(0x7FFF, max(-0x7FFF, self.y * 0x7FFF)),
                min(0x7FFF, max(-0x7FFF, self.z * 0x7FFF)),
                status)

    def decode(self, t):
        self.x = t[0] / 0x7FFF
        self.y = t[1] / 0x7FFF
        if len(t) > 3:
            self.z = t[2] / 0x7FFF
        else:
            self.z = 0.0

        self.color = t[3] & 0xFF
        self.blanking = (t[3] & (1 << 14)) != 0


def read(stream):
    """Read ILDA data from a stream until we hit the
       end-of-stream marker. Yields a sequence of Table objects.
       """
    while True:
        t = Table()
        t.read(stream)
        if not t.length:
            # End-of-stream
            break
        yield t


def write(stream, tables):
    """Write a sequence of tables in ILDA format,
       terminated by an end-of-stream marker.
       """
    for t in tables:
        t.write(stream)
    Table().write(stream)


def readFrames(stream):
    """Read ILDA data from a stream, and ignore
       all non-frame tables. Yields only 2D or 3D
       point tables.
       """
    for t in read(stream):
        if t.format in (FORMAT_2D, FORMAT_3D):
            yield t


def readFirstFrame(stream):
    """Read only a single frame from an ILDA stream."""
    for frame in readFrames(stream):
        return frame


if __name__ == "__main__":
    # Test program- dump frames and points from a file whose name is
    # specified on the command line

    import sys

    f = open(sys.argv[1], 'rb')
    for t in readFrames(f):
        for p in t.iterPoints():
            print "\t%r" % p

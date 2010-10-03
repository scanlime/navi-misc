#!/usr/bin/env python
"""
This is a simple and self-contained Python script for converting
images from PNG to the format used by Nokia6100.spin's image() method.

Usage:
  Nokia6100_Convert.py input.png [output.bin]

-- Micah Elizabeth Scott <micah@navi.cx>
"""

import sys, os, png


def convert(input, output=None):
    if not output:
        output = os.path.splitext(input)[0] + '.bin'

    print "Converting '%s' to '%s'" % (input, output)

    reader = png.Reader(filename=input)
    writer = LCDWriter(output)

    width, height, data, meta = reader.asRGBA8()

    # Bottom to top
    data = list(data)
    data.reverse()

    for row in data:
        # Right to left. Also swaps channel order.
        row.reverse()
        row = iter(row)

        for i in xrange(width):
            a, b, g, r = row.next(), row.next(), row.next(), row.next()
            writer.writeChannel(r)
            writer.writeChannel(g)
            writer.writeChannel(b)

    writer.finish()
    print "Converted %d x %d image (%d bytes)" % (width, height, writer.byteCount)


class LCDWriter:
    buffer = None
    byteCount = 0

    def __init__(self, filename):
        self.file = open(filename, "wb")

    def writeChannel(self, channel):
        nybble = int( (channel * 15 / 255.0) + 0.5 ) 

        if self.buffer is None:
            self.buffer = nybble
        else:
            self.file.write(chr((self.buffer << 4) | nybble))
            self.byteCount += 1
            self.buffer = None

    def finish(self):
        if self.buffer is not None:
            self.writeChannel(0)


if __name__ == "__main__":
    if len(sys.argv) in (2, 3):
        convert(*(sys.argv + [None])[1:3])
    else:
        sys.stderr.write(__doc__)

#!/usr/bin/env python
#
# A simple Python script to compress data
# with zlib's DEFLATE algorithm at build time.
#

import zlib, sys

level = 9

input = sys.stdin.read()
zData = zlib.compress(input, level)

# Strip off the zlib header, and return the raw DEFLATE data stream.
# See the zlib RFC: http://www.gzip.org/zlib/rfc-zlib.html

cmf = ord(zData[0])
flg = ord(zData[1])
assert (cmf & 0x0F) == 8   # DEFLATE algorithm
assert (flg & 0x20) == 0   # No preset dictionary

# Strip off 2-byte header and 4-byte checksum
rawData = zData[2:len(zData)-4]

sys.stdout.write(rawData)

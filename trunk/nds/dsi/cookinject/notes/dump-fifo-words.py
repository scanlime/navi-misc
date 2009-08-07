#!/usr/bin/env python

import sys
import struct

f = open(sys.argv[1], 'rb').read()
for i, w in enumerate(struct.unpack("<%dI" % (len(f) / 4), f)):
	if w != 0xc0000006:
		print "%04x: 0x%08x" % (i,w)


#!/usr/bin/env python
#
# Prints a hex dump of the PIC's entire address space
#

import rcpod, StringIO, sys


def hexDump(src, dest, bytesPerLine=16, wordSize=2):
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


pic = rcpod.Device()

# Dump the address space to a StringIO
str = ''
for address in xrange(0x200):
    str += chr(pic.peek(address))
sio = StringIO.StringIO(str)

# Get hexdump to do its thing
hexDump(sio, sys.stdout, wordSize=1)

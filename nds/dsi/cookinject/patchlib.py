#
# A simple Python library for writing binary patcher scripts.
# An example patch script:
#
#   from patchlib import *
#   data = BinaryPatcher().load()
#
#   data[0x513] = 52
#
#   character = data.offset(0x18e0)
#   character[0x10] = unhex("00 05 1a")
#
# -- Micah Dowty <micah@navi.cx>
# 

import optparse, sys, binascii, atexit


class Patchable:
    """A wrapper around some binary data that we'd like to patch.

       - patchable[address] = 42
         Writes the byte value '42' to 'address

       - patchable[address] = 'foo'
         Write the raw binary string 'foo' to address through address+2.

       This can be combined with tools like struct.pack and 'unhex' to
       generate useful binary strings.
       """

    def __init__(self, array, offset=0):
        self._array = array
        self._offset = offset

    def __len__(self):
        return len(self._array)

    def offset(self, offset):
        """Create a second Patchable that references a portion of this
           Patchable, starting at a particular offset. Modifications to
           the new object will affect this one.
           """
        return Patchable(self._array, self._offset + offset)

    def __getitem__(self, x):
        return self._array[x + self._offset]

    def __setitem__(self, x, value):
        if type(value) in (int, long):
            if value < 0 or value > 255:
                raise ValueError("Byte value out of range: %d" % value)
            self._array[x + self._offset] = value

        elif type(value) is str:
            self._array[x + self._offset:
                        x + self._offset + len(value)] = map(ord, value)

        else:
            raise TypeError("Unsupported type for Patchable: %r" % value)

    def __str__(self):
        return ''.join(map(chr, self._array[self._offset:]))


def unhex(str):
    """Simple wrapper to remove whitespace from a string and convert hex to binary."""
    return binascii.a2b_hex(str.replace(" ", ""))


class BinaryPatcher:
    def __init__(self):
        self.parser = optparse.OptionParser()
        self.parser.usage = "%prog [options] original-file patched-file"

    def load(self):
        values, args = self.parser.parse_args()
        if len(args) != 2:
            self.parser.print_usage()
            sys.exit(1)

        self.inFile, self.outFile = args

        self.patchable = Patchable(map(ord, open(self.inFile).read()))
        atexit.register(self.save)
        return self.patchable

    def save(self):
        open(self.outFile, "wb").write(str(self.patchable))

""" mi6k

Python module for interacting with the Media Infrawidget 6000
"""

from fcntl import ioctl
import struct
from Numeric import *


class CenturyVFD:
    width = 20
    lines = 2
    userDefinedCharacters = "\xF6\xF7\xF8\xF9\xFA\xFB\xFC\xFD\xFE\xFF"

    def __init__(self, dev):
        self.dev = dev

    def powerOn(self):
        ioctl(self.dev, 0x3600, 1)

    def powerOff(self):
        ioctl(self.dev, 0x3600, 0)

    def write(self, data):
        "Write a string, handling newlines"
        lines = data.split('\n')
        lastLine = lines[-1]
        for l in lines[:-1]:
            self.writeVFD(l)
            self.nl()
        self.writeVFD(lastLine)

    def writeVFD(self, data):
        self.dev.write(data)

    def cr(self):
        self.dev.write("\r")

    def lf(self):
        self.dev.write("\n")

    def nl(self):
        self.cr()
        self.lf()

    def clear(self):
        self.writeVFD(chr(0x15))

    def home(self):
        self.writeVFD(chr(0x16))

    def cursorOff(self):
        self.writeVFD(chr(0x0E))

    def writeScreen(self, data):
        """Write a complete screen of data, without clearing the screen"""
        lines = data.split('\n')
        while len(lines)<self.lines:
            lines.append("")
        self.home()
        self.cursorOff()
        for l in lines:
            if len(l) < self.width:
                l += " " * (self.width - len(l))
            elif len(l) > self.width:
                l = l[:self.width]
            self.writeVFD(l)
        self.dev.flush()

    def writeArray(self, data):
        """write a complete screen of data from a Numeric array"""
        data = asarray(data).astype(UInt8)
        self.home()
        self.cursorOff()
        self.writeVFD(data.tostring())
        self.dev.flush()

    def defineCharacter(self, char, data):
        """Set a user defined character from a 5x7 Numeric array"""

        # The VFD has a strange dot ordering, we swizzle our original
        # bitmap into an array ordered first by bit position then by byte
        bits = take(data.flat, ( (17, 13,  9,  5,  1),
                                 ( 0, 31, 27, 23, 19),
                                 (16, 12,  8,  4,  0),
                                 (34, 30, 26, 22, 18),
                                 (15, 11,  7,  3,  0),
                                 (33, 29, 25, 21,  0),
                                 (14, 10,  6,  2,  0),
                                 (32, 28, 24, 20,  0) ))

        # Pack this resulting array into bytes
        bytes = bits[0] + bits[1]*2 + bits[2]*4 + bits[3]*8 + bits[4]*16 + bits[5]*32 + bits[6]*64 + bits[7]*128

        # Convert to a command string and send it
        self.writeVFD(chr(0x18) + char + bytes.astype(UInt8).tostring())

    def setBrightness(self, l, column=0xFF):
        """Set the brightness of one column, or by default the entire display. l should be in the range [0,1]"""
        self.writeVFD("\x19\x30" + chr(column) + chr(int((1-l) * 0x07)))

    def flush(self):
        self.dev.flush()


class Lights(object):
    """Abstraction for the mi6k's front panel lights. They can both be set in one operation
       using the set() member, or they can be transparently modified using the magic of properties.
       All brightnesses are normalized to fit the range [0,1]
       """
    def __init__(self, dev):
        self.dev = dev
        self._white = 0
        self._blue = 0

    def set(self, white, blue):
        ioctl(self.dev, 0x3601, struct.pack("HH",
                                            min(1, max(0, white)) * 0xFFFF,
                                            min(1, max(0, blue )) * 0xFFFF))

    def setWhite(self, white):
        self.set(white, self._blue)
        self._white = white
        self.dev.flush()

    def setBlue(self, blue):
        self.set(self._white, blue)
        self._blue = blue
        self.dev.flush()

    def getWhite(self):
        return self._white

    def getBlue(self):
        return self._blue

    blue = property(getBlue, setBlue)
    white = property(getWhite, setWhite)


class MI6K:
    def __init__(self, device="/dev/usb/mi6k0"):
        self.dev = open(device, "w")
        self.vfd = CenturyVFD(self.dev)
        self.lights = Lights(self.dev)

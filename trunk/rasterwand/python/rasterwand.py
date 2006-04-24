""" rasterwand

Pure Python interface to the Raster Wand kernel module. This includes
simple text and animation support using PIL.
"""
#
# Raster Wand
# Copyright (C) 2004 Micah Dowty <micah@navi.cx>
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
import struct, os, fcntl, time


powerModes = {
    'switch': 0,
    'off': 1,
    'on': 2,
    'auto': 3,
    }


class _ioctlStruct:
    """A structure that is read and written via ioctl()s"""
    def __init__(self, fd):
        self.__dict__['fd'] = fd
        self.get()

    def get(self):
        buffer = chr(0)*struct.calcsize(self._struct)
        f = struct.unpack(self._struct, fcntl.ioctl(self.fd, self._read_ioctl, buffer))
        self.__dict__.update(dict(zip(self._fields, f)))

    def put(self):
        f = [int(self.__dict__[n]) for n in self._fields]
        fcntl.ioctl(self.fd, self._write_ioctl, struct.pack(self._struct, *f))

    def __setattr__(self, key, value):
        self.__dict__[key] = value
        self.put()


class Params(_ioctlStruct):
    """Encapsulates the rwand driver's tweakable parameters. Writing
       to the corresponding python attribute updates the driver's parameter.
       """
    _fields = [
        'display_center',
        'display_width',
        'coil_center',
        'coil_width',
        'duty_cycle',
        'fine_adjust',
        'power_mode',
        'num_columns',
        ]
    _struct = "i" * 8
    _read_ioctl  = 0x3B01
    _write_ioctl = 0x3B02


class Startup(_ioctlStruct):
    """Encapsulates parameters that control the wand's startup state"""
    _fields = [
        'min_period',
        'max_period',
        'climb_rate',
        ]
    _struct = "i" * 3
    _read_ioctl  = 0x3B04
    _write_ioctl = 0x3B05


class Status:
    """A read-only representation of the device's status"""
    _fields = {
        'period': 0,
        'phase': 1,
        'edge_count': 2,
        'mode': 3,
        'flip_count': 4,
        'buttons': 5,
        }

    _struct = "HHBBBB"

    def __init__(self, fd):
        self.fd = fd

    def __getattr__(self, key):
        buffer = chr(0)*struct.calcsize(self._struct)
        f = struct.unpack(self._struct, fcntl.ioctl(self.fd, 0x3B03, buffer))
        return f[self._fields[key]]


class Device:
    """The Raster Wand device. Contains the parameters, and provides
       methods for generating frames to display.
       """
    def __init__(self, name="/dev/rwand0"):
        self.fd = os.open(name, os.O_RDWR)
        self.params = Params(self.fd)
        self.status = Status(self.fd)
        self.startup = Startup(self.fd)

    def writeRaw(self, frame):
        """Write a raw framebuffer to the device"""
        os.write(self.fd, frame)

    def writeImage(self, img):
        """Write a PIL image to the device. Only up to 80x8 pixels will be extracted from it."""
        width = min(80, img.size[0])
        img = img.crop((0,0,width,8)).convert("1")
        bytes = [0]*width
        bit = 1
        x = 0
        for pixel in img.getdata():
            if pixel:
                bytes[x] |= bit
            x += 1
            if x == width:
                x = 0
                bit <<= 1
        self.writeRaw("".join([chr(b) for b in bytes]))

    def showText(self, text):
        TextScroller(self, text).show()

    def hScrollText(self, text, speed=0.5, pause=1, width=40):
        s = TextScroller(self, text, width=width)
        s.scroll(start=(-1, 0), speed=speed)
        time.sleep(pause)
        s.scroll(end=(1, 0), speed=speed)

    def vScrollText(self, text, speed=0.5, pause=1, width=40):
        s = TextScroller(self, text, width=width)
        s.scroll(start=(0, 1), speed=speed)
        time.sleep(pause)
        s.scroll(end=(0, -1), speed=speed)


def sgn(x):
    if x > 0:
        return 1
    elif x < 0:
        return -1
    else:
        return 0


class TextScroller:
    def __init__(self, dev, text, font=None, fontFile="data/helvetica8.pil", width=40):
        import ImageDraw, Image, ImageFont

        if font is None:
            font = ImageFont.load(fontFile)

        self.font = font
        self.dev = dev
        self.text = text

        self.frame = Image.new("1", (width, 8))
        self.draw = ImageDraw.Draw(self.frame)
        self.textWidth = self.draw.textsize(text, font=font)[0]

    def show(self, offset=(0,0)):
        """Draw and show the text with the given offset"""
        self.frame.paste(0)
        self.drawText(offset)
        self.dev.writeImage(self.frame)

    def drawText(self, offset=(0,0)):
        self.draw.text( ((self.frame.size[0] - self.textWidth)/2 + offset[0] - 1,
                         -2 + offset[1]),
                        self.text, fill=255, font=self.font)

    def _textAlignOuter(self, p):
        return [(self.frame.size[0]/2 + self.textWidth/2 + 4) * p[0],
                12 * p[1]]

    def scroll(self, start=(0,0), end=(0,0), speed=0.5):
        p = self._textAlignOuter(start)
        end = self._textAlignOuter(end)
        d = [sgn(end[i] - p[i]) for i in (0,1)]
        while d[0] or d[1]:
            self.show(p)
            p = [p[0]+d[0]*speed, p[1]+d[1]*speed]
            for i in (0,1):
                if (d[i] > 0 and p[i] >= end[i]) or (d[i] < 0 and p[i] <= end[i]):
                    d[i] = 0
                    p[i] = end[i]


class AnimatedImage:
    """A simple animation loop for the Rasterwand based on ImageDraw"""
    def __init__(self, dev,
                 numColumns   = 40,
                 displayWidth = 0.6,
                 dutyCycle    = 0.2
                 ):
        import Image, ImageDraw

        self.dev = dev
        dev.params.display_width = 0xFFFF * displayWidth
        dev.params.duty_cycle = 0xFFFF * dutyCycle
        self.numColumns = numColumns
        self.image = Image.new("1", (numColumns, 8))
        self.draw = ImageDraw.Draw(self.image)

    def run(self):
        then = time.time()
        while 1:
            now = time.time()
            self.image.paste(0)
            self.update(now - then)
            then = now
            self.dev.writeImage(self.image)

### The End ###

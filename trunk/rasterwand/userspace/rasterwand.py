#!/usr/bin/env python
import struct, os, fcntl


class Params:
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

    def __init__(self, fd):
        self.__dict__['fd'] = fd
        self.get()

    def get(self):
        buffer = chr(0)*struct.calcsize(self._struct)
        f = struct.unpack(self._struct, fcntl.ioctl(self.fd, 0x3B01, buffer))
        self.__dict__.update(dict(zip(self._fields, f)))

    def put(self):
        f = [int(self.__dict__[n]) for n in self._fields]
        fcntl.ioctl(self.fd, 0x3B02, struct.pack(self._struct, *f))

    def __setattr__(self, key, value):
        self.__dict__[key] = value
        self.put()


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
    def __init__(self, name="/dev/usb/rwand0"):
        self.fd = os.open(name, os.O_RDWR)
        self.params = Params(self.fd)
        self.status = Status(self.fd)

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


### The End ###

#!/usr/bin/env python
import struct, os, fcntl, Tweak


class Params:
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


p = Params(os.open("/dev/usb/rwand0", os.O_RDWR))

Tweak.Window(
    Tweak.Quantity(p, 'display_center', range=(0,0xFFFF)),
    Tweak.Quantity(p, 'display_width', range=(0,0xFFFF)),
    Tweak.Quantity(p, 'coil_center', range=(0,0xFFFF)),
    Tweak.Quantity(p, 'coil_width', range=(0,0xFFFF)),
    Tweak.Quantity(p, 'duty_cycle', range=(0,0xFFFF)),
    Tweak.Quantity(p, 'fine_adjust', range=(-500,500)),
    )

Tweak.run()

""" rcpod

Remote Controlled PIC of Doom
--Micah Dowty <micah@navi.picogui.org>

A python module for rapid firmware prototyping using Python and the
rcpod firmware and kernel module.

The firmware and kernel module act as a simple way to peek and poke
values into the PIC's RAM over USB. This python module provides an abstraction
for the PIC's peripherals.
"""

from fcntl import ioctl
import struct, glob


class Device:
    """Container for all hardware reachable through the rcpod interface.
       Device is a pattern to search for the device node with.
       """
    def __init__(self, devPattern="/dev/usb/rcpod*"):
        devs = glob.glob(devPattern)
        if not devs:
            raise IOError, "No rcpod device found"
        self.dev = open(devs[0], "w")

    def peek(self, address):
        """Return the byte at the given 9-bit address"""
        return ioctl(self.dev, 0x3702, address)

    def poke(self, address, data):
        """Store the given address/data pair"""
        ioctl(self.dev, 0x3701, struct.pack("HH", address, data))


if __name__ == '__main__':
    dev = Device()
#    dev.poke(0x87, 0x00)
#    dev.poke(0x07, 0xFF)
#    print dev.peek(0x87)
#    print dev.peek(0x07)
    import sys
    for address in xrange(0x200):
        sys.stdout.write(chr(dev.peek(address)))

### The End ###

""" rcpod.device

A more pythonically-happy wrapper around the rcpod device.
Handles library initialization at import time, provides
a function for enumerating attached rcpod devices, and
a class encapsulating the device.
"""
#
# Remote Controlled PIC of Doom
# Copyright (C) 2003 Micah Dowty <micahjd@users.sourceforge.net>
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

from librcpod import *

# These are the symbols that will be pulled from this module
# into the 'pyrcpod' package.
__all__ = ['scanForDevices', 'devices', 'mapAddress']


def mapAddress(name):
    """Given a register name, return the address of that register.
       Passes integers through unaffected.
       """
    if type(name) == type(''):
        return globals()['RCPOD_REG_' + name.upper()]
    return name


def to_ucharArray(list):
    """Converts a python list to a malloc'ed C unsigned char array.
       The resulting array must be freed with delete_ucharArray.
       """
    n = len(list)
    a = new_ucharArray(n)
    for i in xrange(n):
        ucharArray_setitem(a, i, list[i])
    return a


def from_ucharArray(a, n):
    """Converts a C unsigned character array with 'n' entries to a python list"""
    return [ucharArray_getitem(a, i) for i in xrange(n)]


class OpenedDevice:
    """Wraps an opened rcpod device, represented in librcpod
       by the rcpod_dev* opaque type.
       """
    def __init__(self, dev, availableDevice):
        self.dev = dev
        self.availableDevice = availableDevice

        # Make the scratchpad location more easily accessable
        self.scratchpadRange = (RCPOD_REG_SCRATCHPAD,
                                RCPOD_REG_SCRATCHPAD + RCPOD_SCRATCHPAD_SIZE)

        # Create pin descriptors corresponding to the ones in librcpod
        # of the form R*. (The others are building blocks for pin
        # descriptors, rather than complete descriptors for actual pins)
        for key, value in globals().iteritems():
            if key.startswith("RCPOD_PIN_R"):
                # Hack off the "RCPOD_PIN_" part and lowercase it
                pinName = key[10:].lower()

                # Wrap it in a Pin class
                self.__dict__[pinName] = Pin(self, value)

    def close(self):
        """Terminate our connection to the rcpod. No attributes
           on this class may be called afterwards.
           """
        if self.dev:
            rcpod_Close(self.dev)
            self.dev = None
            self.availableDevice.opened = False

    def reset(self):
        """Reset I/O-related registers to their power-on state.
           Automatically called in AvailableDevice.open() unless
           the 'reset' flag is set to False.
           """
        rcpod_Reset(self.dev)

    def poke(self, address, data):
        """Put the given 8-bit value into an address in the PIC's RAM.
           'address' can either be a numerical address or the name
           of a register (any RCPOD_REG_* or RCPOD_MEM_* constant).

           'data' can be either:
               - a character
               - a number between 0 and 255
               - a string
               - a list/tuple of values between 0 and 255

           If data is a list, tuple, or string, this function will poke
           multiple bytes starting at the given address.
           """
        address = mapAddress(address)

        # Convert strings and characters to lists and scalars
        if type(data) == type(''):
            if len(data) > 1:
                data = [ord(c) for c in data]
            else:
                data = ord(data)

        # If we have a scalar value, use rcpod_Poke. If we have
        # multiple bytes, use rcpod_PokeBuffer.
        if type(data) == type(()) or type(data) == type([]):
            try:
                arr = to_ucharArray(data)
                rcpod_PokeBuffer(self.dev, address, arr, len(data))
            finally:
                delete_ucharArray(arr)
        else:
            rcpod_Poke(self.dev, address, data)

    def peek(self, address, length=1, retType=int):
        """Read 'length' bytes from the given address in the PIC's RAM.
           'address' can either be a numerical address or the name
           of a register (any RCPOD_REG_* or RCPOD_MEM_* constant).
           Data will be returned in the given type, which may be:

             int  : Single bytes will be returned as integers,
                    multiple bytes will be returned as lists of integers
             list : Always returns a list of integers
             str  : Returns a string with the peek'ed bytes
           """
        address = mapAddress(address)

        if length > 1:
            # Use PeekBuffer, converting from a C array
            try:
                arr = new_ucharArray(length)
                rcpod_PeekBuffer(self.dev, address, arr, length)
                data = from_ucharArray(arr, length)
            finally:
                delete_ucharArray(arr)
        elif length == 1:
            data = [rcpod_Peek(self.dev, address)]
        else:
            data = []

        # data is now a list, convert it if necessary
        if retType == str:
            s = ""
            for value in data:
                s += chr(value)
            return s
        if retType == int and length == 1:
            return data[0]
        return data

    def analogReadAll(self):
        """Read all analog channels, returns a list of 8 values between 0 and 255"""
        arr = new_ucharArray(8)
        rcpod_AnalogReadAll(self.dev, arr)
        l = from_ucharArray(arr, 8)
        delete_ucharArray(arr)
        return l

    def analogReadChannel(self, c):
        """Read one analog channel, returns a values between 0 and 255"""
        if c < 0 or c > 7:
            raise ValueError("Channel number out of range")
        return rcpod_AnalogReadChannel(self.dev, c)


class Pin:
    """Encapsulates an rcpod pin descriptor, a value which describes
       one bit on an I/O port or it's tristate register, and its polarity.

       Normally a pin descriptor is first encountered as an attribute
       of an OpenedDevice, then modified if necessary using the pin
       descriptor's attributes.
       """
    def __init__(self, rcpod, value):
        self.rcpod = rcpod
        self.value = value

    def __repr__(self):
        """Decode the pin descriptor bitfields to give a human-readable representation"""
        polarityBit = self.value & RCPOD_PIN_HIGH
        trisBit = self.value & RCPOD_PIN_TRIS
        portBits = self.value & 0x38
        bitBits = self.value & 0x07

        if self.value == 0:
            return "<Pin (none)>"
        if portBits < RCPOD_PIN_PORTA or portBits > RCPOD_PIN_PORTE:
            return "<Pin (invalid)>"

        if trisBit:
            if polarityBit:
                desc = "input"
            else:
                desc = "output"
        else:
            if polarityBit:
                desc = "high"
            else:
                desc = "low"
        name = "R%s%s" % (" ABCDE"[portBits >> 3], bitBits)
        return "<Pin %s %s>" % (name, desc)

    def assert_(self):
        """Place this pin descriptor in its active state, setting the pin
           high if it is active-high, or low if it is active-low.
           """
        rcpod_GpioAssert(self.rcpod.dev, self.value)

    def test(self):
        """Return a boolean indicating whether this pin is currently asserted or not"""
        return bool(rcpod_GpioTest(self.rcpod.dev, self.value))

    def negate(self):
        """Return a new pin descriptor equivalent to this one except with
           the polarity reversed- active high becomes active low and vice versa.
           """
        return Pin(self.rcpod, self.value ^ RCPOD_PIN_HIGH)

    def output(self):
        """Return a pin descriptor that is asserted when this pin is an output"""
        return Pin(self.rcpod, (self.value | RCPOD_PIN_TRIS) & ~RCPOD_PIN_HIGH)

    def input(self):
        """Return a pin descriptor that is asserted when this pin is an input"""
        return Pin(self.rcpod, self.value | RCPOD_PIN_TRIS | RCPOD_PIN_HIGH)


class AvailableDevice:
    """A class representing an available but not opened rcpod device.
       Wraps the usb_device structure used by librcpod, and provides
       an open() method that creates an OpenDevice.
       """
    def __init__(self, usbdev):
        self.usbdev = usbdev
        self.opened = False

    def open(self, reset=True):
        """Open the device, and wrap it in an OpenedDevice class.
           By default the device is reset, though this can be overridden.
           """
        if self.opened:
            raise IOError("rcpod device is already open")
        self.opened = True
        opened = OpenedDevice(rcpod_Open(self.usbdev), self)
        if reset:
            opened.reset()
        return opened


# A list of available rcpod devices (AvailableDevice instances)
# Generated by updateDevices
devices = []

def scanForDevices():
    """Scans for available rcpod devices, updates the 'devices' list"""
    global devices
    pyrcpod_findDevices()

    # Convert librcpod's linked list into a python list, wrapping
    # each device in an AvailableDevice instance
    llist = rcpod_GetDevices()
    devices = []
    while llist:
        devices.append(AvailableDevice(llist))
        llist = pyrcpod_nextDevice(llist)

# Initialize librcpod and build an initial list of attached devices
pyrcpod_init()
scanForDevices()

### The End ###

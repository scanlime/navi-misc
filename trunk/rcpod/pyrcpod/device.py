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
__all__ = ['scanForDevices', 'devices', 'mapAddress', 'OpenedRcpod']


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


def from_ucharArray(a, n, retType=list):
    """Converts a C unsigned character array with 'n' entries to
       a python object, depending on retType:

       int  : Single bytes will be returned as integers,
              multiple bytes will be returned as lists of integers
       list : Always returns a list of integers
       str  : Returns a string with the peek'ed bytes
       """
    data = [ucharArray_getitem(a, i) for i in xrange(n)]
    if retType == str:
        s = ""
        for value in data:
            s += chr(value)
        return s
    if retType == int and n == 1:
        return data[0]
    return data


class OpenedRcpod:
    """Wraps an opened rcpod device, represented in librcpod
       by the rcpod_dev* opaque type.
       Functions are provided to interface with the librcpod API.
       Additionally, objects representing pin descriptors are available
       as attributes of this class, named after the pins. ('rb4' for example)

       On reset, the PIC's model number and the set of pins implemented
       in hardware is determined. The 'model' attribute is either "PIC16C745"
       or "PIC16C765", and the 'pins' attribute is a list of Pin instances.
       """
    def __init__(self, availableDevice, reset=True):
        if availableDevice.opened:
            raise IOError("rcpod device is already open")
        availableDevice.opened = True

        self.dev = rcpod_Open(availableDevice.usbdev)
        self.availableDevice = availableDevice

        # Make the scratchpad location more easily accessable
        self.scratchpadRange = (RCPOD_REG_SCRATCHPAD,
                                RCPOD_REG_SCRATCHPAD + RCPOD_SCRATCHPAD_SIZE)
        self.scratchpadSize = RCPOD_SCRATCHPAD_SIZE

        # Initially the PIC model number is unknown. It can be determined
        # automatically in reset(), or set manually if reset has been disabled.
        self.setModel(None)

        if reset:
            self.reset()

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

        # Based on the reset state of TRISD, we can determine the PIC's model
        # number. It gets initialized to 0xFF, but on the PIC16C745, the register
        # isn't implemented and will read as 0x00.
        trisd = self.peek('trisd')
        if trisd == 0x00:
            self.setModel('PIC16C745')
        elif trisd == 0xFF:
            self.setModel('PIC16C765')
        else:
            self.model = None
            raise ValueError('Incorrect value of TRISD after reset (0x%02X)' % trisd)

    def setModel(self, name):
        """Set the PIC model name, and generate a list of pins available on this PIC"""
        self.model = name

        # Create pin descriptors corresponding to the ones in librcpod
        # of the form R*. (The others are building blocks for pin
        # descriptors, rather than complete descriptors for actual pins)
        self.pins = {}
        for key, value in globals().iteritems():
            if key.startswith("RCPOD_PIN_R"):
                # Hack off the "RCPOD_PIN_" part and lowercase it
                pinName = key[10:].lower()

                # Don't do anything with this pin if it's not implemented
                # in hardware- this means PORTD and PORTE, on the PIC16C745
                if self.model == "PIC16C745" and (pinName.startswith('rd' or pinName.startswith('re'))):
                    continue

                # Wrap it in a Pin class
                self.pins[pinName] = Pin(self, value)

        # Merge in the pins dict with this class's dict, for convenience
        # (device.rb4, rather than device.pins['rb4'])
        self.__dict__.update(self.pins)

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
        try:
            arr = new_ucharArray(length)
            rcpod_PeekBuffer(self.dev, address, arr, length)
            data = from_ucharArray(arr, length, retType)
        finally:
            delete_ucharArray(arr)
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

    def assertPin(self, pin):
        """Assert the given pin descriptor, putting it in its active state"""
        rcpod_GpioAssert(self.dev, pin.value)

    def deassertPin(self, pin):
        """Deassert the given pin, putting it in its inactive state"""
        rcpod_GpioDeassert(self.dev, pin.value)

    def testPin(self, pin):
        """Return a True value if the given pin is asserted"""
        return rcpod_GpioTest(self.dev, pin.value)

    def assertPins(self, pins):
        """Assert every pin in the given list, in order"""
        arr = to_ucharArray([pin.value for pin in pins])
        rcpod_GpioAssertBuffer(self.dev, arr, len(pins))
        delete_ucharArray(arr)

    def deassertPins(self, pins):
        """Deassert every pin in the given list, in order"""
        arr = to_ucharArray([pin.value for pin in pins])
        rcpod_GpioDeassertBuffer(self.dev, arr, len(pins))
        delete_ucharArray(arr)

    def serialInit(self, baudRate, setPinDirections=True):
        """Initialize the serial port and set it to the given baud rate"""
        rcpod_SerialInit(self.dev, baudRate)
        if setPinDirections:
            Pin(self, RCPOD_PIN_TX).output().assert_()
            Pin(self, RCPOD_PIN_RX).input().assert_()

    def serialTxRxStart(self, txData):
        """Transmit the given buffer, then immediately begin receiving data.
           Currently the maximum size for txBytes is RCPOD_SCRATCHPAD_SIZE. The error handler
           will be called if this maximum is exceeded. The data will start being received
           into the rcpod's (tiny) internal buffer.
           currently this buffer is of size RCPOD_SCRATCHPAD_SIZE, and it will wrap around
           when full. The receive will continue until the next call to any of the
           following functions:
              serialTxRxStart, serialTx, serialRxStart, or serialRxFinish
           """
        # Convert strings to lists
        if type(txData) == type(''):
            txData = [ord(c) for c in txData]

        arr = to_ucharArray(txData)
        rcpod_SerialTxRxStart(self.dev, arr, len(txData))
        delete_ucharArray(arr)

    def serialTx(self, txData):
        """Transmit the given buffer. The same transmit buffer size
           limitation exists as in SerialTxRxStart
           """
        # Convert strings to lists
        if type(txData) == type(''):
            txData = [ord(c) for c in txData]

        arr = to_ucharArray(txData)
        rcpod_SerialTx(self.dev, arr, len(txData))
        delete_ucharArray(arr)

    def serialRxStart(self):
        """Start receiving data into the rcpod's (tiny) internal buffer.
           Currently this buffer is of size RCPOD_SCRATCHPAD_SIZE, and it will wrap around
           when full. The receive will continue until the next call to any of the
           following functions:
              SerialTxRxStart, SerialTx, SerialRxStart, or SerialRxFinish
           """
        rcpod_SerialRxStart(self.dev)

    def serialRxFinish(self):
        """Cancel the current receive. Normally this does not need to be done explicitly,
           but if you need to use the rcpod's scratchpad buffer for some other purpose,
           it may be.
           """
        rcpod_SerialRxFinish(self.dev)

    def serialRxProgress(self):
        """Return the number of bytes received so far, without stopping the
           receive in progress or retrieving any data. This will also detect
           a buffer overflow condition if one exists on the rcpod. If the
           rcpod's internal buffer overflows, an IOError will be raised.
           """
        return rcpod_SerialRxProgress(self.dev)

    def serialRxRead(self, retType=list):
        """Read any available bytes from the rcpod's on-chip serial receive
           buffer, making that space available for continued reception.
           A buffer overflow will be detected here, and it will cause an
           IOError to be raised.
           """
        buffer = new_ucharArray(RCPOD_SCRATCHPAD_SIZE)
        count = rcpod_SerialRxRead(self.dev, buffer, RCPOD_SCRATCHPAD_SIZE)
        data = from_ucharArray(buffer, count, retType)
        delete_ucharArray(buffer)
        return data

    def serialSetTxEnable(self, pin, setPinDirections=True):
        """Set the given Pin instance as a serial transmit enable.
           It will be asserted immediately before transmitting and deasserted
           after the transmission is completely finished.
           If setPinDirections is true, the pin will be made an output.
           """
        if pin:
            rcpod_SerialSetTxEnable(self.dev, pin.value)
            pin.output().assert_()
        else:
            rcpod_SerialSetTxEnable(self.dev, RCPOD_PIN_NONE)

    def serialUnsetTxEnable(self):
        self.serialSetTxEnable(None)


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
        self.rcpod.assertPin(self)

    def deassert(self):
        """Place this pin descriptor in its inactive state (same as pin.negate().assert_())"""
        self.rcpod.deassertPin(self)

    def test(self):
        """Return a boolean indicating whether this pin is currently asserted or not"""
        return self.rcpod.testPin(self)

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

    def open(self, cls=OpenedRcpod, reset=True):
        """Open the device, and wrap it in an OpenedDevice class.
           By default the device is reset, though this can be overridden.
           """
        return cls(self, reset)


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

""" rcpod.i2c

This package makes it easy to communicate with I2C peripherals
using Python and the RCPOD. The objects here provide abstractions for
individual I2C busses and devices, based on the low-level I2C code
in librcpod.

The other modules in this package provide interfaces to various I2C
devices. This includes generic devices like I2C EEPROMs, and specialized
devices sorted by manufacturer.
"""
#
# Remote Controlled PIC of Doom
# Copyright (C) 2003-2004 Micah Dowty <micahjd@users.sourceforge.net>
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

import time
from pyrcpod.librcpod import *
from pyrcpod.device import to_ucharArray, from_ucharArray

class I2CError(IOError):
    pass


class Bus(object):
    """One I2C bus, with any number of uniquely-addressed devices attached.
       A bus is uniquely identified by the two RCPOD pins it's connected to.
       """
    def __init__(self, clockPin, dataPin):
        assert clockPin.rcpod is dataPin.rcpod
        self.rcpod = clockPin.rcpod

        self.clock = clockPin
        self.data = dataPin

    def __repr__(self):
        return "<i2c.Bus on clock %r and data %r>" % (
            self.clock, self.data)


class Device(object):
    """Represents one I2C device on a bus attached to the rcpod"""
    busAddressBase = 0

    def __init__(self, bus, address, speed=0):
        self.bus = bus
        self.address = self.busAddressBase | address
        self._idev = new_i2cDev(speed, bus.clock.value,
                                bus.data.value, self.address)

    def __repr__(self):
        return "<i2c.Device at address 0x%02X on %r>" % (
            self.address, self.bus)

    def __del__(self):
        try:
            delete_i2cDev(self._idev)
        except AttributeError:
            pass

    def busWrite(self, txData):
        try:
            arr = to_ucharArray(txData)
            ackCount = rcpod_I2CWrite(self.bus.rcpod.dev, self._idev, arr, len(txData))
        finally:
            delete_ucharArray(arr)

        if ackCount == 0:
            raise I2CError("No device acknowledged address for write")
        elif ackCount < len(txData)+1:
            raise I2CError("Device stopped acknowledging %d bytes into write" % (ackCount-1))

    def busRead(self, count, retType=list):
        try:
            buffer = new_ucharArray(count)
            ackCount = rcpod_I2CRead(self.bus.rcpod.dev, self._idev, buffer, count)
            data = from_ucharArray(buffer, count, retType)
        finally:
            delete_ucharArray(buffer)

        if ackCount == 0:
            raise I2CError("No device acknowledged address for read")
        return data

    def busWriteRead(self, txData, rxCount, retType=list):
        """Do a write immediately followed by a read, may be faster than
           separate writes and reads in some common cases.
           """
        try:
            txBuffer = to_ucharArray(txData)
            rxBuffer = new_ucharArray(rxCount)
            ackCount = rcpod_I2CWriteRead(self.bus.rcpod.dev, self._idev,
                                          txBuffer, len(txData),
                                          rxBuffer, rxCount)
            rxData = from_ucharArray(rxBuffer, rxCount, retType)
        finally:
            delete_ucharArray(txBuffer)
            delete_ucharArray(rxBuffer)

        if ackCount == 0:
            raise I2CError("No device acknowledged address for write")
        elif ackCount < len(txData)+1:
            raise I2CError("Device stopped acknowledging %d bytes into write" % (ackCount-1))
        elif ackCount == len(txData)+1:
            raise I2CError("No device acknowledged address for read")
        return rxData

    def ping(self):
        """Just address this device and ensure that it ACKs.
           Returns None if the device is present, or an I2CError if it's nonresponsive.
           """
        self.busWrite([])

    def pollUntilReady(self, timeout):
        """Keep trying to address the device for up to 'timeout' seconds,
           raising an I2CError if it hasn't succeeded by this time.
           Some devices have to be polled in this way after some long-running
           process has started.
           """
        deadline = time.time() + timeout
        while time.time() < deadline:
            try:
                self.ping()
            except I2CError:
                pass
            else:
                return
        raise I2CError("Poll timeout, device still not ready")

### The End ###

""" rcpod.i2c.ti

Interfaces to specialized I2C devices produced by Texas Instruments
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
from pyrcpod import i2c


class DAC8574(i2c.Device):
    """The DAC8574 is a quad 16-bit D/A converter"""
    busAddressBase = 0x4C

    def setAddress(self, address):
        # Only the two low bits of the address are actually used to form
        # the I2C bus address- the other two bits are sent with the control
        # byte.
        self.address = self.busAddressBase | (address & 0x03)
        self.controlAddress = (address & 0x0C) << 6

    def shutdown(self):
        """Put the DAC into low-power mode. The next write operation
           will take it out of low-power mode, or you can explicitly
           accomplish this with resume().

           This connects the output to ground via a 100k resistor. The device
           supports other power-down modes, but this class hasn't implemented
           them yet.
           """
        self.busWrite([self.controlAddress | 0x01, 0x80, 0x00])

    def resume(self):
        """Explicitly exit low-power mode"""
        self.busWrite([self.controlAddress | 0x00, 0x00, 0x00])

    def write(self, a=None, b=None, c=None, d=None):
        """Update one or more of the device's channels simultaneously.
           Unspecified channels remain in their previous state. If floating
           point numbers are given, they are assumed to be in the range [0,1].
           Integers are assumed to be in the range [0x0000, 0xFFFF].
           All values are clamped to the DAC's range.
           """
        # Create a dictionary mapping channel number to value, only
        # for values that we're actually setting. Also convert floats to ints.
        channelDict = {}
        values = a,b,c,d
        for i in range(4):
            v = values[i]
            if v is not None:
                if type(v) is float:
                    v = int(v * 0xFFFF + 0.5)
                if v < 0:
                    v = 0
                if v > 0xFFFF:
                    v = 0xFFFF
                channelDict[i] = v

        if not channelDict:
            return

        # Send all channel values but the last one without updating the DACs, then
        # update all DACs with the last write we do.
        channels = channelDict.keys()
        for channel in channels[:-1]:
            # Load only, with no DAC update
            v = channelDict[channel]
            self.busWrite([self.controlAddress | (channel << 1), v >> 8, v & 0xFF])
        # DAC update on the last one
        channel = channels[-1]
        v = channelDict[channel]
        self.busWrite([self.controlAddress | 0x20 | (channel << 1), v >> 8, v & 0xFF])

    def read(self, channelNumber, returnType=float):
        """Read the current value of one DAC channel, returning the indicated data type"""
        if channelNumber < 0 or channelNumber > 3:
            raise ValueError("Channel number %d is out of range" % channelNumber)
        bytes = self.busWriteRead([self.controlAddress | (channelNumber << 1)], 2)
        code = (bytes[0] << 8) | bytes[1]

        if returnType is float:
            return code / 65535.0
        elif returnType is int:
            return code
        else:
            raise ValueError("Unsupported return type '%r'" % returnType)


class ADS1112(i2c.Device):
    """The ADS1112 is a 16-bit A/D converter with 3 single-ended inputs
       or 2 differential inputs.
       """
    busAddressBase = 0x48

    # Store the most recently configured values
    inputChannel = 0
    singleMode = False
    dataRate = 3
    gain = 0

    def readWithStatus(self, channel=None, returnType=float):
        """Read the latest value, returning None if no new value is available.
           Optionally changes the input channel before reading.
           """
        if channel is not None and channel != self.inputChannel:
            self.setChannel(channel)
            if not self.singleMode:
                # The A/D converter was probably already busy on the
                # previous channel- we need to wait for that to finish
                # so this next value is definitely from the correct channel.
                self.blockingRead()

        high, low, status = self.busRead(3)
        if status & 0x80:
            return None
        else:
            return self._decode(high, low, returnType)

    def blockingRead(self, channel=None, returnType=float, timeout=2.0):
        """Like readWithStatus, but we keep trying until new data is here
           or there's an error of some sort. A timeout, in seconds, limits
           the number of attempts we make.
           """
        deadline = time.time() + timeout
        while time.time() < deadline:
            result = self.readWithStatus(channel, returnType)
            if result is not None:
                return result
        raise IOError("Timeout in blocking read from ADS1112 device")

    def read(self, returnType=float):
        """Read the most recently converted value, without regard to whether
           it has already been read. This does not support changing input
           channels, since without reading the status bit we won't know if
           this data is actually from a different channel or not.
           """
        high, low = self.busRead(2)
        return self._decode(high, low, returnType)

    def _decode(self, high, low, returnType):
        """Given the high and low bytes returned by the ADC,
           convert them to the required return type.
           """
        # Unpack the signed 16-bit integer
        code = (high << 8) | low
        if code & 0x8000:
            code -= 0x10000

        if returnType is float:
            return float(code) / 0x7FFF
        elif returnType is int:
            return code
        else:
            raise ValueError("Unsupported return type '%r'" % returnType)

    def setChannel(self, channel):
        """Set the input channel number, as defined in the data sheet.
           Each channel setting selects a different pair of differential
           inputs. The voltage returned for each channel will be:

           0:  AIN0 - AIN1
           1:  AIN2 - AIN3
           2:  AIN0 - AIN3
           3:  AIN1 - AIN3
           """
        self.writeConfigRegister(inputChannel = channel)

    def enterSingleMode(self):
        """Put the A/D converter in single conversion mode.
           Conversions from now on will only be made explicitly.
           """
        self.writeConfigRegister(singleMode=True)

    def enterContinuousMode(self):
        """Put the A/D converter into continuous mode, explicit
           conversion commands will no longer be necessary.
           This is the default.
           """
        self.writeConfigRegister(singleMode=False)

    def startConversion(self):
        """Explicitly start a conversion, as is required in
           single conversion mode. By default this is not necessary.
           """
        self.writeConfigRegister(start=True)

    def writeConfigRegister(self,
                            start        = False,
                            inputChannel = None,
                            singleMode   = None,
                            dataRate     = None,
                            gain         = None,
                            ):
        """Write to the device's configuration register.
           All values not specified are set to the last
           values written.
           """
        if inputChannel is None:
            inputChannel = self.inputChannel
        if singleMode is None:
            singleMode = self.singleMode
        if dataRate is None:
            dataRate = self.dataRate
        if gain is None:
            gain = self.gain

        reg = 0
        if start:
            reg |= 0x80
        if singleMode:
            reg |= 0x10
        reg |= (inputChannel & 3) << 5
        reg |= (dataRate & 3) << 2
        reg |= (gain & 3)
        self.busWrite([reg])

        self.inputChannel = inputChannel
        self.singleMode = singleMode
        self.dataRate = dataRate
        self.gain = gain

### The End ###

""" rcpod.board

Contains subclasses of RcpodDevice that implement special features
of particular boards built around the rcpod. Construct these classes
with an AvailableRcpod instance, or pass them as an argument to
AvailableRcpod's open() method.
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
import device
import i2c
import i2c.ti

# These are the symbols that will be pulled from this module
# into the 'pyrcpod' package.
__all__ = ['Rcpod485', 'RcpodDB50']


class Rcpod485(device.OpenedRcpod):
    """Implements the special features of the rcpod-485 board.
       This board includes two LEDs and an RS-485 transceiver.
       """
    def __init__(self, *args, **kwargs):
        device.OpenedRcpod.__init__(self, *args, **kwargs)

        # Set up LEDs
        self.led1 = self.rc2
        self.led2 = self.rc1
        self.led1.output().assert_()
        self.led2.output().assert_()

        # Set up transmit enable pin
        self.txe = self.rd4
        self.serialSetTxEnable(self.txe)


class RcpodDB50(device.OpenedRcpod):
    """Implements the special features of an rcpod-db50 board.
       This board includes one LED, 8 channels of analog output
       via two quad-DAC chips, and an analog mux with voltage dividers.
       """
    def __init__(self, *args, **kwargs):
        device.OpenedRcpod.__init__(self, *args, **kwargs)

        # Set up LEDs
        self.led1 = self.rc2
        self.led1.output().assert_()

        # Set up the I2C bus and DACs
        self.i2cBus = i2c.Bus(self.rd6, self.rd7)
        self.dac1 = i2c.ti.DAC8574(self.i2cBus, 2)
        self.dac2 = i2c.ti.DAC8574(self.i2cBus, 3)

        # Set up the analog mux
        self.analogMuxPins = [self.rd0, self.rd1, self.rd2, self.rd3]
        for pin in self.analogMuxPins:
            pin.output().assert_()

        # Store gains for each analog channel. This is the
        # number our 0-255 reading is multiplied by to get
        # voltage. The defaults assume ideal resistors and
        # voltage reference, but this may be calibrated
        # by a separate tool.
        self.analogGains = {}
        for pin in (3,4,5,7,8,9,10):
            # Low-voltage internal inputs
            self.analogGains[pin] = 5.0 / 255
        for pin in xrange(27,43):
            # High-voltage multiplexed inputs (voltage divider,
            # ideally with 1.0k and 4.7k resistors)
            self.analogGains[pin] = (5.0 / 255) * ((1.0 + 4.7) / 1.0)

    def setAnalogMuxChannel(self, ch):
        """Set the current channel on the external analog multiplexer
           to the indicated value between 0 and 15.
           """
        pins = list(self.analogMuxPins)
        for i in range(4):
            if not (ch & (1<<i)):
                pins[i] = pins[i].negate()
        self.assertPins(pins)

    def analogScan(self, muxChannels=range(16)):
        """Scan through all multiplexer channels in the given list,
           taking analog readings for each. With the default settings,
           this reads all high voltage analog inputs once, and reads each
           low-voltage analog input 16 times. All readings are averaged
           together, and the result is a dictionary mapping DB-50 pin
           number to voltage in volts.
           """
        totals = {}
        nSamples = {}
        for muxChannel in muxChannels:
            self.setAnalogMuxChannel(muxChannel)

            # analogReadAll is faster usually than just reading
            # one channel, since this is implemented in firmware.
            # The first result is our current multiplexed input,
            # the rest are samples for the low-voltage inputs.
            v = self.analogReadAll()

            # Store the external mux results
            pin = 27 + muxChannel
            totals[pin] = totals.get(pin, 0) + v[0]
            nSamples[pin] = nSamples.get(pin, 0) + 1

            # Store the internal mux results
            for pin, ichannel in ( (3, 1), (4, 2), (5, 3), (7, 4),
                                   (8, 5), (9, 6), (10, 7) ):
                totals[pin] = totals.get(pin, 0) + v[ichannel]
                nSamples[pin] = nSamples.get(pin, 0) + 1

        for pin, total in totals.items():
            totals[pin] = float(total) / nSamples[pin] * self.analogGains[pin]
        return totals

    def analogOutput(self,
                     a=None, b=None, c=None, d=None,
                     e=None, f=None, g=None, h=None):
        """Update up to 8 analog channels. The channels located
           on the same DAC chip (a-d, e-h) will be updated synchronously.
           FIXME: The hardware supports updating multiple chips in sync, but
                  we don't yet.
           """
        self.dac1.write(a,b,c,d)
        self.dac2.write(e,f,g,h)

### The End ###


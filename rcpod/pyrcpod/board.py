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


""" rcpod.i2c.maxim

Interfaces to specialized I2C devices produced by Maxim
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

from pyrcpod import i2c


class MAX7300(i2c.Device):
    """The MAX7300 is an I2C-attached I/O expander, featuring 20
       bidirectional digital I/O pins.
       """
    busAddressBase = 0x40

    def shutdown(self):
        """Put the device into shutdown, tristating all ports"""
        self.busWrite([0x04, 0x00])

    def resume(self):
        """Resume normal operation after a shutdown"""
        self.busWrite([0x04, 0x01])

    def pinHigh(self, p):
        """Bring one pin high, given its port number"""
        print p
        self.busWrite([0x20 + p, 0x01])

    def pinLow(self, p):
        """Bring one pin low, given its port number"""
        self.busWrite([0x20 + p, 0x00])

    def pinRead(self, p):
        """Read the status of one pin, given its port number"""
        return self.busWriteRead([0x20 + p], 1)[0]

    def pinInput(self, p):
        """Make one pin a tristated input, given its port number"""

    def pinOutput(self, p):
        """Make one pin a driven output, given its port number"""


### The End ###

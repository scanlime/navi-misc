""" rcpod.i2c.microchip

Interfaces to specialized I2C devices produced by Microchip
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


class TC74(i2c.Device):
    """The TC74 is a small I2C temperature sensor with
       8-bit precision, returning its results directly
       in degrees celsius.
       """
    busAddressBase = 0x48

    def read(self):
        """Read the current temperature, in degrees C"""
        self.resume()
        return self.quickRead()

    def quickRead(self):
        """Like read(), but make sure the sensor isn't in
           shutdown mode first. If it might have been
           reset recently, this might give you an old reading.
           """
        temp = self.busWriteRead([0x00], 1)[0]
        if temp & 0x80:
            temp -= 256
        return temp

    def shutdown(self):
        """Put the sensor into its low-power mode"""
        self.busWrite([0x01, 0x80])

    def resume(self):
        """Resume normal sampling after a shutdown"""
        self.busWrite([0x01, 0x00])

### The End ###

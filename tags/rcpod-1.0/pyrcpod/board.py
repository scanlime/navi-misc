""" rcpod.board

Contains subclasses of RcpodDevice that implement special features
of particular boards built around the rcpod. Construct these classes
with an AvailableRcpod instance, or pass them as an argument to
AvailableRcpod's open() method.
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

import device, time

# These are the symbols that will be pulled from this module
# into the 'pyrcpod' package.
__all__ = ['Rcpod485']


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

### The End ###


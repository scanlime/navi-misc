""" pyunicone.device

Low-level interface for initializing the Unicone device
and communicating with controller emulators installed in its FPGA.

"""
#
# Universal Controller Emulator project
# Copyright (C) 2004 Micah Dowty
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#

from libunicone import *

__all__ = ["UniconeDevice"]


class UniconeDevice(object):
    """Low-level interface for initializing the Unicone device
       and communicating with controller emulators installed in its FPGA.
       """
    _dev = None
    _progress_py = None
    _progress_c = None

    def __init__(self):
        unicone_usb_init()
        self._dev = unicone_device_new()
        self._progress_c = progress_reporter_console_new()
        if not self._dev:
            raise IOError("Unable to open Unicone device")

    def __del__(self):
        if self._dev:
            unicone_device_delete(self._dev)
        if self._progress_c:
            progress_reporter_delete(self._progress_c)

    def configure(self, bitstream, firmware = "firmware.bin"):
        """Ensure that the Unicone device has the given FPGA bitstream
           and microcontroller firmware. If the firmware and bitstream
           are already installed, this returns quickly. If any downloads
           have to be performed, the current progress object reports
           status.
           """
        if unicone_device_configure(self._dev, firmware,
                                    bitstream, self._progress_c) < 0:
            raise IOError("Error configuring unicone device")

    def setLed(self, brightness, decayRate=0):
        """Set the brightness of the device's status LED,
           optionally also specifying a decay rate. The brightness
           is a number in the range [0, 1] with 16-bit resolution.

           The decay rate is also in the range [0, 1], in arbitrary
           units dependent on the speed of the microcontroller's
           main loop. It has no timer hardware that could be used
           to calibrate this.
           """
        if unicone_device_set_led(self._dev, brightness, decayRate) < 0:
            raise IOError("Error setting LED brightness")

    def i2cWrite(self, address, data):
        """Write an arbitrary packet to an I2C device with the
           given 7-bit address.
           """
        if unicone_device_i2c_write(self._dev, address, data) < 0:
            raise IOError("Error sending I2C write request")

    def _setProgress(self, progress):
        self._progress_py = progress
        if self._progress_c:
            progress_reporter_delete(self._progress_c)
        if progress:
            self._progress_c = progress_reporter_python_new(progress)
        else:
            self._progress_c = None

    def _getProgress(self):
        return self._progress_py

    progress = property(_getProgress, _setProgress)
    """By default, progress is reported on stdout. A Python implementation
       of a progress reporting object may be assigned to this property to
       report using it instead. Progress reporting can be suppressed entirely
       by setting this to None.
       """

### The End ###

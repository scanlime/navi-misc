""" PyUnicone.Device

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


class Emulator(object):
    """Abstract base class for controller emulators. Every emulator
       has a set of ports that hold Controller instances. These could
       be any type of Controller, but the emulator specifies the type
       of controller it's designed to work with. The emulator also
       specifies the name of the FPGA configuration that knows how
       to emulate this type of controller.

       The emulator has an onSync event that should be triggered
       to update the hardware with the current state of our virtual
       controllers. We encode all controllers, then send the encoded
       data to an I2C device in the FPGA.
       """
    numPorts = None
    controllerClass = None
    fpgaConfigName = None

    def __init__(self, uniconeDevice):
        self.dev = uniconeDevice
        self.ports = [None] * self.numPorts
        self.configure()

    def configure(self):
        self.dev.configure("fpga/%(fpgaConfigName)s/%(fpgaConfigName)s.bit" %
                           self.__class__.__dict__)

    def attach(self, controller, portNumber):
        self.ports[portNumber] = controller

    def remove(self, controller):
        self.ports[self.ports.index(controller)] = None

    def sync(self):
        """Send our our current controller status to the hardware.
           The default implementation calls encodePort on each
           port, concatenates the results, and sends the resulting
           packet to I2C address 0x40.
           """
        assert len(self.ports) == self.numPorts
        packet = ''.join([self.encodePort(port) for port in self.ports])
        self.dev.i2cWrite(0x40, packet)

        # Make the status LED glow a bit to indicate activity
        self.dev.setLed(0.05, 0.001)


def packAxisBytes(controller, format):
    """Given a Controller and a list of (actuator name, scale, bias) tuples,
       encode a set of axes into a string of bytes. The controller may be
       None, in which case all axes read zero.
       """
    bytes = []
    for name, scale, bias in format:
        if controller:
            ac = controller[name]
            if ac:
                v = ac.value or 0
            else:
                v = 0
        else:
            v = 0
        byte = min(0xFF, max(0x00, int(v * scale + bias + 0.5)))
        bytes.append(chr(byte))
    return ''.join(bytes)

def packButtonBits(controller, format):
    """Given a controller and a list of (actuator name, threshold, invert) tuples,
       encode a set of buttons into a string of bytes. The controller may be
       None, in which case all buttons are released. Without 'invert', a '1' bit
       is inserted anywhere the actuator value is greater than the given threshold.
       'invert' reverses this. It may be used in conjunction with an actuator name
       of None to insert constant bits. Bits are specified starting with the MSB
       of the first byte.
       """
    bytes = []
    byte = 0
    mask = 0x80
    for name, threshold, invert in format:
        if controller:
            ac = controller[name]
            if ac:
                v = (ac.value or 0) > threshold
            else:
                v = False
        else:
            v = False
        v ^= invert
        if v:
            byte |= mask
        mask >>= 1
        if not mask:
            bytes.append(chr(byte))
            byte = 0
            mask = 0x80
    if mask != 0x80:
        bytes.append(chr(byte))
    return ''.join(bytes)

### The End ###

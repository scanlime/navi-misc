""" PyUnicone.Device

Low-level interface for talking to our hardware and building
controller emulator objects. This focuses on our Unicone hardware
and loading emulator cores into its FPGA, but it also supports
other hardware like our Sega Genesis microcontroller.

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

import os
from libunicone import *

unicone_usb_init()


class UniconeDevice(object):
    """Low-level interface for initializing the Unicone device
       and communicating with controller emulators installed in its FPGA.
       """
    _dev = None
    _progress_py = None
    _progress_c = None

    def __init__(self):
        self._dev = unicone_device_new()
        self._progress_c = progress_reporter_console_new()
        if not self._dev:
            raise IOError("Unable to open Unicone device")

    def __del__(self):
        if self._dev:
            unicone_device_delete(self._dev)
        if self._progress_c:
            progress_reporter_delete(self._progress_c)

    def configure(self, firmware, bitstream):
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

_uniconeDevice = None
def getUniconeDevice():
    """Return the singleton UniconeDevice"""
    global _uniconeDevice
    if not _uniconeDevice:
        _uniconeDevice = UniconeDevice()
    return _uniconeDevice


class GenesisDevice(object):
    """Low-level interface for initializing the Sega Genesis controller
       emulation microcontroller and sending it status updates.
       """
    _dev = None

    def __init__(self):
        self._dev = genesis_device_new()
        if not self._dev:
            raise IOError("Unable to open Genesis device")

    def __del__(self):
        if self._dev:
            genesis_device_delete(self._dev)

    def update(self, wValue, wIndex):
        """Send a raw update. The caller is responsible for packing controller status."""
        genesis_device_update(self._dev, wValue, wIndex)

_genesisDevice = None
def getGenesisDevice():
    """Return the singleton GenesisDevice"""
    global _genesisDevice
    if not _genesisDevice:
        _genesisDevice = GenesisDevice()
    return _genesisDevice


class Emulator(object):
    """Abstract base class for controller emulators. Every emulator
       has a set of ports that hold Controller instances. These could
       be any type of Controller, but the emulator specifies the type
       of controller it's designed to work with.

       The emulator has an onSync event that should be triggered
       to update the hardware with the current state of our virtual
       controllers.
       """
    numPorts = None
    controllerClass = None

    def __init__(self):
        self.ports = [None] * self.numPorts

    def attach(self, portNumber):
        """Attach a controller on the given port number. This creates
           a new virtual controller, if necessary, and returns it.
           """
        if not self.ports[portNumber]:
            self.ports[portNumber] = self.controllerClass()
        return self.ports[portNumber]

    def remove(self, portNumber):
        self.ports[portNumber] = None

    def sync(self):
        """Send our our current controller status to the hardware.
           This should be implemented by subclasses.
           """
        pass


class FPGAEmulator(Emulator):
    """Abstract base class for controller emulators built around
       the Unicone device's FPGA. Every subclass specifies how
       each controller is encoded in the packet sent over I2C,
       and it specifies which FPGA configuration to use.
       """
    fpgaConfigName = None

    def __init__(self, uniconeDevice=None):
        if not uniconeDevice:
            uniconeDevice = getUniconeDevice()
        self.dev = uniconeDevice
        Emulator.__init__(self)
        self.configure()

    def configure(self, dataDir=None):
        """Configure the FPGA and/or microcontroller as necessary.
           We look for the firmware and FPGA files relative to our
           python module's directory by default.
           """
        if not dataDir:
            import PyUnicone
            moduleDir = os.path.split(PyUnicone.__file__)[0]
            dataDir = os.path.join(moduleDir, '..')
            
        self.dev.configure(os.path.join(dataDir, 'firmware.bin'),
                           os.path.join(dataDir, 'fpga', self.fpgaConfigName,
                                        "%s.bit" % self.fpgaConfigName))

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

### The End ###

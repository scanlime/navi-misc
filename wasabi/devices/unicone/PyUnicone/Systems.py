""" PyUnicone.Systems

Controller and emulator implementations specific to individual
video game systems.

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

import struct
import Device, Controller


class GamecubeController(Controller.Controller):
    """A virtual Nintendo Gamecube controller"""
    def __init__(self):
            Controller.Controller.__init__(self, "Gamecube Controller")
            self.addActuator(Controller.Button("BTN_START"))
            self.addActuator(Controller.Button("BTN_A"))
            self.addActuator(Controller.Button("BTN_B"))
            self.addActuator(Controller.Button("BTN_X"))
            self.addActuator(Controller.Button("BTN_Y"))
            self.addActuator(Controller.Button("BTN_Z"))
            self.addActuator(Controller.Button("BTN_TL"))
            self.addActuator(Controller.Button("BTN_TR"))
            self.addActuator(Controller.Axis("ABS_X"))
            self.addActuator(Controller.Axis("ABS_Y"))
            self.addActuator(Controller.Joystick("JS_ABS", self['ABS_X'], self['ABS_Y']))
            self.addActuator(Controller.Axis("ABS_RX"))
            self.addActuator(Controller.Axis("ABS_RY"))
            self.addActuator(Controller.Joystick("JS_ABS_RIGHT", self['ABS_RX'], self['ABS_RY']))
            self.addActuator(Controller.Axis("ABS_Z"))
            self.addActuator(Controller.Axis("ABS_RZ"))


class GamecubeEmulator(Device.FPGAEmulator):
    """Emulator for Gamecube controllers"""
    numPorts = 4
    fpgaConfigName = 'gamecube'
    controllerClass = GamecubeController

    def encodePort(self, port):
        """Convert the state of a GamecubeController to the 8-byte
           packet used internally on GC hardware and by the emulator.
           """
        if not port:
            return "\x00\x00\x80\x80\x80\x80\x00\x00"
        return (
            port.packButtonBits(
                (None,         0.5, 0),   # Unused bits
                (None,         0.5, 0),
                (None,         0.5, 0),
                ('BTN_START',  0.5, 0),
                ('BTN_Y',      0.5, 0),
                ('BTN_X',      0.5, 0),
                ('BTN_B',      0.5, 0),
                ('BTN_A',      0.5, 0),
                (None,         0.5, 1),   # Indicates that controller is ready?
                ('BTN_TL',     0.5, 0),
                ('BTN_TR',     0.5, 0),
                ('BTN_Z',      0.5, 0),
                ('ABS_HAT0Y',  0.5, 0),   # The D-pad
                ('ABS_HAT0Y', -0.5, 1),
                ('ABS_HAT0X',  0.5, 0),
                ('ABS_HAT0X', -0.5, 1),
                ) +
            port.packAxisBytes(
                ('ABS_X',      0x7F, 0x80),
                ('ABS_Y',     -0x7F, 0x80),
                ('ABS_RX',     0x7F, 0x80),
                ('ABS_RY',    -0x7F, 0x80),
                ('ABS_Z',      0xFF, 0x00),
                ('ABS_RZ',     0xFF, 0x00),
                ))


class GenesisController(Controller.Controller):
    """A virtual controller representing a Sega Genesis gamepad"""
    def __init__(self):
            Controller.Controller.__init__(self, "Sega Genesis Controller")
            self.addActuator(Controller.Button("BTN_A"))
            self.addActuator(Controller.Button("BTN_B"))
            self.addActuator(Controller.Button("BTN_C"))
            self.addActuator(Controller.Button("BTN_START"))
            self.addActuator(Controller.Axis("ABS_X"))
            self.addActuator(Controller.Axis("ABS_Y"))
            self.addActuator(Controller.Joystick("JS_ABS", self['ABS_X'], self['ABS_Y']))


class GenesisEmulator(Device.Emulator):
    """Emulator for two Sega Genesis controllers, using the special genesis
       emulation hardware, not the unicone device. This also includes two
       actuators for controlling the power and reset buttons.
       """
    numPorts = 2
    controllerClass = GenesisController

    def __init__(self, genesisDevice=None):
        if not genesisDevice:
            genesisDevice = Device.getGenesisDevice()
        self.dev = genesisDevice
        Device.Emulator.__init__(self)
        self.power = Controller.Button("BTN_POWER", 0)
        self.reset = Controller.Button("BTN_RESET", 0)

    def sync(self):
        assert len(self.ports) == self.numPorts
        portBytes = [self.encodePort(port) for port in self.ports]
        wIndex = 0
        if self.power.value:
            wIndex |= 0x02
        if self.reset.value:
            wIndex |= 0x01
        self.dev.update(portBytes[0] | (portBytes[1]<<8), wIndex)

    def encodePort(self, port):
        if not port:
            return 0
        return ord(port.packButtonBits(
            ('BTN_START',  0.5, 0),
            ('BTN_C',      0.5, 0),
            ('BTN_B',      0.5, 0),
            ('BTN_A',      0.5, 0),
            ('ABS_X',      0.4, 0),
            ('ABS_X',     -0.4, 1),
            ('ABS_Y',      0.4, 0),
            ('ABS_Y',     -0.4, 1),
            )[0])

### The End ###

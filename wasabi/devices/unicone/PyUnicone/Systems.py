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
import Device

class GamecubeEmulator(Device.Emulator):
    """Emulator for Gamecube controllers"""
    numPorts = 4
    controllerClass = None
    fpgaConfigName = 'gamecube'

    def encodePort(self, port):
        """Convert the state of a GamecubeController to the 8-byte
           packet used internally on GC hardware and by the emulator.
           """
        return (
            Device.packButtonBits(port, (
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
                )) +
            Device.packAxisBytes(port, (
                ('ABS_X',      0x7F, 0x80),
                ('ABS_Y',     -0x7F, 0x80),
                ('ABS_RX',     0x7F, 0x80),
                ('ABS_RY',    -0x7F, 0x80),
                ('ABS_Z',      0xFF, 0x00),
                ('ABS_RZ',     0xFF, 0x00),
                )))

### The End ###

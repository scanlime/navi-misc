""" Wasabi.Mixer

Python abstraction for setting the sound card's mixer levels,
implemented using the OSS /dev/mixer device.
"""
#
# Wasabi Project
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

from fcntl import ioctl
import struct


# Mapping from channel name to OSS channel number
channelDict = {
    'volume': 0,
    'bass': 1,
    'treble': 2,
    'synth': 3,
    'pcm': 4,
    'speaker': 5,
    'line': 6,
    'mic': 7,
    'cd': 8,
    'imix': 9,
    'altpcm': 10,
    'reclev': 11,
    'igain': 12,
    'ogain': 13,
    }


# The volume to set channels to when they're fully on.
# This is set to a value less than 1 to avoid clipping.
onVolume = 0.9


class Device(object):
    """Abstraction for an Open Sound System compatible mixer device.
       Mixer levels are represented as attributes of this device, scaled
       between 0 and 1.

       In the OSS mixer devices, channels are given predefined constants
       as identifiers. ioctls are provided to read and write the channels
       using these numbers. Volumes are represented as values from 0 to 100,
       with the left channel in the low byte and the right channel in the
       next 8 bits.
       """
    def __init__(self, device="/dev/mixer"):
        dev = open(device, "w")

        # Prevent invocation of __setattr__
        self.__dict__['dev'] = dev

    def read(self, channel):
        """Returns a tuple (left, right) with the two volumes for this channel, in the range [0,1]"""
        v = struct.unpack("i", ioctl(self.dev, 0x80044d00 | channel, struct.pack("i", 0)))[0]
        return ((v & 0xFF) / 100.0,
                ((v >> 8) & 0xFF) / 100.0)

    def write(self, channel, value):
        """Writes a volume in the range [0,1] to the specified channel. The value may be a single
           float to set left and right channels to the same value, or a (left, right) tuple.
           """
        if type(value) == type(()) or type(value) == type([]):
            left, right = value
        else:
            left = right = value
        v = int(left * 100.5) | (int(left * 100.5) << 8)
        ioctl(self.dev, 0xc0044d00 | channel, struct.pack("i", v))

    def __setattr__(self, attr, value):
        self.write(channelDict[attr], value)

    def __getattr__(self, attr):
        return self.read(channelDict[attr])

    def reset(self):
        """Set all channel volumes to zero"""
        for channel in channelDict.values():
            try:
                self.write(channel, 0)
            except:
                # Ignore unimplemented channels
                pass

### The End ###

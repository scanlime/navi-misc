""" Wasabi.VideoSwitch

A PyBZEngine-friendly abstraction for the USB video switch
"""
#
# Wasabi Project
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

from BZEngine import Event
import glob


class Device(object):
    """Abstraction for one uvswitch device. The device's input channels and
       bypass switch are accessable as attributes of this object.
       """
    def __init__(self, devPattern="/dev/usb/uvswitch*"):
        devs = glob.glob(devPattern)
        if not devs:
            raise IOError, "No uvswitch device found"
        self.dev = open(devs[0], "w")

        # Default settings show video from wasabi, with no external input selected
        self._videoChannel = 0
        self._bypassSwitch = 1
        self._whiteAudioChannel = 0
        self._redAudioChannel = 0
        self.update()

    def update(self):
        """Send all current settings to the device"""
        self.dev.write("%d %d %d %d\n" % (self._videoChannel,
                                          self._bypassSwitch,
                                          self._whiteAudioChannel,
                                          self._redAudioChannel))
        self.dev.flush()

    def setChannel(self, c):
        self._videoChannel = c
        self._whiteAudioChannel = c
        self._redAudioChannel = c
        self.update()

    def getVideoChannel(self):
        return self._videoChannel

    channel = property(getVideoChannel, setChannel)

    def setBypassSwitch(self, b):
        self._bypassSwitch = b
        self.update()

    def getBypassSwitch(self):
        return self._bypassSwitch

    bypassSwitch = property(getBypassSwitch, setBypassSwitch)

    def setAudioChannel(self, c):
        self._whiteAudioChannel = c
        self._redAudioChannel = c
        self.update()

    def setWhiteAudioChannel(self, c):
        self._whiteAudioChannel = c
        self.update()

    def setRedAudioChannel(self, c):
        self._RedAudioChannel = c
        self.update()

    def getWhiteAudioChannel(self):
        return self._whiteAudioChannel

    def getRedAudioChannel(self):
        return self._redAudioChannel

    audioChannel = property(getWhiteAudioChannel, setAudioChannel)
    whiteAudioChannel = property(getWhiteAudioChannel, setWhiteAudioChannel)
    redAudioChannel = property(getRedAudioChannel, setRedAudioChannel)

### The End ###

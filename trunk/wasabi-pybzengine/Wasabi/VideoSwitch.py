""" Wasabi.VideoSwitch

A PyBZEngine-friendly abstraction for the USB video switch
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

from BZEngine import Event, Util
from BZEngine.UI import Sequencer
from Wasabi import Icon
import glob


class Device(object):
    """Abstraction for one uvswitch device. The device's input channels and
       bypass switch are accessable as attributes of this object.

       If an eventLoop is provided, this will also set up events for detecting
       changes in the video input status.
       """
    def __init__(self, devPattern="/dev/usb/uvswitch*", eventLoop=None):
        devs = glob.glob(devPattern)
        if not devs:
            raise IOError, "No uvswitch device found"
        self.writeDev = open(devs[0], "w")
        self.readDev = open(devs[0], "r")

        # Default settings show video from wasabi, with no external input selected
        self._videoChannel = 0
        self._bypassSwitch = 1
        self._whiteAudioChannel = 0
        self._redAudioChannel = 0
        self.update()

        # Add ourselves to the main loop if we have one
        self.activeChannels = []
        Event.attach(self, "onChannelActive", "onChannelInactive", "onActiveChannelsChanged")

        if eventLoop:
            eventLoop.add(self)

    def update(self):
        """Send all current settings to the device"""
        self.writeDev.write("%d %d %d %d\n" % (self._videoChannel,
                                          self._bypassSwitch,
                                          self._whiteAudioChannel,
                                          self._redAudioChannel))
        self.writeDev.flush()

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

    def getSelectable(self):
        """Called by the main loop to see what items we have for its select()"""
        return self.readDev

    def needsWrite(self):
        """Called by the main loop to see if we need to poll for writing.
           We don't, since writes to uvswitch are relatively fast.
           """
        return False

    def pollRead(self, eventLoop):
        """Called by the main loop when we have new data from the device.
           From the uvswitch, this will be a line of text indicating which
           input ports are active.
           """
        line = self.readDev.readline().strip()
        if line:
            newActiveChannels = map(int, line.split(" "))
        else:
            newActiveChannels = []
        for channel in newActiveChannels:
            if not channel in self.activeChannels:
                self.onChannelActive(channel)
        for channel in self.activeChannels:
            if not channel in newActiveChannels:
                self.onChannelInactive(channel)
        self.activeChannels = newActiveChannels
        self.onActiveChannelsChanged()


inputDict = None

def getInputDict():
    """Returns a dictionary mapping port numbers to to icon instances.
       This executes the 'video_inputs.py' datafile, allows it to
       manipulate the dictionary as 'input' with all icons in its
       namespace, then caches the resulting dictionary.
       """
    global inputDict
    if not inputDict:
        inputDict = {}

        # Run video_inputs.py, with our dict in its namespace
        # as 'input' and all icons available to it.
        ns = {'input': inputDict}
        ns.update(Icon.getIconDict())
        exec open(Util.dataFile('video_inputs.py')) in ns

    return inputDict

### The End ###

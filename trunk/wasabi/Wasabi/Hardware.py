""" Wasabi.Hardware

Detection and initialization for all of wasabi's hardware. This includes
the mi6k (both through the mi6k module and through lircd) and the uvswitch
(through its device node).
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

import IR, Mixer, threading, time


class Devices:
    """Container for hardware connected to wasabi. Initializes what hardware
       is available, and provides functionality to tie the hardware and
       software together at a high level.
       """
    def __init__(self, eventLoop):
        # Try to connect to lircd
        try:
            IR.initDefaultClient(eventLoop)
        except:
            import sys
            self.warn("Can't connect to lircd, %s" % sys.exc_info()[1])

        # Try to initialize the mi6k
        try:
            import mi6k
            self.mi6k = mi6k.Device()
        except:
            import sys
            self.warn("Can't connect to the mi6k, %s" % sys.exc_info()[1])
            self.mi6k = None

        # Try to initialize the uvswitch
        try:
            import VideoSwitch
            self.uvswitch = VideoSwitch.Device(eventLoop=eventLoop)
        except IOError:
            import sys
            self.warn("Can't connect to the uvswitch, %s" % sys.exc_info()[1])
            self.uvswitch= None

        # Open the sound mixer (it should always be present)
        self.mixer = Mixer.Device()

        # Now that the hardware is connected (if it's connected) switch to wasabi's video and audio
        if self.uvswitch:
            self.selectWasabiVideo()

        # If we have an mi6k connection, start the updater thread
        if self.mi6k:
            self.mi6kUpdater = Mi6kUpdater(self.mi6k)
            self.mi6kUpdater.start()

    def warn(self, msg):
        """Issue a warning related to hardware initialization"""
        print "*** Warning: %s" % msg

    def selectWasabiVideo(self):
        """Show video from wasabi on the TV, with no external input connected"""
        self.setMixerDefaults()
        self.uvswitch.bypassSwitch = 1
        self.uvswitch.channel = 0

    def selectDirectVideo(self, channel):
        """Connect a video channel directly to the TV"""
        self.setMixerDefaults()
        self.uvswitch.channel = channel
        self.uvswitch.bypassSwitch = 0
        self.mixer.line = Mixer.onVolume

    def selectIndirectVideo(self, channel):
        """Connect a video channel to our capture card, and video from wasabi to the TV"""
        self.setMixerDefaults()
        self.uvswitch.channel = channel
        self.uvswitch.bypassSwitch = 1
        self.mixer.line = Mixer.onVolume

    def setMixerDefaults(self):
        """Set our mixer device with default volumes"""
        self.mixer.reset()
        self.mixer.volume = Mixer.onVolume
        self.mixer.pcm = Mixer.onVolume

    def shutdown(self):
        """Safely power down devices on exit"""
        if self.mi6k:
            self.mi6k.lights.blue = 0
            self.mi6k.lights.white = 0


class Mi6kUpdater(threading.Thread):
    """Thread to update the VFD and LEDs on the mi6k.
       These operations take a relatively long time and can't yet be made nonblocking.
       """
    def __init__(self, mi6k):
        threading.Thread.__init__(self)
        self.running = 1
        self.mi6k = mi6k

    def run(self):
        while self.running:
            self.mi6k.lights.blue = time.time() % 1

### The End ###

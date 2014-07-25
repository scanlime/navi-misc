""" Wasabi.Hardware

Detection and initialization for all of wasabi's hardware. This includes
the mi6k (both through the mi6k module and through lircd) and the uvswitch
(through its device node).
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

import IR, Mixer, threading, time, math
from BZEngine import Animated

# A place for the singleton Devices instance
dev = None

def initDevices(mainLoop):
    global dev
    dev = Devices(mainLoop)


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

            # Blank the VFD, don't power it off yet
            self.mi6k.vfd.writeScreen("")
            #self.mi6k.vfd.powerOff()

    def showStatus(self, status):
        """Display a simple status line on the VFD, if we have one"""
        if self.mi6k:
            self.mi6kUpdater.vfdPage = StatusClockVFDPage(status)


class Mi6kUpdater(threading.Thread):
    """Thread to update the VFD and LEDs on the mi6k.
       These operations take a relatively long time and can't yet be made nonblocking.
       """
    def __init__(self, mi6k):
        threading.Thread.__init__(self)

        self.mi6k = mi6k
        self.mi6k.vfd.powerOn()
        self.vfdPage = None

        IR.defaultClient.onReceivedCode.observe(self.irFeedback)

        self.running = 1
        self.start()

    def run(self, hzLimit=10):
        """Run self.integrate() up to hzLimit times per second, calculating the proper
           delta-t value.
           """
        then = time.time()
        while self.running:
            now = time.time()
            dt = now - then
            then = now
            self.integrate(dt)
            sleepTime = 1.0 / hzLimit - dt
            if sleepTime > 0:
                time.sleep(sleepTime)

    def integrate(self, dt):
        """Called every loop iteration with a delta-t value, to update the
           current state of the mi6k displays.
           """
        # Decay the current LED brightnesses
        lights = self.mi6k.lights
        for color in ('blue', 'white'):
            value = getattr(lights, color)
            # Don't bother updating it once it's below the smallest
            # light level the hardware can display.
            if value > 0.0001:
                value *= 0.2
                setattr(lights, color, value)

        # update the VFD
        if self.vfdPage:
            self.vfdPage.integrate(dt, self.mi6k.vfd)

    def irFeedback(self, code):
        """Visual feedback for received IR codes"""
        if code.remote == "wasabi":
            # A button on wasabi's remote
            self.mi6k.lights.blue = 0.1
        else:
            # Any other recognized remote
            self.mi6k.lights.white = 0.1


class VFDPage:
    """An abstract page of information that is displayed on the VFD. Updated by
       the Mi6kUpdater thread.
       """
    def integrate(self, dt, vfd):
        """Called by the updater thread with the delta-T since the last update, and the
           VFD to draw to.
           """
        pass


class StatusClockVFDPage(VFDPage):
    """A VFD page that displays a clock and an optional status line.
       Brightness is faded from the maximum to the minimum, to get the user's
       attention when this page is first loaded or a new status line is set.
       """
    def __init__(self, status=""):
        self.colonIndex = 0
        self.brightness = 1
        self.status = status

    def integrate(self, dt, vfd):
        # Clock with a flashing colon
        self.colonIndex += dt * 3
        self.colonIndex %= 2
        localTime = time.localtime()
        clockTime = time.strftime("%%H%s%%M" % ": "[int(self.colonIndex)], localTime)
        clockDate = time.strftime("%a %b %d", localTime)

        # If the status has changed, make the VFD bright and dim it gradually
        if self.brightness > 0.01:
            self.brightness -= dt * 2
        else:
            self.brightness = 0
        vfd.setBrightness(self.brightness)

        vfd.writeScreen("%s\n%5s%15s" % (self.status, clockTime, clockDate))

### The End ###

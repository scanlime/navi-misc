""" MI6K Vacuum Fluorescent Display plugin for Freevo

This file contains Freevo plugins for displaying status information
on the VFD included in the Media Infrawidget 6000 set-top-box peripheral.

It only controls the VFD and related display functionality of the MI6K,
IR receive and transmit is done through the de-facto standard LIRC daemon.
This requires a recent version of the 'mi6k' and 'vfdwidgets' python
modules, included in the MI6K source.

The MI6K does support lcdproc now, so in theory you could use freevo's
standard 'lcd' plugin. However, there are  a number of problems with this.
The MI6K's lcdproc driver requires a CVS version of lcdproc whereas the
PyLCD module used by freevo's lcd plugin doesn't support this recent
a version. The 'lcd' plugin uses hardcoded layouts which aren't nearly
as flexible as the vfdwidgets module. Finally, the MI6K supports brightness
control, power management, and LED features that lcdproc doesn't.

Copyright (C) 2004 Micah Dowty <micah@navi.cx>
"""

import plugin
import config
import audio.player
from event import *

import mi6k
import vfdwidgets

import time
import threading
import thread


class MediaClock(vfdwidgets.Clock):
    """A Clock widget that displays a time
       given in seconds, rather than wallclock time.
       """
    def __init__(self,
                 seconds   = 0,
                 format    = "%H:%M:%S",
                 blinkTime = None,
                 **kwargs):
        vfdwidgets.Clock.__init__(self, format, blinkTime, **kwargs)
        self.seconds = seconds

    def getTime(self):
        return time.gmtime(self.seconds)


class UpdaterThread(threading.Thread):
    """This thread runs continuously in the background, updating our VFD"""
    def __init__(self, vfd, surface):
        threading.Thread.__init__(self)
        self.vfd = vfd
        self.surface = surface
        self.stop = False
        self.lock = thread.allocate_lock()

    def run(self):
        oldFrame = None
        while not self.stop:
            self.surface.update()
            self.lock.acquire()
            try:
                frame = self.surface.draw()
            finally:
                self.lock.release()
            if frame == oldFrame:
                # Nothing changed, wait a few ticks
                time.sleep(0.01)
            else:
                # Do a blocking write of the latest frame
                self.vfd.writeLines(frame)
            oldFrame = frame
        self.vfd.clear()


class PluginInterface(plugin.DaemonPlugin):
    """Plugin to start and stop the VFD updater thread, and pass events to it"""
    def __init__(self, device="/dev/usb/mi6k*"):
        plugin.DaemonPlugin.__init__(self)
        self.osdWidget = None
        self.lastDrawHandler = None

        self.mi6k = mi6k.Device(device)
        self.vfd = self.mi6k.vfd
        self.initVfd()

        self.surface = vfdwidgets.Surface(self.vfd.width, self.vfd.lines)
        self.initWidgets()

        self.updater = UpdaterThread(self.vfd, self.surface)
        self.updater.setDaemon(1)
        self.updater.start()

    def initVfd(self):
        """Power on the VFD, set defaults, and allocate characters"""
        self.vfd.powerOn()
        self.vfd.setBrightness(0.1)

        self.ellipses = self.vfd.allocCharacter([
            [ 0, 0, 0, 0, 0 ],
            [ 0, 0, 0, 0, 0 ],
            [ 0, 0, 0, 0, 0 ],
            [ 0, 0, 0, 0, 0 ],
            [ 0, 0, 0, 0, 0 ],
            [ 0, 0, 0, 0, 0 ],
            [ 1, 0, 1, 0, 1 ] ])

    def shutdown(self):
        while self.updater.isAlive():
            self.updater.stop = True
            time.sleep(0.1)

    def initWidgets(self):
        """Add all permanent widgets to our VFD surface"""

        # Add some low-priority widgets to fill in the space
        # if nothing else is present- a title at the top, and
        # a clock in the lower-left.
        self.surface.add(vfdwidgets.Clock(gravity=(-10,1),
                                          priority=0))
        if hasattr(config, 'VFD_TITLE'):
            self.surface.add(vfdwidgets.Text(config.VFD_TITLE,
                                             gravity=(0,-10),
                                             priority=-1))

        # The title of the currently selected item
        self.selectionTitle = vfdwidgets.LoopingScroller(visible=False,
                                                         gravity=(0,-5),
                                                         priority=1)
        self.surface.add(self.selectionTitle)

        # A relatively high priority widget with the title of the
        # currently playing video or audio track.
        self.playerTitle = vfdwidgets.LoopingScroller(visible=False,
                                                      gravity=(0,-1),
                                                      priority=10)
        self.surface.add(self.playerTitle)

        # Add another two higher-priority clocks at the lower-right
        # to show our progress through media files.
        self.elapsedTime = MediaClock(gravity=(9,1),
                                      priority=5,
                                      visible=False)
        self.surface.add(self.elapsedTime)
        self.totalTime = MediaClock(gravity=(10,1),
                                    priority=4,
                                    visible=False)
        self.surface.add(self.totalTime)

    def drawBegin(self):
        """Called before any draw_* handler, or even if we have no handler
           for the current object type. This resets all widgets to an
           inactive state, so widgets we aren't using will go away.
           """
        self.selectionTitle.visible = False
        self.elapsedTime.visible = False
        self.totalTime.visible = False
        self.playerTitle.visible = False

    def draw(self, (type, object), osd):
        """This handler gives plugins a chance to draw extra information
           on the display. We don't actually draw anything or even update
           the VFD here, but we do collect information about freevo's
           current state and update our widgets with it. We dispatch
           this to draw_* handlers based on the type of object onscreen.
           """
        f = getattr(self, "draw_%s" % type, None)
        if f != self.lastDrawHandler:
            # We've switched gears, relayout the screen next chance we get
            self.surface.layoutRequired = True
            self.lastDrawHandler = f

        self.updater.lock.acquire()
        try:
            self.drawBegin()
            if f:
                f(object, osd)
        finally:
            self.updater.lock.release()

    def eventhandler(self, event, menuw=None):
        """Handle special events containing messages we should display"""
        if event == OSD_MESSAGE:
            self.osdMessage(event.arg)

    def osdMessage(self, message):
        """Show an OSD message, replacing any previous OSD message
           we may have been in the middle of showing.
           """
        # Make this atomic so we don't see the old OSD momentarily
        # disappear if we're replacing one that's still active.
        self.updater.lock.acquire()
        try:
            try:
                self.surface.remove(self.osdWidget)
            except ValueError:
                pass
            w = vfdwidgets.Text(message,
                                gravity  = (0, 100),
                                priority = 100,
                                ellipses = self.ellipses)
            self.surface.add(w, lifetime = 1)
            self.osdWidget = w
        finally:
            self.updater.lock.release()

    def draw_menu(self, menuw, osd):
        """Update our widgets with information about the current
           menu and the currently hilighted menu item. This should
           work well enough to give us basic navigation even with
           the TV turned off.
           """
        # If audio is playing anyway, the audio player is probably
        # running in detached mode. Show its status too, even if it
        # covers up menu status.
        if audio.player.get() and audio.player.get().player:
            self.draw_player(audio.player.get().item)

        menu = menuw.menustack[-1]
        self.selectionTitle.visible = True
        self.selectionTitle.text = menu.selected.name

    def draw_player(self, player, osd=None):
        self.playerTitle.visible = True
        self.playerTitle.text = player.getattr('title') or player.getattr('name')

        if hasattr(player, 'elapsed'):
            self.elapsedTime.visible = True
            self.elapsedTime.seconds = player.elapsed
            if player.elapsed >= 3600:
                self.elapsedTime.format = "%H:%M:%S"
            else:
                self.elapsedTime.format = "%M:%S"

        if hasattr(player, 'length'):
            self.totalTime.visible = True
            self.totalTime.seconds = player.length
            if player.length >= 3600:
                self.totalTime.format = "/ %H:%M:%S"
            else:
                self.totalTime.format = "/ %M:%S"

        # Always re-layout if we get here- it's easier than
        # keeping track of whether any of the time formats changed :P
        self.surface.layoutRequired = True

### The End ###

""" PyUnicone.Freevo

Glue between the rest of PyUnicone and the Freevo set-top-box
framework. This includes hooks to automaticaly set up controller
emulation when a video game system is selected, and a progress
reporter that uses freevo's GUI.

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

import struct, time, threading, re
import Device, Controller

import rc
from event import *


class ProgressReporter:
    """A progress reporter, compatible with UniconeDevice,
       that uses Freevo's GUI ProgressBox.
       """
    def start(self, name):
        return ProgressOperation(self, name)

    def message(self, s):
        print s

    def error(self, s):
        from gui import AlertBox
        AlertBox(text=s).show()

class ProgressOperation:
    """Represents one operation, currently being executed, that
       we're getting progress reports to. It's created by
       ProgressReporter, and corresponds to one freevo progress box.
       """
    def __init__(self, reporter, name):
        self.reporter = reporter
        self.lastUpdateTime = time.time()

        from gui import ProgressBox
        self.box = ProgressBox(text=name, full=100)
        self.box.show()

    def update(self, completion):
        # Limit the rate at which we redraw the progress bar, since
        # it's kind of CPU intensive.
        now = time.time()
        if now < self.lastUpdateTime + 0.1:
            return
        
        self.box.progressbar.position = int(completion * 100 + 0.5)
        self.box.draw(update=True)
        self.lastUpdateTime = now

    def finish(self, error):
        self.box.destroy()
        if error:
            self.reporter.error(error)


class UniconeHooks:
    """Hook functions for use with the freevo uvswitch.input plugin, also
       part of the Wasabi project. This is how we get notified when a
       video game system becomes active.
       """
    def __init__(self, emulatorClass):
        self.emulatorClass = emulatorClass

    def start(self):
        # Install our freevo-happy progress reporter, if this is
        # an emulator that will need it.
        if issubclass(self.emulatorClass, Device.FPGAEmulator):
            Device.getUniconeDevice().progress = ProgressReporter()

        # Create the emulator. On many systems, this takes a few seconds
        # because an FPGA configuration must be downloaded.
        self.emulator = self.emulatorClass()

        # The EvdevBus monitors for controller hotplug events,
        # and collects updates from all active controllers.
        self.bus = Controller.EvdevBus()
        self.bus.onPlug.observe(self.plug)
        self.bus.onUnplug.observe(self.unplug)
        self.bus.onSync.observe(self.emulator.sync)

        # Create a separate thread to run the EvdevBus's main loop
        self.eventThread = threading.Thread(target=self.bus.mainLoop)
        self.eventThread.start()

    def stop(self):
        # Don't join the thread here- it will take a couple seconds
        # to die, due to the select() timeout. We don't really need to
        # make sure it's dead just yet.
        self.bus.running = False

    def plug(self, c):
        """A new controller was just plugged in"""
        # FIXME: For now we hardcode gamecube controllers
        try:
            c.portNumber = int(re.match("Gamecube Controller (\d+)", c.name).group(1))
        except:
            return
        rc.post_event(Event(OSD_MESSAGE, arg="(+) Controller %d" % c.portNumber))

        # FIXME: This mapping has a special case for the Genesis. This all
        #        needs to be made spiffier and stuff.
        mapping = Controller.Mapping(c, self.emulator.attach(c.portNumber-1))
        mapping.matchNames()
        try:
            # Ugly workaround for the Sega Genesis
            mapping.map("BTN_START", "BTN_C")
        except KeyError:
            pass

    def unplug(self, c):
        """A controller was just unplugged"""
        if hasattr(c, 'portNumber'):
            rc.post_event(Event(OSD_MESSAGE, arg="(-) Controller %d" % c.portNumber))

### The End ###

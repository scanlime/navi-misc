""" Wasabi.Main

Top-level initialization of the Wasabi UI. This starts up the PyBZEngine and
hardware components, then uses Sequencer to start the menu.
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

import os, pygame, threading
from BZEngine.UI import Viewport, ThreeDRender, ThreeDControl, Sequencer, Input
from BZEngine import Event
from Wasabi import Hardware, Logos, Menu, IR, Icon, VideoSwitch, Settings

# Modify the default font size so we can actually see it on a TV.
# Only render it at one size, since on a TV we also don't care about hinting
from BZEngine.UI import GLText
GLText.defaultSize = 30
GLText.defaultSizes = [GLText.maxRenderSize]


class Main:
    """Initializes other components and store references to them, runs the main loop"""
    def __init__(self):
        self.loop = Event.EventLoop()

        # Detect hardware
        Hardware.initDevices(self.loop)

        # An nvidia-widget to sync to vertical blank, to avoid tearing. Should
        # have no effect on platforms where this isn't supported.
        os.environ['__GL_SYNC_TO_VBLANK'] = '1'

        # If we're running on actual wasabi hardware, make sure we use the local X display.
        # (Just a convenience, since this will generally be run over ssh for testing)
        if Hardware.dev.uvswitch:
            os.environ['DISPLAY'] = ':0'

        self.viewport = Viewport.OpenGLViewport(self.loop)
        self.viewport.setCaption("Wasabi UI")
        self.view = ThreeDRender.View(self.viewport)

        # The UI runs fine without ThreeDControl, but it's useful for debugging
        # and doesn't get in the way during normal use.
        self.control = ThreeDControl.Viewing(self.view, self.viewport)

        # If we're running on actual wasabi hardware, go fullscreen
        if Hardware.dev.uvswitch:
            self.viewport.display.toggle_fullscreen()
            pygame.mouse.set_visible(0)

        # Set up our main book
        self.book = Sequencer.CyclicBook(self.view, self.getPages())

    def getPages(self):
        """Return a list of pages to include in the main sequencer book"""
        return [
            # Display the logo sequence until user interruption
            Sequencer.FadeOut(0.2, (1,1,1), Menu.userPageInterrupter(Logos.getLogoSubBook())),

            # Run the main menu, with fade in and out
            Sequencer.FadeIn(0.5, (1,1,1), Sequencer.FadeOut(0.25, (0,0,0), MainMenu))

            # The main menu will insert additional pages at the end,
            # depending on the selection made.
            ]

    def run(self):
        """Run the main loop, doesn't return until the program exits"""
        try:
            self.loop.run()
        finally:
            # Get all our threads to stop gracefully
            for thread in threading.enumerate():
                if thread != threading.currentThread():
                    thread.running = False
                    thread.join()

            # Shut down hardware gracefully
            Hardware.dev.shutdown()


class VideoChannelPage(Sequencer.Page):
    """A sequencer page that displays video from an external input, via uvswitch"""
    def __init__(self, view, channel):
        Sequencer.Page.__init__(self, view)
        Hardware.dev.showStatus(VideoSwitch.getInputDict()[channel].text)
        Hardware.dev.selectDirectVideo(channel)

    def finalize(self):
        Hardware.dev.selectWasabiVideo()


class VideoInput(Menu.PageItem):
    """A menu item representing a video input. This is added to the MainMenu whenever
       it becomes active as specified by the video switch.
       """
    def __init__(self, channel):
        Menu.PageItem.__init__(self,
                               VideoSwitch.getInputDict()[channel],
                               Menu.userPageInterrupter(lambda book: VideoChannelPage(book, channel))
                               )


class MainMenu(Menu.RingMenu):
    """A RingMenu subclass that displays available devices, and lets the user select one.
       Devices can be made available and unavailable during the operation of the menu, with
       the menu properly reflecting this.
       """
    def __init__(self, book):
        # Add items that always appear on the menu
        menuItems = [
            Menu.Item(Icon.load('navi')),
            Menu.PageItem(Icon.load('settings'), Menu.defaultFades(Settings.SettingsMenu)),
            ]

        # If we have a video switch device, integrate it with the main menu
        if Hardware.dev.uvswitch:

            # Add items for video devices that are already active
            self.channelItems = {}
            for channel in Hardware.dev.uvswitch.activeChannels:
                item = VideoInput(channel)
                self.channelItems[channel] = item
                menuItems.append(item)

            # Observe the video detection events, so icons can be dynamically added and removed
            Hardware.dev.uvswitch.onChannelActive.observe(self.addChannel)
            Hardware.dev.uvswitch.onChannelInactive.observe(self.removeChannel)

        Hardware.dev.showStatus("Main Menu")

        Menu.RingMenu.__init__(self, book, menuItems)

    def addChannel(self, channel):
        """Observing the uvswitch onChannelActive event, this is called when a new
           channel becomes active, to add an icon to the menu.
           """
        item = VideoInput(channel)
        self.channelItems[channel] = item
        self.add(item)

    def removeChannel(self, channel):
        """Observing the uvswitch onChannelInactive event, this is called when a new
           channel becomes inactive, to remove its icon from the menu.
           """
        item = self.channelItems[channel]
        del self.channelItems[channel]
        self.remove(item)

### The End ###

""" Wasabi.Main

Top-level initialization of the Wasabi UI. This starts up the PyBZEngine and
hardware components, then uses Sequencer to start the menu.
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

import os, pygame
from BZEngine.UI import Viewport, ThreeDRender, ThreeDControl, Sequencer, Input
from BZEngine import Event
from Wasabi import Hardware, Logos, Menu, IR, Icon, VideoSwitch


class Main:
    """Initializes other components and store references to them, runs the main loop"""
    def __init__(self):
        self.loop = Event.EventLoop()

        # Detect hardware
        self.hardware = Hardware.Devices(self.loop)

        # An nvidia-widget to sync to vertical blank, to avoid tearing. Should
        # have no effect on platforms where this isn't supported.
        os.environ['__GL_SYNC_TO_VBLANK'] = '1'

        # If we're running on actual wasabi hardware, make sure we use the local X display.
        # (Just a convenience, since this will generally be run over ssh for testing)
        if self.hardware.uvswitch:
            os.environ['DISPLAY'] = ':0'

        self.viewport = Viewport.OpenGLViewport(self.loop)
        self.viewport.setCaption("Wasabi UI")
        self.view = ThreeDRender.View(self.viewport)

        # If we're running on actual wasabi hardware, go fullscreen
        if self.hardware.uvswitch:
            self.viewport.display.toggle_fullscreen()
            pygame.mouse.set_visible(0)
        else:
            # Otherwise, set up a ThreeDControl to make debugging easier
            self.control = ThreeDControl.Viewing(self.view, self.viewport)

        # Set up our main book
        self.book = Sequencer.CyclicBook(self.view, self.getPages())

    def getPages(self):
        """Return a list of pages to include in the main sequencer book"""
        return [
            # Display the logo sequence until user interruption
            Sequencer.FadeOut(0.2, (1,1,1), logoInterrupter),

            # Run the main menu, with fade in and out
            Sequencer.FadeIn(0.5, (1,1,1), Sequencer.FadeOut(0.25, (0,0,0), lambda book:
                                                             MainMenu(book, self.hardware)))

            # The main menu will insert additional pages at the end,
            # depending on the selection made.
            ]

    def run(self):
        """Run the main loop, doesn't return until the program exits"""
        self.loop.run()


def logoInterrupter(book):
    """A wrapper around the book of logos that runs them until any IR code
       is received, the mouse is clicked, or the space/enter keys are pressed.
       """
    events = [
        Input.KeyPress(book.viewport, pygame.K_SPACE),
        Input.KeyPress(book.viewport, pygame.K_RETURN),
        Input.MousePress(book.viewport, 1),
	IR.ButtonPress(),
	]
    return Sequencer.PageInterrupter(events, Logos.getLogoSubBook())(book)


class VideoInput(Menu.Item):
    """A menu item representing a video input. This is added to the MainMenu whenever
       it becomes active as specified by the video switch.
       """
    def __init__(self, mainMenu, channel):
        self.mainMenu = mainMenu
        self.channel = channel
        Menu.Item.__init__(self, VideoSwitch.getInputDict()[channel])

    def onSelected(self, item):
        print self.channel


class MainMenu(Menu.RingMenu):
    """A RingMenu subclass that displays available devices, and lets the user select one.
       Devices can be made available and unavailable during the operation of the menu, with
       the menu properly reflecting this.
       """
    def __init__(self, book, hardware):
        self.hardware = hardware

        # Add items that always appear on the menu
        menuItems = [
            Menu.Item(Icon.load('navi')),
            ]

        # Add items for video devices that are already active
        for channel in self.hardware.uvswitch.activeChannels:
            menuItems.append(VideoInput(channel))

        Menu.RingMenu.__init__(self, book, menuItems)

### The End ###

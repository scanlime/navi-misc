""" Wasabi.Menu

Sequencer pages implementing screens for browsing through
and selecting items.
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

from BZEngine.UI import Sequencer, Layout, HUD
from BZEngine import Event
from Wasabi import Icon
from math import *
import pygame


class Item:
    """A single menu items, used by all Menu subclasses. Specifies
       an icon for the menu item to be represented by, and an event handler
       to be called when the item is selected.
       """
    def __init__(self, icon):
        Event.attach(self, 'onSelected')
        self.icon = icon


class Menu(Sequencer.Page):
    """Abstract base class for menus. All menus have common code for managing
       backgrounds, and all menus signal selections compatibly. The menu item's
       onSelected event is called when a selection is made, then the menu's onFinish
       is called to end this page. Normally the onSelected event would push new pages
       onto the sequencer book to implement the selection.
       """
    def __init__(self, book):
        Sequencer.Page.__init__(self, book)

        # Create an ortho mode viewport over the whole screen
        self.overlay = self.viewport.region(Layout.Rect(self.viewport))

        # Draw a background image
        self.background = HUD.Background(self.overlay, "brushed_metal.png")


class RingMenu(Menu):
    def __init__(self, book, items):
        Menu.__init__(self, book)
        self.dock = Icon.Dock(self.overlay, self.trackFunction, [item.icon for item in items])
        self.viewport.onKeyDown.observe(self.keyDown)

    def keyDown(self, ev):
        if ev.key == pygame.K_SPACE:
            self.onFinish()
        elif ev.key == pygame.K_LEFT:
            self.dock.selectionIndex -= 1
        elif ev.key == pygame.K_RIGHT:
            self.dock.selectionIndex += 1

    def trackFunction(self, x):
        """A track function that moves the icons along a circle in
           the middle of the screen. The bottom of the circle is at 0,
           increasing parameters move clockwise. The icon at 0 is the largest.
           """
        radius = self.viewport.size[1] * 0.04
        theta = -x * 2*pi + pi/2
        size = (pow(sin(theta)*0.5+0.5, 3) * 0.20 + 0.06) * self.viewport.size[1]
        center = (self.viewport.size[0] * 0.5,
                  self.viewport.size[1] * 0.25)
        aspect = 2.9
        return (
            (cos(theta) * aspect * (radius + size) + center[0],
             sin(theta) * (radius + size) + center[1]),
            size
            )

    def finalize(self):
        # UGLY HACK!!!!!!!!!
        self.dock = None

### The End ###

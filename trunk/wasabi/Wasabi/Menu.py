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

from BZEngine.UI import Sequencer, Layout, HUD, Input
from BZEngine import Event
from Wasabi import Icon
from math import *
import pygame


class Item:
    """A single menu items, used by all Menu subclasses. Specifies
       an icon for the menu item to be represented by, and an event handler
       to be called when the item is selected.

       The onSelected() event is passed a reference to the menu selecting this item.
       """
    def __init__(self, icon):
        Event.attach(self, 'onSelected')
        self.icon = icon


class PageItem(Item):
    """A menu item that, upon selection, pushes a specified page onto the current book"""
    def __init__(self, icon, page):
        self.page = page
        Item.__init__(self, icon)

    def onSelected(self, menu):
        self.menu = menu
        menu.book.pushBack(self.page)


class Menu(Sequencer.Page):
    """Abstract base class for menus. All menus have common code for managing
       backgrounds, and all menus signal selections compatibly. The menu item's
       onSelected event is called when a selection is made, then the menu's onFinish
       is called to end this page. Normally the onSelected event would push new pages
       onto the sequencer book to implement the selection.

       The menu's onSelected event is called when any item is selected. It is given
       a reference to the Item instance selected.
       """
    def __init__(self, book):
        Sequencer.Page.__init__(self, book)
        Event.attach(self, 'onSelected')
        self.bindings = Input.load(self.viewport, self, self.keyFile)

        # Create an ortho mode viewport over the whole screen
        self.overlay = self.viewport.region(Layout.Rect(self.viewport))

        # Draw a background image
        self.background = HUD.Background(self.overlay, "calm.png")


class DockMenu(Menu):
    """Base class for menus using one Icon.Dock to hold all their items"""
    def __init__(self, book, items=[], **kwargs):
        Menu.__init__(self, book)
        self.items = items
        self.dock = Icon.Dock(self.overlay, self.trackFunction,
                              [item.icon for item in items], **kwargs)

    def finalize(self):
        # Forcibly break a few circular references that python's GC seems
        # to have trouble with, at least in version 2.2.3
        self.dock = None
        self.items = None
        self.bindings = None

    def add(self, *items):
        """Adds one or more items to the menu"""
        self.items.extend(items)
        self.dock.add(*[item.icon for item in items])

    def remove(self, *items):
        """Removes one or more items from the menu"""
        for item in items:
            self.items.remove(item)
        self.dock.remove(*[item.icon for item in items])



class RingMenu(DockMenu):
    """A circular menu, showing the current item large and near the middle of the screen"""
    keyFile = 'ringmenu_keys.py'
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

    def spinLeft(self):
        """Spin to the next icon leftwardsly"""
        self.dock.selectionIndex -= 1

    def spinRight(self):
        """Spin to the next icon rightwardsly"""
        self.dock.selectionIndex += 1

    def selectCurrent(self):
        """Select the currently largest icon and finish the menu"""
        # Round to the nearest integer index. modulo the size of our list.
        # (the index may actually be much larger or smaller than the range
        # of our item list, since the index itself doesn't wrap around.
        # This is currently done to make the animation code simpler)
        index = int(floor(self.dock.selectionIndex + 0.5) % len(self.dock.icons))
        item = self.items[index]

        item.onSelected(self)
        self.onSelected(item)
        self.onFinish()


class ArcMenu(DockMenu):
    """A menu that forms an arc along the right side of the screen, with a title shown to the left"""
    keyFile = 'arcmenu_keys.py'
    def __init__(self, book, items=[], title=None):
        DockMenu.__init__(self, book, items,
                          iconSpacing = 1
                          )
        if title:
            self.titleText = HUD.Text(self.overlay, title,
                                      fontSize = self.viewport.size[1] / 15,
                                      alignment = (0.3, 0.5),
                                      shadow = True
                                      )

    def trackFunction(self, x):
        """A track function that moves the icons along an arc on the right side of the screen.
           Icons are spaced apart by 1 unit, positive is down.
           """
        thetaExp = 1
        thetaCoeff = 0.3
        sizeExp = 2.5
        maxSize = self.viewport.size[0] * 0.15
        radius = self.viewport.size[0] * 0.7
        vCenter = self.viewport.size[1] * 0.5
        hCenter = 0

        if x>0:
            theta = pow(thetaCoeff * x, thetaExp)
        else:
            theta = -pow(-thetaCoeff * x, thetaExp)

        return (
            (hCenter + cos(theta) * radius,
             vCenter + sin(theta) * radius),
            maxSize / pow(abs(sin(theta))+1, sizeExp)
            )

    def spinUp(self):
        """Spin to the next icon upwardsly"""
        if self.dock.selectionIndex > 0:
            self.dock.selectionIndex -= 1

    def spinDown(self):
        """Spin to the next icon upwardsly"""
        if self.dock.selectionIndex < len(self.items) - 1:
            self.dock.selectionIndex += 1

    def selectCurrent(self):
        index = int(floor(self.dock.selectionIndex + 0.5))
        item = self.items[index]

        item.onSelected(self)
        self.onSelected(item)
        self.onFinish()

### The End ###

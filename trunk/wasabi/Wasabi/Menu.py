""" Wasabi.Menu

Sequencer pages implementing screens for browsing through
and selecting items.
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

from BZEngine.UI import Sequencer, Layout, HUD, Input, Viewport
from BZEngine import Event, Animated
from Wasabi import Icon, IR, Hardware
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
    """A menu item that, upon selection, pushes one or more pages onto the current book.
       'page' can be a single page, or a list of pages.
       """
    def __init__(self, icon, page):
        self.page = page
        Item.__init__(self, icon)

    def onSelected(self, menu):
        self.menu = menu
        if type(self.page) == type(()) or type(self.page) == type([]):
            for page in self.page:
                menu.book.pushBack(page)
        else:
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

        # No need to clear the viewport, we're drawing a background
        self.viewport.mode = Viewport.GL.UnclearedMode()

        # Draw a background image
        import Settings
        bgTexture = Settings.BackgroundCache().load(Settings.getCurrentBackground())
        self.background = HUD.Background(self.overlay, bgTexture)

    def cancel(self):
        """Called to cancel the menu without selecting an item"""
        self.onFinish()


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
    keyFile = 'horizontal_menu_keys.py'
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
    keyFile = 'vertical_menu_keys.py'
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

        if theta > pi:
            theta = pi
        if theta < -pi:
            theta = -pi

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
        """Spin to the next icon downwardsly"""
        if self.dock.selectionIndex < len(self.items) - 1:
            self.dock.selectionIndex += 1

    def selectCurrent(self):
        index = int(floor(self.dock.selectionIndex + 0.5))
        item = self.items[index]

        item.onSelected(self)
        self.onSelected(item)
        self.onFinish()


class VerticalMenu(DockMenu):
    """A menu that displays vertically"""
    horz = 0.5
    keyFile = 'vertical_menu_keys.py'
    def __init__(self, book, items=[]):
        DockMenu.__init__(self, book, items,
                          iconSpacing = 1
                          )

    def trackFunction(self, x):
        """A track function that moves the icons along a vertical path according to the
           horizontal position set by self.horz
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

        if theta > pi:
            theta = pi
        if theta < -pi:
            theta = -pi

        return (
            (hCenter + self.horz * self.viewport.size[0],
             vCenter + x * maxSize * 1.5),
            maxSize / pow(abs(sin(theta))+1, sizeExp)
            )

    def spinUp(self):
        """Spin to the next icon upwardsly"""
        if self.dock.selectionIndex > 0:
            self.dock.selectionIndex -= 1

    def spinDown(self):
        """Spin to the next icon downwardsly"""
        if self.dock.selectionIndex < len(self.items) - 1:
            self.dock.selectionIndex += 1

    def selectCurrent(self):
        index = int(floor(self.dock.selectionIndex + 0.5))
        item = self.items[index]

        item.onSelected(self)
        self.onSelected(item)
        self.onFinish()

class ThreeColumnMenu(Menu):
    """A 3-column display"""
    keyFile = 'vertical_menu_keys.py'
    def __init__(self, book, items=[]):
        Menu.__init__(self, book)

        self.left = VerticalMenu(book, items)
        self.left.horz = 0.2
        self.left.bindings = None
        self.left.background = None
        self.center = VerticalMenu(book, items)
        self.center.horz = 0.5
        self.center.bindings = None
        self.center.background = None

    def spinUp(self):
        self.center.spinUp()

    def spinDown(self):
        self.center.spinDown()

    def selectCurrent(self):
        self.center.selectCurrent()

def defaultFades(page):
    """A page wrapper that applies a default fade in from and out to black"""
    return Sequencer.FadeIn(0.25, (0,0,0), Sequencer.FadeOut(0.25, (0,0,0), page))


def userPageInterrupter(page):
    """A page wrapper that interrupts the page when any of several keys are pressed.
       This is like the one included in Sequencer, but IR-aware.
       """
    def f(book):
        events = [
            Input.KeyPress(book.viewport, pygame.K_SPACE),
            Input.KeyPress(book.viewport, pygame.K_RETURN),
            Input.MousePress(book.viewport, 1),
            IR.ButtonPress(book.viewport, 'enter'),
            IR.ButtonPress(book.viewport, 'up'),
            IR.ButtonPress(book.viewport, 'down'),
            IR.ButtonPress(book.viewport, 'left'),
            IR.ButtonPress(book.viewport, 'right'),
            IR.ButtonPress(book.viewport, 'play'),
            IR.ButtonPress(book.viewport, 'stop'),
            ]
        return Sequencer.PageInterrupter(events, page)(book)
    return f


class LoaderPage(Sequencer.Page):
    """A page that displays a 'Loading' message while performing
       some time-consuming function.
       """
    def __init__(self, book):
        Sequencer.Page.__init__(self, book)
        Hardware.dev.showStatus("Loading...")

        # We need the viewport to clear our background to black
        self.viewport.mode = Viewport.GL.ClearedMode()

        self.message = HUD.Text(self.viewport.region(self.viewport.rect),
                              "Loading...",
                              color     = (1,1,1,1),
                              fontSize  = 50,
                              alignment = (0.5, 0.5))

        self.frameCounter = 0
        self.viewport.onSetupFrame.observe(self.setupFrame)

    def setupFrame(self):
        self.frameCounter += 1

        # Actually do the loading after one complete frame, so our message is displayed
        if self.frameCounter == 2:
            self.run()

            # Absorb the gigantic bubble we've just put in the TimeMaster so that it doesn't
            # cause animations the next frame to jump wildly.
            Animated.currentTimeMaster.jump()

            # Terminate this page
            self.onFinish()

    def run(self):
        """Hook for subclasses to add the actual loading functionality"""
        pass


class ImageMenu(DockMenu):
    """A menu that shows the current item very large, and all others in a row along the bottom of the screen"""
    keyFile = 'horizontal_menu_keys.py'
    def __init__(self, book, items=[]):
        DockMenu.__init__(self, book, items,
                          iconSpacing = 1
                          )

    def trackFunction(self, x):
        """A track function that moves the icons along an arc on the right side of the screen.
           Icons are spaced apart by 1 unit, positive is down.
           """
        rowY = self.viewport.size[1] * 0.85
        rowSize = self.viewport.size[1] * 0.14
        centerX = self.viewport.size[0] * 0.5
        centerY = self.viewport.size[1] * 0.4
        centerSize = self.viewport.size[1] * 0.6
        rowSpacing = rowSize * 1.6

        # i is an interpolation amount- the center icon will be near 0,
        # all other icons will be near 1. The states for the center icons
        # and icons i the bottom row are calculated independently, then
        # interpolated together
        i = abs(x)
        if i > 1:
            i = 1
        return (
            (self.lerp(centerX, centerX + rowSpacing * x, i),
             self.lerp(centerY, rowY, i)),
            self.lerp(centerSize, rowSize, i))

    def lerp(self, a, b, i):
        """Linear interpolation between a and b with amount i"""
        return a*(1-i) + b * i

    def spinLeft(self):
        if self.dock.selectionIndex > 0:
            self.dock.selectionIndex -= 1

    def spinRight(self):
        if self.dock.selectionIndex < len(self.items) - 1:
            self.dock.selectionIndex += 1

    def selectCurrent(self):
        index = int(floor(self.dock.selectionIndex + 0.5))
        item = self.items[index]

        item.onSelected(self)
        self.onSelected(item)
        self.onFinish()

### The End ###

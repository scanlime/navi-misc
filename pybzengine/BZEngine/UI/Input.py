""" BZEngine.UI.Input

Contains events and other utilities for setting up mouse and key bindings.
The classes here are used within a bindings file, which is loaded into
a Bindings class.
"""
#
# Python BZEngine Package
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

import pygame
from BZEngine import Event, Util


class Modifiers:
    """Represents a set of modifier keys. The modifer can be zero or more
       pygame.KMOD_* constants or'ed together, None, or 'any'.
       """
    def __init__(self, modifiers):
        if modifiers is None:
            modifiers = 0
        if modifiers == 'any':
            self.requireMods = 0
            self.disallowMods = 0
        else:
            self.requireMods = modifiers
            self.disallowMods = (~modifiers) & (pygame.KMOD_SHIFT | pygame.KMOD_CTRL |
                                                pygame.KMOD_ALT | pygame.KMOD_META)

    def test(self):
        """Return true if the currently pressed modifiers fit this class' criteria"""
        mods = pygame.key.get_mods()
        if mods & self.disallowMods:
            return False
        if mods & self.requireMods or not self.requireMods:
            return True
        return False


class MouseDrag(Event.Event):
    """Binds a mouse drag action to a function, with optional modifier keys.
       The function is called with X and Y relative motion as its two parameters.
       Buttons are numbered starting at 1.
       """
    def __init__(self, viewport, button, modifiers=None, scale=1):
        Event.Event.__init__(self)
        self.button = button
        self.scale = scale
        self.modifiers = Modifiers(modifiers)
        viewport.onMouseMotion.observe(self.handleMotion)

    def handleMotion(self, event):
        if event.buttons[self.button-1] and self.modifiers.test():
                self(event.rel[0] * self.scale, event.rel[1] * self.scale)


class MouseGrab:
    """When the mouse is dragged with the specified button, hide it and
       snap it back to its initial position. This makes it much more
       intuitive to perform drags that don't correspond directly to
       viewport coordinates. Since this doesn't perform an actual mouse grab,
       if the software hangs for any reason, the mouse will not be trapped.
       Buttons are numbered starting at 1.
       Note that this class is not an event, as it doesn't produce any
       output, just modifies the mouse's visibility and position. Use it
       in combination with a MouseDrag event.
       """
    def __init__(self, viewport, button):
        self.button = button
        viewport.onMouseMotion.observe(self.handleMotion)
        viewport.onMouseButtonDown.observe(self.handlePress)
        viewport.onMouseButtonUp.observe(self.handleRelease)
        self.homePos = None

    def handlePress(self, event):
        if event.button == self.button:
            pygame.mouse.set_visible(False)
            self.homePos = event.pos

    def handleRelease(self, event):
        if event.button == self.button:
            pygame.mouse.set_visible(True)
            self.homePos = None

    def handleMotion(self, event):
        if self.homePos:
            # Snap the mouse back to the position it was clicked at.
            # This generates an event, so we have to rummage through the event
            # queue and discard it, putting all the other events back.
            pygame.mouse.set_pos(*self.homePos)
            for newEvent in pygame.event.get([pygame.MOUSEMOTION]):
                if newEvent.pos != self.homePos:
                    pygame.event.post(newEvent)


class KeyPress(Event.Event):
    """An event that fires when a specified key is pressed. Either a
       string or a key constant can be given.
       """
    def __init__(self, viewport, key, modifiers=None):
        Event.Event.__init__(self)
        self.key = key
        self.modifiers = Modifiers(modifiers)
        viewport.onKeyDown.observe(self.handlePress)

    def handlePress(self, event):
        if type(self.key) == str or type(self.key) == unicode:
            if event.unicode == self.key and self.modifiers.test():
                self()
        else:
            if event.key == self.key and self.modifiers.test():
                self()


class KeyAxis(Event.Event):
    """A combination of two keys that simulate a scalar axis"""
    def __init__(self, viewport, keyPlus, keyMinus, scale=1, modifiers=None):
        Event.Event.__init__(self)
        self.scale = scale
        self.keyPlus = KeyPress(viewport, keyPlus, modifiers)
        self.keyMinus = KeyPress(viewport, keyMinus, modifiers)
        self.keyPlus.observe(self.increment)
        self.keyMinus.observe(self.decrement)

    def increment(self):
        self(self.scale)

    def decrement(self):
        self(-self.scale)


class MousePress(Event.Event):
    """An event that fires when a specified mouse button is pressed"""
    def __init__(self, viewport, button, modifiers=None):
        Event.Event.__init__(self)
        self.button = button
        self.modifiers = Modifiers(modifiers)
        viewport.onMouseButtonDown.observe(self.handleButtonDown)

    def handleButtonDown(self, event):
        if event.button == self.button and self.modifiers.test():
            self()


class MouseWheel(Event.Event):
    """An event that fires when the mous wheel is pressed.
       The event handler gets a positive or negative copy of
       'scale' depending on the direction.
       """
    def __init__(self, viewport, scale=1, modifiers=None):
        Event.Event.__init__(self)
        self.scale = scale
        self.modifiers = Modifiers(modifiers)
        viewport.onMouseButtonDown.observe(self.handleWheel)

    def handleWheel(self, event):
        if event.button == 4 and self.modifiers.test():
            self(self.scale)
        elif event.button == 5 and self.modifiers.test():
            self(-self.scale)


class Bindings:
    """A container for key bindings."""
    def __init__(self, viewport):
        self.viewport = viewport
        self.list = []

    def load(self, fileObject, commandObject):
        """Load the key bindings from the given open file object.
           A key bindings file is python code executed with this module
           in its namespace, along with this bindings instance as 'bindings'
           and the commandObject as 'command'.
           """
        # Set up the execution namespace
        ns = globals()
        ns['bindings'] = self
        ns['command'] = commandObject

        exec fileObject in ns

    def add(self, cls, *args, **kw):
        """Set up a key or mouse binding using the given class. Returns the class
           instance ready for binding to an observer, and stores the event for later
           editing.
           This is generally used from inside the file executed by load()
           """
        binding = cls(self.viewport, *args, **kw)
        self.list.append(binding)
        return binding


def load(viewport, commandObject, dataFile):
    """Load bindings from the given datafile name, associated to commands in the given
       command object. Return the resulting Bindings object.
       """
    b = Bindings(viewport)
    b.load(open(Util.dataFile(dataFile)), commandObject)
    return b

### The End ###

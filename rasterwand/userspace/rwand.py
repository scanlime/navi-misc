#
# Lightweight Python interface for the Raster Wand, based on the
# userspace-only 'rwd' driver. This includes animated transitions,
# and a simple menu system.
#
# Copyright(c) 2004-2007 Micah Dowty <micah@navi.cx>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#

import os, re, time, popen2, fcntl
import binascii, random, threading, ConfigParser


class RwdClient(threading.Thread):
    """Client for communicating to a rasterwand via a spawned rwd
       process.  This is a base class which knows how to keep the
       connection alive, automatically locate the device, render
       frames synchronously, and keep button/setting state.

       Application-specific functionality, such as handling button
       presses and deciding what to draw, is governed by other objects
       that are attached to the RwdClient in a stateless way: the
       Renderer and list of KeyListeners. These may be swapped freely at
       runtime.

       Listeners are arranged in a stack: those at the end of the
       list (top of the stack) get first priority on incoming events.
       """
    _connectPollInterval = 0.5
    _lastButtonTimestamp = 0
    model = None
    rwd = None
    
    def __init__(self, renderer=None, listeners=None,
                 devicePath=None, rwdPath=None):
        threading.Thread.__init__(self)

        self.renderer = renderer
        self.listeners = listeners or []
        self.devicePath = devicePath
        self.rwdPath = rwdPath
        self.settings = {}
        self.buttons = set()

    def _getConfigFilePath(self):
        return os.path.join(os.getenv("HOME"), ".rwandrc")

    def loadSettings(self):
        """If the user has a .rwandrc file in their home directory,
           this loads all model-specific settings from it.
           """
        cp = ConfigParser.RawConfigParser()
        cp.read(self._getConfigFilePath())
        if cp.has_section(self.model):
            for option in cp.options(self.model):
                self.send_setting(option, cp.getint(self.model, option))

    def saveSettings(self):
        """Persist all current model-specific settings to a
           dotfile (.rwandrc) in the user's home directory.
           """
        cp = ConfigParser.RawConfigParser()
        cp.read(self._getConfigFilePath())
        if not cp.has_section(self.model):
            cp.add_section(self.model)

        for name, value in self.settings.iteritems():
            cp.set(self.model, name, str(value))

        cp.write(open(self._getConfigFilePath(), 'w'))

    def run(self):
        """poll() this client in an infinite loop."""
        self._running = True
        while self._running:
            self.poll()

    def stop(self):
        """Stop the current run() loop"""
        self._running = False

    def _findDevice(self, usbfsPath="/proc/bus/usb"):
        """Return the usbfs path of the first attached Raster Wand,
           or None if no devices can be found.
           """
        for line in open(os.path.join(usbfsPath, 'devices')):

            if line.startswith('T:'):
                bus = int(re.search(r"Bus= *(\d+)", line).group(1))
                dev = int(re.search(r"Dev#= *(\d+)", line).group(1))

            if line.startswith('P:  Vendor=e461 ProdID=0005'):
                return os.path.join(usbfsPath, "%03d" % bus, "%03d" % dev)
           
    def connect(self, devicePath=None, rwdPath=None):
        """Start a new rwd process to connect to a particular Raster
           Wand device. Both the device path and the path to rwd can
           be auto-detected if they aren't specified. If no device
           is available, this will set self.rwd to None.
           """
        rwdPath = rwdPath or os.path.join(os.path.dirname(__file__), 'rwd')
        devicePath = devicePath or self._findDevice()

        if os.sep not in rwdPath:
            # Need an explicit ./ if rwd is in the current directory
            rwdPath = "./%s" % rwdPath

        if devicePath:
            self.rwd = popen2.Popen3((rwdPath, devicePath),
                                     capturestderr=False)
            self.connected()
        else:
            self.rwd = None

    def getInactivityTime(self):
        return time.time() - self._lastButtonTimestamp

    def send_command(self, tokens):
        try:
            self.rwd.tochild.write(' '.join(tokens))
            self.rwd.tochild.write('\n')
            self.rwd.tochild.flush()
        except IOError:
            self.rwd = None

    def send_frame(self, data):
        self.send_command(('frame', binascii.b2a_hex(data)))

    def send_setting(self, name, value):
        self.settings[name] = value
        self.send_command(('setting', name, str(value)))

    def connected(self):
        """We just connected successfully to rwd. The default
           implementation just bootstraps our frame rendering
           loop by invoking recv_frame_ack().
           """
        self.recv_frame_ack()

    def poll(self):
        """Blocks until we have a connection to the rwd daemon, then
           blocks until we've received output from the daemon. Process
           any responses we receive.
           """
        # Disconnect if rwd died
        if self.rwd and self.rwd.poll() != -1:
            time.sleep(self._connectPollInterval)
            self.rwd = None

        # Try to reconnect if we need to
        while not self.rwd:
            self.connect()
            if not self.rwd:
                time.sleep(self._connectPollInterval)

        # Wait for output from rwd
        line = self.rwd.fromchild.readline()
        if line:
            # Dispatch this response to a handler
            tokens = line.split()
            f = getattr(self, 'recv_' + tokens[0], None)
            if f:
                f(tokens)
        else:
            self.rwd = None

    def recv_model(self, tokens):
        self.model = tokens[1]

    def recv_end_settings(self, tokens):
        """rwd just finished sending us the default settings.
           Now we can override some of those with our own saved settings.
           """
        self.loadSettings()

    def recv_setting(self, tokens):
        self.settings[tokens[1]] = int(tokens[2])

    def recv_buttons(self, tokens):
        new_buttons = set(tokens[1:])

        for pressed in new_buttons.difference(self.buttons):
            self._lastButtonTimestamp = time.time()
            for listener in reversed(self.listeners):
                if listener.keyPress(self, pressed):
                    break

        for released in self.buttons.difference(new_buttons):
            for listener in reversed(self.listeners):
                if listener.keyRelease(self, released):
                    break

        self.buttons = new_buttons

    def recv_frame_ack(self, tokens=None):
        for listener in reversed(self.listeners):
            if listener.pollKeys(self, self.buttons):
                break

        self.send_frame(self.renderer and self.renderer.render(self) or '')


class Renderer:
    """Abstract base class for a RwdClient renderer."""
    def render(self, rwdc):
        return ''


class KeyListener:
    """Abstract base class for a RwdClient key event listener.
       The default implementation dispatches key presses to
       a member function, and ignores releases.

       Handlers should return True to absorb an event,
       preventing it from reaching other KeyListeners.
       """
    def keyPress(self, rwdc, button):
       f = getattr(self, 'press_' + button, None)
       if f:
           return f(rwdc)

    def keyRelease(self, rwdc, button):
        pass

    def pollKeys(self, rwdc, buttons):
        """Called once per frame with a set of all buttons that are down"""
        pass

    def activate(self, rwdc):
        rwdc.listeners.append(self)

    def deactivate(self, rwdc):
        rwdc.listeners.remove(self)


class Font:
    """An 8-pixel font for the Raster Wand, represented as a
       dictionary mapping characters to binary strings.
       """
    def __init__(self, data, spacing='\0', missing=u'\ufffd'):
        self.data = data
        self.spacing = spacing
        self.default = data[missing]

    def render(self, str):
        return self.spacing.join([self.data.get(c, self.default) for c in str])


class TextRenderer(Renderer):
    """Renderer which displays a static string of text, rendered with
       a particular font. Supports padding short strings, and scrolling
       long ones.
       """
    defaultFont = Font({
        #
        # ASCII character set, from the 8-pixel 04B_03 font. Converted with genfont.py
        #
        'a': '\x18$$<', 'b': '>$$\x18', 'c': '\x18$$',
        'd': '\x18$$>', 'e': '\x184,\x08', 'f': '\x08<\x0a', 'g':
        '\x18\xa4\xa4|', 'h': '>\x04\x048', 'i': ':', 'j': '\x80z', 'k':
        '>\x10\x18$', 'l': '>', 'm': '<\x04<\x048', 'n': '<\x04\x048',
        'o': '\x18$$\x18', 'p': '\xfc$$\x18', 'q': '\x18$$\xfc', 'r':
        '<\x08\x04', 's': '(,4\x14', 't': '\x04\x1e$', 'u':
        '\x1c\x20\x20<', 'v': '\x1c\x20\x10\x0c', 'w': '\x0c0\x0c0\x0c',
        'x': '$\x18$', 'y': '\x1c\xa0\xa0|', 'z': '$4,$', 'A':
        '<\x12\x12<', 'B': '>**\x14', 'C': '\x1c\x22\x22', 'D':
        '>\x22\x22\x1c', 'E': '>**', 'F': '>\x0a\x0a', 'G': '\x1c\x22*:',
        'H': '>\x08\x08>', 'I': '\x22>\x22', 'J': '\x10\x20\x22\x1e', 'K':
        '>\x08\x14\x22', 'L': '>\x20\x20', 'M': '>\x04\x08\x04>', 'N':
        '>\x04\x08>', 'O': '\x1c\x22\x22\x1c', 'P': '>\x12\x12\x0c', 'Q':
        '\x1c\x22\x22\x5c', 'R': '>\x12\x12,', 'S': '$**\x12', 'T':
        '\x02>\x02', 'U': '\x1e\x20\x20\x1e', 'V': '\x1e\x20\x18\x06',
        'W': '\x1e\x20\x1c\x20\x1e', 'X': '6\x08\x086', 'Y': '\x06((\x1e',
        'Z': '2*&', '!': '.', '"': '\x06\x00\x06', '#': '\x14>\x14>\x14',
        '$': '(,v\x14', '%': '\x020\x08\x06\x20', '&': '\x14**\x10(',
        '\'': '\x06', '(': '\x1c\x22', ')': '\x22\x1c', '*':
        '\x0a\x04\x0a', '+': '\x08\x1c\x08', ',': '@\x20', '-':
        '\x08\x08\x08', '.': '\x20', '/': '\x20\x10\x08\x04\x02', ':':
        '\x14', ';': '4', '<': '\x08\x14\x22', '=': '\x14\x14\x14', '>':
        '\x22\x14\x08', '?': '\x02*\x0a\x04', '@': '\x1c\x22:*\x1c', '[':
        '>\x22', '\\': '\x02\x04\x08\x10\x20', ']': '\x22>', '^':
        '\x04\x02\x04', '_': '\x20\x20\x20\x20', '`': '\x02\x04', '{':
        '\x086\x22', '|': '>', '}': '\x226\x08', '~': '\x04\x02\x04\x02',
        '0': '\x1c\x22\x22\x1c', '1': '\x02>', '2': '2**$', '3':
        '\x22**\x14', '4': '\x18\x14>\x10', '5': '.**\x12', '6':
        '\x1c**\x10', '7': '\x022\x0a\x06', '8': '\x14**\x14', '9':
        '\x04**\x1c',

        #
        # Miscellaneous whitespace and Unicode characters, added by hand
        #
        ' ': '\x00',
        u'\u2665': '\x0e\x3f\x7e\xfc\x7e\x3f\x0e',      # Heart
        u'\u2190': '\x08\x1c\x2a\x08\x08',              # Left arrow
        u'\u2191': '\x08\x04\x3e\x04\x08',              # Up arrow
        u'\u2192': '\x08\x08\x2a\x1c\x08',              # Right arrow
        u'\u2193': '\x08\x10\x3e\x10\x08',              # Down arrow
        u'\u231A': '\x7e\x81\x81\x9d\x91\x81\x7e',      # Watch
        u'\u2301': '\x08\x10\x3c\x08\x10',              # Electric Arrow
        u'\u263A': '\x7e\x81\x95\xa1\xa1\x95\x81\x7e',  # White smiling face
        u'\u25a1': '\xff\x81\x81\xff',                  # White square
        u'\u266b': '\x30\x30\x3f\x01\xc2\xc4\xfc',      # Beamed eighth notes
        u'\ufffd': '\x7f\x7d\x55\x75\x7b\x7f',          # Replacement character (inverted '?')
        })

    def __init__(self, str, font=None, min_width=60, max_width=80, gap=8):
        # Precompute a buffer from which we'll take slices, in order
        # to implement scrolling. This buffer will have two copies
        # of the original string, separated by some space. 

        rendered = (font or self.defaultFont).render(str)

        # Shorter than min_width: pad the string, use a minimum-sized
        # scrolling window.
        if min_width > len(rendered):
            self._frame = rendered.center(min_width, '\0')
            self._wrap = 1
            self._window = min_width

        # Longer than max_width: Duplicate the string, and set up a
        # scrolling window.
        elif max_width < len(rendered):
            self._frame = rendered + ('\0' * gap) + rendered
            self._wrap = len(rendered) + gap
            self._window = max_width

        # The string is already a good size
        else:
            self._frame = rendered
            self._wrap = 1
            self._window = len(rendered)

        self.resetScrolling()

    def resetScrolling(self):
        self._scroll = 0
        self._delay = 16

    def render(self, rwdc):
        frame = self._frame[self._scroll:self._scroll + self._window]
        if self._delay:
            self._delay -= 1
        else:
            self._scroll = (self._scroll + 1) % self._wrap
        return frame
    


class Transition(Renderer):
    """Abstract base class for transitions that operate on the output
       of two distinct renderers. When the transition completes, the old
       renderer is discarded and the new one is installed directly into
       the RwdClient.
       """
    def __init__(self, fromRenderer, toRenderer, duration):
        self.fromRenderer = fromRenderer
        self.toRenderer = toRenderer
        self.duration = duration
        self.frameNum = 0

    def tween(self, rwdc, fromFrame, toFrame):
        """Render the next in-between frame, given two input frames."""
        raise NotImplementedError()

    def render(self, rwdc):
        if self.frameNum >= self.duration:
            if rwdc.renderer is self:
                rwdc.renderer = self.toRenderer
            return self.toRenderer.render(rwdc)

        if self.fromRenderer:
            fromFrame = self.fromRenderer.render(rwdc)
        else:
            fromFrame = ''
        toFrame = self.toRenderer.render(rwdc)

        # Pad the two input frames to identical widths
        if len(fromFrame) > len(toFrame):
            toFrame = toFrame.center(len(fromFrame), '\0')
        if len(toFrame) > len(fromFrame):
            fromFrame = fromFrame.center(len(toFrame), '\0')
        
        frame = self.tween(rwdc, fromFrame, toFrame)
        self.frameNum += 1
        return frame


class VisualBell(Renderer):
    """Renders a bright single-frame flash, then reverts back to
       another renderer. This signals invalid key input.
       """
    def __init__(self, toRenderer):
        self.toRenderer = toRenderer

    def render(self, rwdc):
        rwdc.renderer = self.toRenderer
        return '\xFF' * len(self.toRenderer.render(rwdc))


class Scroll(Transition):
    """Vertical/horizontal scrolling transition"""
    UP, DOWN, LEFT, RIGHT = range(4)
    
    def __init__(self, fromRenderer, toRenderer, dir=UP, duration=8):
        self.dir = dir
        Transition.__init__(self, fromRenderer, toRenderer, duration)
    
    def tween(self, rwdc, fromFrame, toFrame):

        if self.dir == self.UP:
            shift = self.frameNum * 8 / self.duration
            return ''.join([ chr(0xFF & ((ord(a) >> shift) |
                                         (ord(b) << (8-shift))))
                             for a, b in zip(fromFrame, toFrame) ])

        if self.dir == self.DOWN:
            shift = self.frameNum * 8 / self.duration
            return ''.join([ chr(0xFF & ((ord(a) << shift) |
                                         (ord(b) >> (8-shift))))
                             for a, b in zip(fromFrame, toFrame) ])

        if self.dir == self.LEFT:
            shift = self.frameNum * len(toFrame) / self.duration
            return (fromFrame + toFrame)[shift:shift+len(toFrame)]

        if self.dir == self.RIGHT:
            shift = len(toFrame) - self.frameNum * len(toFrame) / self.duration
            return (toFrame + fromFrame)[shift:shift+len(toFrame)]


class Dissolve(Transition):
    """Pseudo-random dissolve transition, based on an 8x8 repeating
       stencil.  On instantiation, this creates a shuffled list of bit
       masks representing each pixel in the stencil. Based on our
       current frame number, those masks are used to update our
       current stencil pattern. Finally, the stencil is used to blend
       the two input frames.
       """
    def __init__(self, fromRenderer, toRenderer, duration=16):
        self.stencil = [0] * 8
        self.pixelIndex = 0

        self.pixels = []
        for y in range(8):
            mask = 1 << y
            for x in range(8):
                self.pixels.append((x, mask))
        random.shuffle(self.pixels)

        Transition.__init__(self, fromRenderer, toRenderer, duration)

    def tween(self, rwdc, fromFrame, toFrame):
        newPixelIndex = self.frameNum * len(self.pixels) / self.duration
        while self.pixelIndex < newPixelIndex:
            x, mask = self.pixels[self.pixelIndex]
            self.stencil[x] |= mask
            self.pixelIndex += 1

        b = []
        for i in range(len(fromFrame)):
            mask = self.stencil[i & 7]
            b.append(chr( (ord(fromFrame[i]) & ~mask) |
                          (ord(toFrame[i]) & mask) ))
        return ''.join(b)


class MenuItem(KeyListener):
    def __init__(self, renderer):
        self.renderer = renderer


class TextMenuItem(MenuItem):
    def __init__(self, text):
        MenuItem.__init__(self, TextRenderer(text))

    def activate(self, rwdc):
        self.renderer.resetScrolling()
        MenuItem.activate(self, rwdc)


class MenuList(KeyListener):
    def __init__(self, items, root=False):
        self.items = items
        self.root = root

    def activate(self, rwdc):
        """Enter this menu, temporarily or permanently displacing
           whichever menu was previously active.  If this is a root
           menu, it permanently replaces all other menus in the
           current stack.
           """
        if self.root:
            del rwdc.listeners[:]
        
        self._previousRenderer = rwdc.renderer
        self.index = 0
        item = self.items[self.index]        
        KeyListener.activate(self, rwdc)
        item.activate(rwdc)

        if self.root:
            rwdc.renderer = Dissolve(rwdc.renderer, item.renderer)
        else:
            rwdc.renderer = Scroll(rwdc.renderer, item.renderer, Scroll.LEFT)

    def deactivate(self, rwdc):
        """Exit this menu, returning to the previous one"""
        item = self.items[self.index]
        item.deactivate(rwdc)
        KeyListener.deactivate(self, rwdc)
        rwdc.renderer = Scroll(rwdc.renderer, self._previousRenderer, Scroll.RIGHT)
        self._previousRenderer = None
        self.index = None

    def select(self, rwdc, index):
        """Select a new item, given its index. The menu is a torus. Out-of-range
           indices will wrap around. This automatically picks a transition, based
           on the old and new indices.
           """
        wrappedIndex = index % len(self.items)
        if wrappedIndex == self.index:
            # No items to scroll to
            rwdc.renderer = VisualBell(rwdc.renderer)

        else:
            self.items[self.index].deactivate(rwdc)
            item = self.items[wrappedIndex]

            if index > self.index:
                rwdc.renderer = Scroll(rwdc.renderer, item.renderer, Scroll.UP)
            else:
                rwdc.renderer = Scroll(rwdc.renderer, item.renderer, Scroll.DOWN)

            self.index = wrappedIndex
            self.items[self.index].activate(rwdc)

    def press_down(self, rwdc):
        self.select(rwdc, self.index + 1)
        return True

    def press_up(self, rwdc):
        self.select(rwdc, self.index - 1)
        return True

    def press_left(self, rwdc):
        if self.root:
            rwdc.renderer = VisualBell(rwdc.renderer)
        else:
            self.deactivate(rwdc)
        return True

    def press_right(self, rwdc):
        rwdc.renderer = VisualBell(rwdc.renderer)
        return True

    def press_select(self, rwdc):
        rwdc.renderer = VisualBell(rwdc.renderer)
        return True


class AutoMenuList(MenuList):
    """A menu that automatically scrolls at a fixed rate
       when the controls have been inactive for a while.
       """
    def __init__(self, items, root=False,
                 inactivityDelay = 15,
                 autoScrollDelay = 4,
                 ):
        MenuList.__init__(self, items, root)
        self.inactivityDelay = inactivityDelay
        self.autoScrollDelay = autoScrollDelay
        self._lastScrollTimestamp = time.time()

    def pollKeys(self, rwdc, buttons):
        if rwdc.getInactivityTime() > self.inactivityDelay:
            now = time.time()
            if now >= self._lastScrollTimestamp + self.autoScrollDelay:
                self.select(rwdc, self.index + 1)
                self._lastScrollTimestamp = now


class SubMenuItem(TextMenuItem):
    """A MenuItem that enters a submenu"""
    def __init__(self, text, menu):
        TextMenuItem.__init__(self, u"%s \u2192" % text)
        self.menu = menu

    def press_right(self, rwdc):
        self.menu.activate(rwdc)
        return True

    def press_select(self, rwdc):
        self.menu.activate(rwdc)
        return True


class SettingMenuItem(TextMenuItem):
    """A menu item that, while active, lets the user adjust Raster Wand settings"""
    def __init__(self, text, setting, increment,
                 minimum=0, maximum=0xFFFF):
        self.text = text
        TextMenuItem.__init__(self, u"\u2190 %s \u2192" % text)
        self.setting = setting
        self.increment = increment
        self.minimum = minimum
        self.maximum = maximum

    def activate(self, rwdc):
        TextMenuItem.activate(self, rwdc)

        # Even though we take action during pollKeys, we make
        # sure a corresponding 'press' event has been received.
        # This is important when a button was already down when
        # this listener became active. The 'right' button used
        # to enter this menu item should not also cause a setting
        # adjustment to be made.
        self.pressed = set()

    def deactivate(self, rwdc):
        TextMenuItem.deactivate(self, rwdc)
        rwdc.saveSettings()

    def press_left(self, rwdc):
        self.pressed.add('left')
        return True

    def press_right(self, rwdc):
        self.pressed.add('right')
        return True

    def pollKeys(self, rwdc, buttons):
        if rwdc.settings:
            value = rwdc.settings[self.setting]

            buttons = buttons.intersection(self.pressed)
            if 'left' in buttons:
                value -= self.increment
            if 'right' in buttons:
                value += self.increment

            value = min(self.maximum, max(self.minimum, value))
            if value != rwdc.settings[self.setting]:
                rwdc.renderer = TextRenderer(u"%s: %d" % (self.text, value / self.increment))
                rwdc.send_setting(self.setting, value)
                return True


class SettingsMenu(MenuList):
    def __init__(self):
        MenuList.__init__(self, [
            SettingMenuItem("Position", "display_center", 100),
            SettingMenuItem("Width", "display_width", 100),
            SettingMenuItem("Bright", "duty_cycle", 1000),
            SettingMenuItem("Wiggle", "fine_adjust", 1, minimum=-200, maximum=200),
            SettingMenuItem("Power", "coil_width", 100, maximum=0x8000),
            TextMenuItem(u"\u2190 Back"),
            ])

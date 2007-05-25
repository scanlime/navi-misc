#
# Lightweight Python interface for the Raster Wand, based on the
# userspace-only 'rwd' driver.
#
# Copyright(c) 2004-2007 Micah Dowty <micah@picogui.org>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#

import os, re, time, popen2, fcntl, binascii, random


class RwdClient:
    """Client for communicating to a rasterwand via a spawned rwd
       process.  This is a base class which knows how to keep the
       connection alive, automatically locate the device, render
       frames synchronously, and keep button/setting state.

       Application-specific functionality, such as handling button
       presses and deciding what to draw, is governed by other objects
       that are attached to the RwdClient in a stateless way: the
       Renderer and KeyListener. These may be swapped freely at
       runtime.
       """
    def __init__(self, renderer=None, keyListener=None,
                 devicePath=None, rwdPath=None):
        self.renderer = renderer
        self.keyListener = keyListener
        self.devicePath = devicePath
        self.rwdPath = rwdPath
        self.rwd = None
        self.settings = {}
        self.buttons = set()

    def run(self):
        """poll() this client in an infinite loop."""
        while True:
            self.poll()

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
            self.rwd = popen2.Popen3((rwdPath, devicePath), capturestderr=False)
            self.connected()
        else:
            self.rwd = None

    def send_command(self, tokens):
        self.rwd.tochild.write(' '.join(tokens))
        self.rwd.tochild.write('\n')
        self.rwd.tochild.flush()

    def send_frame(self, data, min_width=50):
        if min_width and min_width > len(data):
            # Optionally, pad the framebuffer up to a minimum width
            data = data.center(min_width, '\0')

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
            time.sleep(1)
            self.rwd = None

        # Try to reconnect if we need to
        while not self.rwd:
            self.connect()
            if not self.rwd:
                time.sleep(1)

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

    def recv_setting(self, tokens):
        self.settings[tokens[1]] = int(tokens[2])

    def recv_buttons(self, tokens):
        new_buttons = set(tokens[1:])
        if self.keyListener:
            for pressed in new_buttons.difference(self.buttons):
                self.keyListener.keyPress(self, pressed)
            for released in self.buttons.difference(new_buttons):
                self.keyListener.keyRelease(self, released)        
        self.buttons = new_buttons

    def recv_frame_ack(self, tokens=None):
        self.send_frame(self.renderer and self.renderer.render(self) or '')


class Renderer:
    """Abstract base class for a RwdClient renderer."""
    def render(self, rwdc):
        return ''


class KeyListener:
    """Abstract base class for a RwdClient key event listener.
       The default implementation dispatches key presses to
       a member function, and ignores releases.
       """
    def keyPress(self, rwdc, button):
       f = getattr(self, 'press_' + button, None)
       if f:
           f(rwdc)

    def keyRelease(self, rwdc, button):
        pass


class Font:
    """An 8-pixel font for the Raster Wand, represented as a
       dictionary mapping characters to binary strings.
       """
    def __init__(self, data, spacing='\0', default=None):
        self.data = data
        self.spacing = spacing
        self.default = default or data['?']

    def render(self, str):
        return self.spacing.join([self.data.get(c, self.default) for c in str])


class TextRenderer:
    """Renderer which displays a static string of text, rendered with
       a particular font.
       """
    defaultFont = Font({ 'a': '\x18$$<', 'b': '>$$\x18', 'c': '\x18$$',
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
                         '\x04**\x1c', })

    def __init__(self, str, font=None):
        self.frame = (font or self.defaultFont).render(str)

    def render(self, rwdc):
        return self.frame


class Transition:
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

        fromFrame = self.fromRenderer.render(rwdc)
        toFrame = self.toRenderer.render(rwdc)

        # Pad the two input frames to identical widths
        if len(fromFrame) > len(toFrame):
            toFrame = toFrame.center(len(fromFrame), '\0')
        if len(toFrame) > len(fromFrame):
            fromFrame = fromFrame.center(len(toFrame), '\0')
        
        frame = self.tween(rwdc, fromFrame, toFrame)
        self.frameNum += 1
        return frame


class VScroll(Transition):
    """Vertical scrolling transition"""
    UP, DOWN = range(2)
    
    def __init__(self, fromRenderer, toRenderer, dir=UP, duration=8):
        self.dir = dir
        Transition.__init__(self, fromRenderer, toRenderer, duration)
    
    def tween(self, rwdc, fromFrame, toFrame):
        shift = self.frameNum * 8 / self.duration

        if self.dir == self.UP:
            return ''.join([ chr(0xFF & ((ord(a) >> shift) |
                                         (ord(b) << (8-shift))))
                             for a, b in zip(fromFrame, toFrame) ])
        elif self.dir == self.DOWN:
            return ''.join([ chr(0xFF & ((ord(a) << shift) |
                                         (ord(b) >> (8-shift))))
                             for a, b in zip(fromFrame, toFrame) ])


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
    

class MyListener(KeyListener):
    def press_left(self, rwdc):
        rwdc.send_setting('display_center', rwdc.settings['display_center'] - 1000)

    def press_right(self, rwdc):
        rwdc.send_setting('display_center', rwdc.settings['display_center'] + 1000)

    def press_down(self, rwdc):
        t = TextRenderer("%.06f" % random.random())
        rwdc.renderer = VScroll(rwdc.renderer, t, VScroll.UP)

    def press_up(self, rwdc):
        t = TextRenderer("%.06f" % random.random())
        rwdc.renderer = VScroll(rwdc.renderer, t, VScroll.DOWN)

    def press_select(self, rwdc):
        t = TextRenderer("Wobble")
        rwdc.renderer = Dissolve(rwdc.renderer, t)


if __name__ == "__main__":
    r = RwdClient(renderer=TextRenderer("Ploo"), keyListener=MyListener())
    while True:
        r.poll()



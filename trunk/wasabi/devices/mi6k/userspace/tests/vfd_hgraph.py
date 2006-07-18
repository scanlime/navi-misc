#!/usr/bin/env python
#
# Demonstration of gapless horizontal bargraphs
#

import time, mi6k
from Numeric import *

class Bargraph:
    def __init__(self, vfd):
        self.vfd = vfd

        self.glyphs = []

        glyph = zeros((7,5))
        self.glyphs.append(array(glyph))

        glyph[:,0] = 1
        self.glyphs.append(array(glyph))

	glyph[:,2] = 1
	self.glyphs.append(array(glyph))

	glyph[:,4] = 1
	self.glyphs.append(array(glyph))

        # Load these characters into the VFD
        self.loadedGlyphs = fromstring(vfd.userDefinedCharacters[:len(self.glyphs)], UInt8)
        for i in xrange(len(self.glyphs)):
            vfd.defineCharacter(chr(self.loadedGlyphs[i]), self.glyphs[i])

        self.values = zeros(vfd.lines)

    def update(self):
        screen = zeros((self.vfd.lines, self.vfd.width), UInt8, savespace=True) + ord(' ')
        for line in xrange(self.vfd.lines):
	    x = self.values[line] * self.vfd.width
            screen[line, 0:int(x)] = self.loadedGlyphs[-1]
	    screen[line, int(x)] = self.loadedGlyphs[int((x - int(x)) * (len(self.glyphs)-1) + 0.5)]
        self.vfd.writeArray(screen)


if __name__ == '__main__':
    bargraph = Bargraph(mi6k.Device().vfd)

    startTime = time.time()
    while True:
        t = time.time() - startTime
        bargraph.values = sin(array((0, 5)) + t*3) * 0.5 + 0.5
        bargraph.update()

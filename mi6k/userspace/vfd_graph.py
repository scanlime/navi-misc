#!/usr/bin/env python
#
# Demonstration of bargraph-type-animation on the VFD
#

import time, mi6k
from Numeric import *

class Bargraph:
    def __init__(self, vfd):
        self.vfd = vfd

        # Create a list of glyphs for representing values between
        # 0 and 8 in a character cell.
        self.glyphs = []
        glyph = zeros((7,5))
        self.glyphs.append(array(glyph))
        for lines in xrange(1,8):
            glyph[-lines] = 1
            self.glyphs.append(array(glyph))

        # Load these characters into the VFD
        self.loadedGlyphs = fromstring(vfd.userDefinedCharacters[:len(self.glyphs)], UInt8)
        for i in xrange(len(self.glyphs)):
            vfd.defineCharacter(chr(self.loadedGlyphs[i]), self.glyphs[i])

        # Initialize the graph values to zero, provide scale values from 0 to 1 for the X axis
        self.values = zeros(self.vfd.width)
        self.scale = arange(0, 1 + 1.0 / self.vfd.width, 1.0 / (self.vfd.width-1))

    def update(self):
        # Calculate which custom character to use for each cell on the screen to
        # generate the actual bargraph.
        screen = zeros((self.vfd.lines, self.vfd.width), UInt8, savespace=True) + ord(' ')
        lineStep = 1.0 / self.vfd.lines
        for line in xrange(self.vfd.lines):
            scaledValues = clip((self.values - (lineStep * line)) / lineStep, 0, 0.99)
            screen[-(1+line)] = take(self.loadedGlyphs, (scaledValues * 8).astype(Int))
        self.vfd.writeArray(screen)


if __name__ == '__main__':
    bargraph = Bargraph(mi6k.MI6K().vfd)

    startTime = time.time()
    while True:
        t = time.time() - startTime
        bargraph.values = sin(bargraph.scale*10 + t*10) * 0.5 + 0.5
        bargraph.update()

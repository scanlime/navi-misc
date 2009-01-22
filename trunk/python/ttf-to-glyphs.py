#!/usr/bin/env python
#
# This is a tiny Python script that uses PyGame to render a TrueType
# font into a collection of glyphs, which are stored in a simple
# binary format suitable for use in embedded systems.
#
# Usage:
#   ttf-to-glyphs.py <ttf file> <glyph file> <font size> <foreground> <background>
#
# Colors are in any format supported by Pygame.
#
# The resulting file begins with table which includes each character's
# size and offset. We don't include any real font metrics- the
# assumption is that glyphs do not overlap, and they can simply be
# packed tightly on a line from left to right. The image itself
# includes any necessary inter-character spacing.
#
# The table consists of 256 characters, each with the format:
#
#   Offset   Type    Description
#   ------------------------------
#   0        uint8   Glyph width
#   1        uint8   Glyph height
#   2        uint8   (reserved)
#   3        uint8   (reserved)
#   4        uint32  File offset for image data
#
# Micah Dowty <micah@navi.cx>
#

import struct
import pygame
import sys

class Args:
    ttfFile = sys.argv[1]
    glyphFile = sys.argv[2]
    fontSize = int(sys.argv[3])
    fgColor = pygame.Color(sys.argv[4])
    bgColor = pygame.Color(sys.argv[5])

pygame.font.init()
pygame.display.init()

font = pygame.font.Font(Args.ttfFile, Args.fontSize)

class GlyphInfo:
    def __init__(self, char):
        self.offset = 0

        try:
            glyph = font.render(char, True, Args.fgColor, Args.bgColor)
        except pygame.error:
            self.data = ''
            self.width = 0
            self.height = 0
            return

        glyph = glyph.convert(32)
        self.width, self.height = glyph.get_size()
        self.data = str(glyph.get_buffer().raw)

    def packHeader(self):
        return struct.pack("BBBBI", self.width, self.height, 0, 0, self.offset)

# Render all glyphs
glyphs = [GlyphInfo(chr(i)) for i in range(256)]

# Assign offsets: Start by measuring the size of the header, then
# measure each glyph's data
offset = 0
for glyph in glyphs:
    offset += len(glyph.packHeader())
for glyph in glyphs:
    glyph.offset = offset
    offset += len(glyph.data)

# Now generate the output file
f = open(Args.glyphFile, "wb")
for glyph in glyphs:
    f.write(glyph.packHeader())
for glyph in glyphs:
    f.write(glyph.data)

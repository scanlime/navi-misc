""" Captcha.Visual.Text

Text generation for visual CAPTCHAs.
"""
#
# PyCAPTCHA Package
# Copyright (C) 2004 Micah Dowty <micah@navi.cx>
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

import random, os
import Captcha
from Captcha import Visual
import ImageFont, ImageDraw


class FontFactory(object):
    """Picks random fonts and/or sizes from a given list.
       'sizes' can be a single size or a (min,max) tuple.
       If any of the given files are directories, all *.ttf found
       in that directory will be added.
       """
    def __init__(self, sizes, *fileNames):
        self.fileNames = fileNames
        self.fullPaths = None

        if type(sizes) is tuple:
            self.minSize = sizes[0]
            self.maxSize = sizes[1]
        else:
            self.minSize = sizes
            self.maxSize = sizes

    def findFullPaths(self, names):
        """Find full paths corresponding to the given file names"""
        paths = []
        for name in names:
            path = os.path.join(Captcha.dataDir, "fonts", name)
            if os.path.isdir(path):
                for content in os.listdir(path):
                    if content.endswith(".ttf"):
                        paths.append(os.path.join(path, content))
            else:
                paths.append(path)
        return paths

    def pick(self):
        if self.fullPaths is None:
            self.fullPaths = self.findFullPaths(self.fileNames)
        fileName = random.choice(self.fullPaths)
        size = int(random.uniform(self.minSize, self.maxSize) + 0.5)
        return ImageFont.truetype(fileName, size)

# Predefined font factories
defaultFontFactory = FontFactory((25, 40), "vera")


class TextLayer(Visual.Layer):
    """Represents a piece of text rendered within the image.
       Alignment is given such that (0,0) places the text in the
       top-left corner and (1,1) places it in the bottom-left.

       The font and alignment are optional, if not specified one is
       chosen randomly. If no font factory is specified, the default is used.
       """
    def __init__(self, text,
                 alignment   = None,
                 font        = None,
                 fontFactory = None,
                 textColor   = "black",
                 borderSize  = 0,
                 borderColor = "white",
                 ):
        if fontFactory is None:
            global defaultFontFactory
            fontFactory = defaultFontFactory

        if font is None:
            font = fontFactory.pick()

        if alignment is None:
            alignment = (random.uniform(0,1),
                         random.uniform(0,1))

        self.textSize    = font.getsize(text)
        self.text        = text
        self.alignment   = alignment
        self.font        = font
        self.textColor   = textColor
        self.borderSize  = borderSize
        self.borderColor = borderColor

    def render(self, img):
        draw = ImageDraw.Draw(img)

        # Find the text's origin given our alignment and current image size
        x = int((img.size[0] - self.textSize[0] - self.borderSize*2) * self.alignment[0] + 0.5)
        y = int((img.size[1] - self.textSize[1] - self.borderSize*2) * self.alignment[1] + 0.5)

        # Draw the border if we need one. This is slow and ugly, but there doesn't
        # seem to be a better way with PIL.
        if self.borderSize > 0:
            for bx in (-1,0,1):
                for by in (-1,0,1):
                    draw.text((x + bx * self.borderSize,
                               y + by * self.borderSize),
                              self.text, font=self.font, fill=self.borderColor)

        # And the text itself...
        draw.text((x,y), self.text, font=self.font, fill=self.textColor)

### The End ###

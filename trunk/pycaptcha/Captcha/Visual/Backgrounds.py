""" Captcha.Visual.Backgrounds

Background layers for visual CAPTCHAs
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

from Captcha.Visual import Layer
import Captcha
import random, os
import ImageDraw, Image


class SolidColor(Layer):
    """A solid color background. Very weak on its own, but good
       to combine with other backgrounds.
       """
    def __init__(self, color="white"):
        self.color = color

    def render(self, image):
        image.paste(self.color)


class Grid(Layer):
    """A grid of lines, with a given foreground color.
       The size is given in pixels. The background is transparent,
       so another layer (like SolidColor) should be put behind it.
       """
    def __init__(self, size=16, foreground="black"):
        self.size = size
        self.foreground = foreground

    def render(self, image):
        # Random grid alignment
        xa = random.uniform(0, self.size)
        ya = random.uniform(0, self.size)
        draw = ImageDraw.Draw(image)

        for i in xrange(image.size[0] / self.size + 1):
            draw.line( (i*self.size+xa, 0,
                        i*self.size+xa, image.size[1]), fill=self.foreground)

        for i in xrange(image.size[0] / self.size + 1):
            draw.line( (0, i*self.size+ya,
                        image.size[0], i*self.size+ya), fill=self.foreground)


class TiledImage(Layer):
    """Pick a random image and a random offset, and tile the rendered image with it"""
    def pickTile(self):
        """Return the image we'll use for tiling"""
        bgDir = os.path.join(Captcha.dataDir, "backgrounds")
        files = []
        for name in os.listdir(bgDir):
            if name.endswith(".jpeg"):
                files.append(os.path.join(bgDir, name))

        return Image.open(random.choice(files))

    def render(self, image):
        # Random image
        tile = self.pickTile()

        # Random offset
        xa = random.uniform(0, tile.size[0])
        ya = random.uniform(0, tile.size[1])

        for j in xrange(-1, int(image.size[1] / tile.size[1]) + 1):
            for i in xrange(-1, int(image.size[0] / tile.size[0]) + 1):
                dest = (int(xa+i*tile.size[0]), int(ya+j*tile.size[1]))
                image.paste(tile, dest)

### The End ###

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

from Captcha.Visual import Layer, Pictures
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
        self.offset = (random.uniform(0, self.size),
                       random.uniform(0, self.size))

    def render(self, image):
        draw = ImageDraw.Draw(image)

        for i in xrange(image.size[0] / self.size + 1):
            draw.line( (i*self.size+self.offset[0], 0,
                        i*self.size+self.offset[0], image.size[1]), fill=self.foreground)

        for i in xrange(image.size[0] / self.size + 1):
            draw.line( (0, i*self.size+self.offset[1],
                        image.size[0], i*self.size+self.offset[1]), fill=self.foreground)


class TiledImage(Layer):
    """Pick a random image and a random offset, and tile the rendered image with it"""
    def __init__(self, imageFactory=None):
        if not imageFactory:
            imageFactory = Pictures.defaultImageFactory
        self.tile = imageFactory.pick()
        self.offset = (random.uniform(0, self.tile.size[0]),
                       random.uniform(0, self.tile.size[1]))

    def render(self, image):
        for j in xrange(-1, int(image.size[1] / self.tile.size[1]) + 1):
            for i in xrange(-1, int(image.size[0] / self.tile.size[0]) + 1):
                dest = (int(self.offset[0] + i*self.tile.size[0]),
                        int(self.offset[1] + j*self.tile.size[1]))
                image.paste(self.tile, dest)


class RandomDots(Layer):
    """Draw random colored dots"""
    def __init__(self, colors=("white", "black"), dotSize=4, numDots=400):
        self.colors = colors
        self.dotSize = dotSize
        self.numDots = numDots

    def render(self, image):
        r = random.Random(self)
        for i in xrange(self.numDots):
            bx = int(r.uniform(0, image.size[0]-self.dotSize))
            by = int(r.uniform(0, image.size[1]-self.dotSize))
            image.paste(r.choice(self.colors), (bx, by,
                                                bx+self.dotSize-1,
                                                by+self.dotSize-1))

### The End ###

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
import random
import ImageDraw


class SolidColor(Layer):
    """A solid color background, mostly for testing"""
    def __init__(self, color="white"):
        self.color = color

    def render(self, image):
        image.paste(self.color)


class Grid(Layer):
    """A grid of lines, with a given foreground and background.
       The size is given in pixels. The background can be None
       to avoid drawing any background.
       """
    def __init__(self, size=16, background="white", foreground="black"):
        self.size = size
        self.background = background
        self.foreground = foreground

    def render(self, image):
        # Random grid alignment
        xa = random.uniform(0, self.size)
        ya = random.uniform(0, self.size)
        draw = ImageDraw.Draw(image)

        if self.background:
            image.paste(self.background)

        for i in xrange(image.size[0] / self.size + 1):
            draw.line( (i*self.size+xa, 0,
                        i*self.size+xa, image.size[1]), fill=self.foreground)

        for i in xrange(image.size[0] / self.size + 1):
            draw.line( (0, i*self.size+ya,
                        image.size[0], i*self.size+ya), fill=self.foreground)

### The End ###

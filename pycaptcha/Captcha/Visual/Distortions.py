""" Captcha.Visual.Distortions

Distortion layers for visual CAPTCHAs
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
import Numeric, RandomArray
import Image, ImageDraw
import random, math


class WigglyBlocks(Layer):
    """Randomly select and shift blocks of the image"""
    def __init__(self, blockSize=16, sigma=0.01, iterations=300):
        self.blockSize = blockSize
        self.sigma = sigma
        self.iterations = iterations

    def render(self, image):
        for i in xrange(self.iterations):
            # Select a block
            bx = int(random.uniform(0, image.size[0]-self.blockSize))
            by = int(random.uniform(0, image.size[1]-self.blockSize))
            block = image.crop((bx, by, bx+self.blockSize-1, by+self.blockSize-1))

            # Figure out how much to move it.
            # The call to floor() is important so we always round toward
            # 0 rather than to -inf. Just int() would bias the block motion.
            mx = int(math.floor(random.normalvariate(0, self.sigma)))
            my = int(math.floor(random.normalvariate(0, self.sigma)))

            # Now actually move the block
            image.paste(block, (bx+mx, by+my))

### The End ###

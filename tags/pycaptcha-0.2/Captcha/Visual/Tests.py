""" Captcha.Visual.Tests

Visual CAPTCHA tests
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

from Captcha.Visual import Text, Backgrounds, Distortions, ImageCaptcha
from Captcha import Words
import random

__all__ = ["PseudoGimpy", "AngryGimpy", "AntiSpam"]


class PseudoGimpy(ImageCaptcha):
    """A relatively easy CAPTCHA that's somewhat easy on the eyes"""
    def getLayers(self):
        word = Words.defaultWordList.pick()
        self.addSolution(word)
        return [
            random.choice([
                Backgrounds.CroppedImage(),
                Backgrounds.TiledImage(),
            ]),
            Text.TextLayer(word, borderSize=1),
            Distortions.SineWarp(),
            ]


class AngryGimpy(ImageCaptcha):
    """A harder but less visually pleasing CAPTCHA"""
    def getLayers(self):
        word = Words.defaultWordList.pick()
        self.addSolution(word)
        return [
            Backgrounds.TiledImage(),
            Backgrounds.RandomDots(),
            Text.TextLayer(word, borderSize=1),
            Distortions.WigglyBlocks(),
            ]


class AntiSpam(ImageCaptcha):
    """A fixed-solution CAPTCHA that can be used to hide email addresses or URLs from bots"""
    fontFactory = Text.FontFactory(20, "vera/VeraBd.ttf")
    defaultSize = (512,50)

    def getLayers(self, solution="murray@example.com"):
        self.addSolution(solution)

        textLayer = Text.TextLayer(solution,
                                   borderSize = 2,
                                   fontFactory = self.fontFactory)

        return [
            Backgrounds.CroppedImage(),
            textLayer,
            Distortions.SineWarp(amplitudeRange = (2, 4)),
            ]

### The End ###

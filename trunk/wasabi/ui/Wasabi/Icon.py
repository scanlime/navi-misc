""" Wasabi.Icon

Icons are the basic user interface element used for Wasabi's menu
system. An Icon is an image combined with text. Icons are placed
onto tracks that parametrically define a position-size curve that
the icons follow. User interaction can then select an active
icon track and rotate icons along this track.
"""
#
# Wasabi Project
# Copyright (C) 2003 Micah Dowty <micahjd@users.sourceforge.net>
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

from BZEngine.UI import Texture, GLText, GLOrtho
from OpenGL import GL


class IconStyle:
    """The collection of settings specifying how an icon is to be proportioned and rendered.
       This includes all aspects of icon rendering not specific to any single icon image or text.

       textHeight is the height of the text relative to the image.

       textSide is a 2-tuple specifying where the text is anchored to the
       image. (0,0) would place the text past the top-left corner of the image,
       (1,1) would place it past the bottom-right, (0.5, 1) would place it
       centered below the image, (1, 0.5) would place it centered to the right
       of the image.
       """
    def __init__(self,
                 textHeight    = 0.25,
                 textSide      = (0.5, 1),
                 fontName      = None,
                 textColor     = (1,1,1,1),
                 shadowOffset  = (2,2),
                 shadowColor   = (0,0,0, 0.5),
                 shadowEnabled = True):
        self.textHeight = textHeight
        self.textSide = textSide
        self.fontName = fontName
        self.textColor = textColor
        self.shadowOffset = shadowOffset
        self.shadowColor = shadowColor
        self.shadowEnabled = shadowEnabled


# Define a few common icon styles
ListIconStyle = IconStyle(
    textHeight = 0.9,
    textSide   = (1, 0.5))
LargeIconStyle = IconStyle()


class Icon:
    """An icon, consisting of an image and text with fixed aspect ratio and
       positioning relative to each other, that can be rendered at any size.
       This class only encapsulates the details specific to a particular icon.
       The rendering style is given at render time with an IconStyle instance.

       imageAspect is the width/height ratio for the image.
       """
    def __init__(self, imageName, text, imageAspect=1):
        # Load the texture, disable wrapping to avoid half-pixel
        # artifacts at the edges of the icon.
        self.texture = Texture.load(imageName)
        self.texture.setRepeat(GL.GL_CLAMP, GL.GL_CLAMP)

        self.text = text
        self.imageAspect = imageAspect

    def draw(self, style, height):
        """Draws the icon, centered about the current origin, at the specified
           height and with the specified style. Assumes the OpenGL context
           is already in a GLOrtho-friendly state.
           """
        # Calculate an aspect-correct width for the icon and draw it centered at the origin
        width = self.imageAspect * height
        GLOrtho.setColor(1,1,1,1)
        GLOrtho.setTexture(self.texture)
        GLOrtho.filledRect((width, height), (0.5, 0.5))

        # Move to the edge of the image indicated by our textSide and calculate the
        # text's corresponding alignment value and font size
        GLOrtho.push()
        GLOrtho.translate(width  * (style.textSide[0] - 0.5),
                          height * (style.textSide[1] - 0.5))
        textAlignment = (1 - style.textSide[0],
                         1 - style.textSide[1])
        textHeight = height * style.textHeight

        # Render the shadow and the text itself
        if style.shadowEnabled:
            GLOrtho.setColor(*style.shadowColor)
            GLOrtho.push()
            GLOrtho.translate(*style.shadowOffset)
            GLText.draw(self.text, textHeight, style.fontName, textAlignment)
            GLOrtho.pop()
        GLOrtho.setColor(*style.textColor)
        GLText.draw(self.text, textHeight, style.fontName, textAlignment)
        GLOrtho.pop()

### The End ###

""" BZEngine.UI.HUD

Views and utilities for rendering the Heads Up Display
"""
#
# Python BZEngine Package
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

from __future__ import division
from BZEngine.UI import GLText, GLExtension, GLOrtho, Texture
from BZEngine import Animated
from Numeric import subtract


class Panel:
    """A view that draws a shaded background and thin border"""
    def __init__(self, viewport):
        viewport.fov = None
        self.viewport = viewport
        viewport.onDrawFrame.observe(self.render)

    def render(self):
        GLOrtho.setup()

        # Set the square inside our viewport by one
        # pixel so the antialiased lines won't be chopped in half.
        GLOrtho.translate(1,1)
        size = subtract(self.viewport.size, (2,2))

        # Translucent filling
        GLOrtho.setColor(0,0,0,0.3)
        GLOrtho.filledRect(size)

        # Bright border
        GLOrtho.setColor(1,1,1,1)
        GLOrtho.rectOutline(size)


class Text:
    """A view that draws a block of text.
       'alignment' specifies the text's justification relative
       to the viewport. (0,0) is top-left, (1,1) is bottom-right,
       (0.5, 0.5) is centered, etc.
       """
    def __init__(self, viewport,
                 text         = '',
                 color        = (1,1,1,1),
                 fontSize     = None,
                 fontName     = None,
                 shadow       = False,
                 shadowColor  = (0,0,0,0.6),
                 shadowOffset = 2,
                 alignment    = (0,0)
                 ):
        viewport.fov = None
        self.viewport = viewport
        self.text = text
        self.color = color
        self.fontSize = fontSize
        self.fontName = fontName
        self.shadow = shadow
        self.shadowColor = shadowColor
        self.shadowOffset = shadowOffset
        self.alignment = alignment
        viewport.onDrawFrame.observe(self.render)
        self.init()

    def init(self):
        """A hook for clients to do extra initialization before the first write() without
           affecting the constructor's calling signature.
           """
        pass

    def write(self, text):
        self.text += text

    def getText(self):
        return self.text

    def render(self):
        text = self.getText()
        if text:
            GLOrtho.setup()

            if self.alignment:
                GLOrtho.translate(self.viewport.size[0] * self.alignment[0],
                                  self.viewport.size[1] * self.alignment[1])
            if self.shadow:
                GLOrtho.push()
                GLOrtho.translate(self.shadowOffset, self.shadowOffset)
                GLOrtho.setColor(*self.shadowColor)
                GLText.draw(text, self.fontSize, self.fontName, self.alignment)
                GLOrtho.pop()
            GLOrtho.setColor(*self.color)
            GLText.draw(text, self.fontSize, self.fontName, self.alignment)


class Image:
    """A view that draws an image, stored in a single texture. The image's
       size is specified separately, so the image can be rescaled to fit in
       an OpenGL-friendly texture size.
       'alignment' specifies the text's justification relative
       to the viewport. (0,0) is top-left, (1,1) is bottom-right,
       (0.5, 0.5) is centered, etc.
       """
    def __init__(self, viewport, textureName, size,
                 color        = (1,1,1,1),
                 alignment    = (0,0)
                 ):
        viewport.fov = None
        self.viewport = viewport
        self.texture = Texture.load(textureName)
        self.size = size
        self.color = color
        self.alignment = alignment
        viewport.onDrawFrame.observe(self.render)

    def render(self):
        GLOrtho.setup()

        if self.alignment:
            GLOrtho.translate(self.viewport.size[0] * self.alignment[0],
                              self.viewport.size[1] * self.alignment[1])
            GLOrtho.setColor(*self.color)
            GLOrtho.setTexture(self.texture)
            GLOrtho.filledRect(self.size, self.alignment)


class ScrolledText(Text):
    """A view that draws a block of vertically scrolled text"""
    def init(self):
        self.scroll = Animated.Value(Animated.LogApproach(0, 0.2))

    def write(self, text):
        Text.write(self, text)

        lineHeight = GLText.size("", self.fontSize, self.fontName)[1]
        viewableLines = int(self.viewport.size[1] / lineHeight)
        self.text = "\n".join(self.text.split("\n")[-viewableLines:])

### The End ###

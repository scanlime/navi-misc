""" BZEngine.UI.GLText

Interface for drawing text in OpenGL. This uses freetype via pygame to
build texture pages holding glyph images, then it renders individual
glyphs using those texture pages.

For best results the fonts should be rendered with the viewport in
ortho mode, with OpenGL units corresponding directly to pixels.
Other configurations will work, but only with a 1:1 correspondance
to pixels can this module render with proper hinting.

The origin of a character or string is always the top-left corner.
Due to the limitations of SDL_ttf, this module never works with raw
glyph images, it works instead with glyphs padded to include the font
height and escapement.
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

from __future__ import division
import pygame, string, math
from pygame.locals import *
from BZEngine import Util
from BZEngine.UI.Texture import Texture, GLExtension
from OpenGL.GL import *


# Default font height, in pixels
defaultSize = 14

# Some other helpful sizes
smallSize = 10
maxRenderSize = 64

# Names that can be used in place of actual filenames
fontAliases = {
    None:       'Vera.ttf',
    'bold':     'VeraBd.ttf',
    }

# Default list of sizes to render fonts at
defaultSizes = [smallSize, defaultSize, maxRenderSize]

# Cache of font objects, by data file name
loadedFonts = {}


class Glyph:
    """Positioning information for one glyph on a font page"""
    def __init__(self, rect, page):
        self.size = rect[2:]
        self.page = page
        self.texCoords = (rect[0] / self.page.size[0],
                          (self.page.size[1]-rect[1]-rect[3]) / self.page.size[1],
                          rect[2] / self.page.size[0],
                          rect[3] / self.page.size[1])

    def draw(self, magnification):
        self.page.updateTexture()
        self.page.texture.bind()

        v = (self.size[0] * magnification, self.size[1] * magnification)
        t = self.texCoords
        glBegin(GL_QUADS)
        glTexCoordf(t[0],t[1])
        glVertex2f(0, v[1])
        glTexCoordf(t[0]+t[2],t[1])
        glVertex2f(v[0], v[1])
        glTexCoordf(t[0]+t[2],t[1]+t[3])
        glVertex2f(v[0], 0)
        glTexCoordf(t[0],t[1]+t[3])
        glVertex2f(0, 0)
        glEnd()
        glTranslatef(math.floor(v[0]), 0, 0)


class FontPage:
    """A collection of glyphs rendered to one texture"""
    def __init__(self, font, size=(256,256)):
        self.font = font
        self.packingY = 0
        self.packingX = 0
        self.lineHeight = 0
        self.size = size
        self.surface = pygame.Surface(size)
        self.texture = Texture()
        self.textureDirty = True

        # To avoid mipmaps blending glyhps together, we have to
        # leave a gap between glyphs equal to the number of mipmap levels we have.
        self.gap = (int(math.log(size[0]) / math.log(2)),
                    int(math.log(size[1]) / math.log(2)))

    def updateTexture(self):
        """If necessary, update the texture from the surface"""
        if not self.textureDirty:
            return
        self.texture.loadSurface(self.surface, monochrome=True)
        self.textureDirty = False

    def allocRect(self, size):
        """Allocate space for a rectangle of the given size on the texture.
           Return a point indicating the top-left corner of the allocated
           rectangle, or None if there is no room.
           This implementation simply packs top-down left-to-right in rows.
           """
        if size[0] > self.size[0]:
            return None

        if self.packingX + size[0] + self.gap[0] > self.size[0]:
            # Next line
            self.packingX = 0
            self.packingY += self.lineHeight
            self.lineHeight = 0

        if self.packingY + size[1] > self.size[1]:
            return None
        if size[1] + self.gap[1] > self.lineHeight:
            self.lineHeight = size[1] + self.gap[1]

        allocated = (self.packingX, self.packingY)
        self.packingX += size[0] + self.gap[0]
        return allocated

    def pack(self, char):
        """Pack the glyph for the given character into our texture, return a Glyph.
           If there isn't enough room, return None.
           """
        # Size the glyph and allocate space for it
        glyphSize = self.font.size(char)
        allocated = self.allocRect(glyphSize)
        if not allocated:
            return None

        # Render the character and create a Glyph
        self.surface.blit(self.font.render(char, True, (255,255,255), (0,0,0)), allocated)
        self.textureDirty = True
        return Glyph((allocated[0], allocated[1], glyphSize[0], glyphSize[1]), self)


class RenderedFont:
    """Represents one font size, rendered to zero or more texture 'pages'"""
    def __init__(self, filename, size):
        self.font = pygame.font.Font(filename, size)
        self.pages = [FontPage(self.font)]
        self.glyphs = {}
        self.size = size

        # Preload the pages with common glyphs. This will load anything
        # considered a printable character in the current locale.
        for char in string.digits + string.letters + string.punctuation:
            self.getGlyph(char)

        # We don't store whitespace rendered, but we do need to know how big it is
        self.spaceSize = self.font.size(" ")

    def getGlyph(self, char):
        """Get the associated glyph for a character, either by looking up
           an existing glyph or by packing the character in an available page.
           """
        try:
            return self.glyphs[char]
        except KeyError:
            g = self.pages[-1].pack(char)
            if not g:
                self.pages.append(FontPage(self.font))
                g = self.pages[-1].pack(char)
            self.glyphs[char] = g
            return g

    def draw(self, char, size):
        self.getGlyph(char).draw(size / self.size)


class Font:
    """Represents all sizes of one scalable font."""
    def __init__(self, name, sizes=defaultSizes):
        filename = Util.dataFile(name)
        pygame.font.init()
        self.sizes = {}
        self.sortedSizes = sizes
        self.sortedSizes.sort()
        for size in sizes:
            self.sizes[size] = RenderedFont(filename, size)

    def findRendered(self, size):
        """Find a rendered font best suitable for the given size.
           An exact match is searched for first, then the next largest font.
           """
        try:
            # See if we have the exact size
            rendered = self.sizes[size]
        except KeyError:
            # Nope. Find the next larger size if we can
            rendered = None
            for renderedSize in self.sortedSizes:
                if renderedSize > size:
                    rendered = self.sizes[renderedSize]
                    break

            # If we didn't have anything big enough, just use the largest size we have
            if not rendered:
                rendered = self.sizes[self.sortedSizes[-1]]
        return rendered

    def draw(self, text, size=defaultSize, alignment=None):
        """Draw the given text using the current OpenGL transform and color.
           All glyph escapements will cause the OpenGL modelview matrix to
           be translated.

           'alignment' specifies the text's justification relative
           to the origin. (0,0) places the top-left corner at the origin,
           (1,1) places the bottom bottom-right at the origin,
           (0.5, 0.5) is centered, etc.
           The default is equivalent to (0,0)
           """
        if alignment:
            textSize = self.size(text, size)
            glTranslatef(-textSize[0] * alignment[0],
                         -textSize[1] * alignment[1],
                         0)

        # Prepare the OpenGL state
        GLExtension.disableMultitex()
        glEnable(GL_TEXTURE_2D)
        glDisable(GL_LIGHTING)
        glDisable(GL_COLOR_MATERIAL)
        glDisable(GL_DEPTH_TEST)
        glEnable(GL_BLEND)
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA)

        rendered = self.findRendered(size)
        magnification = size / rendered.size

        glPushMatrix()
        for char in text:
            # Handle whitespace here
            if char == " ":
                glTranslatef(math.floor(rendered.spaceSize[0] * magnification), 0, 0)
            elif char == "\n":
                glPopMatrix()
                glTranslatef(0, math.floor(rendered.spaceSize[1] * magnification), 0)
                glPushMatrix()
            elif char == "\t":
                glTranslatef(math.floor(rendered.spaceSize[0] * 8 * magnification), 0, 0)

            else:
                rendered.draw(char, size)

        glPopMatrix()
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)

    def size(self, text, fontSize=defaultSize):
        """Measure the rendered size of the given text"""
        rendered = self.findRendered(fontSize)
        magnification = fontSize / rendered.size
        unscaled = rendered.font.size(text)
        return (unscaled[0] * magnification,
                unscaled[1] * magnification)

    def drawCentered(self, text, fontSize=defaultSize):
        """Draw text centered around the origin"""
        size = self.size(text, fontSize)
        glTranslatef(math.floor(-size[0]/2), math.floor(-size[1]/2), 0)
        self.draw(text, fontSize)


def findFont(fontName=None):
    """Look up and/or load the given font. Font names can be a data file
       name, or an alias as defined at the top of this module.
       """
    global fontAliases, loadedFonts
    try:
        fontName = fontAliases[fontName]
    except:
        pass
    try:
        font = loadedFonts[fontName]
    except:
        font = Font(fontName)
        loadedFonts[fontName] = font
    return font


def draw(text, fontSize=None, fontName=None, alignment=None):
    if fontSize is None:
        fontSize = defaultSize
    findFont(fontName).draw(text, fontSize, alignment)


def size(text, fontSize=None, fontName=None):
    if fontSize is None:
        fontSize = defaultSize
    return findFont(fontName).size(text, fontSize)

### The End ###

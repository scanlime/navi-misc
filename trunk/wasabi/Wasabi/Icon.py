""" Wasabi.Icon

Icons are the basic user interface element used for Wasabi's menu
system. An Icon is an image combined with text. Icons are placed
onto tracks that parametrically define a position-size curve that
the icons follow. User interaction can then select an active
icon track and rotate icons along this track.
"""
#
# Wasabi Project
# Copyright (C) 2003-2004 Micah Dowty <micahjd@users.sourceforge.net>
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
from BZEngine import Animated, Util
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
    textSide   = (1.05, 0.5))
LargeIconStyle = IconStyle()


class Icon:
    """An icon, consisting of an image and text with fixed aspect ratio and
       positioning relative to each other, that can be rendered at any size.
       This class only encapsulates the details specific to a particular icon.
       The rendering style is given at render time with an IconStyle instance.

       imageAspect is the width/height ratio for the image.

       The provided 'image' is assumed to be a data file name if it is a string.
       It may also be a Texture instance.
       """
    def __init__(self, image, text=None, imageAspect=1):
        if isinstance(image, Texture.Texture):
            self.imageName = None
            self.texture = image
        else:
            self.imageName = image
            self.texture = None
        self.text = text
        self.imageAspect = imageAspect

    def draw(self, style, height):
        """Draws the icon, centered about the current origin, at the specified
           height and with the specified style. Assumes the OpenGL context
           is already in a GLOrtho-friendly state.
           """
        # Lazy texture loading
        if not self.texture:
            # Load the texture, disable wrapping to avoid half-pixel
            # artifacts at the edges of the icon.
            self.texture = Texture.load(self.imageName)
            self.texture.setRepeat(GL.GL_CLAMP, GL.GL_CLAMP)

        # Calculate an aspect-correct width for the icon and draw it centered at the origin
        width = self.imageAspect * height
        GLOrtho.setColor(1,1,1,1)
        GLOrtho.setTexture(self.texture)
        GLOrtho.filledRect((width, height), (0.5, 0.5))

        if self.text:
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


class DockIcon(object):
    """An icon container used by the dock. Includes animation information for each icon,
       and methods for rendering the icon on a dock's parametric track.
       """
    def __init__(self, dock, icon):
        self.dock = dock
        self.icon = icon
        self.animPosition = Animated.Value(Animated.LogApproach(0, dock.iconApproachSpeed))

    def integrate(self, dt):
        self.animPosition.integrate(dt)

    def setPosition(self, x):
        """Sets the animation target for this icon's position"""
        self.animPosition.f.target = x

    def getPosition(self):
        """Get the animation target for this icon's position"""
        return self.animPosition.f.target

    position = property(getPosition, setPosition)

    def jump(self):
        """Instantly jump to the target track position"""
        self.animPosition.value = self.animPosition.f.target

    def draw(self):
        """Draw the icon at its current position, size, and style"""
        (position, height) = self.dock.trackFunction(self.animPosition.value)
        vpSize = self.dock.viewport.size
        if (position[0] > -vpSize[0]/2 and position[0] < vpSize[0]*1.5 and
            position[1] > -vpSize[1]/2 and position[1] < vpSize[1]*1.5):
            GLOrtho.push()
            GLOrtho.translate(*position)
            self.icon.draw(self.dock.iconStyle, height)
            GLOrtho.pop()


class Dock(object):
    """A container for icons. The Dock defines a parametric 'track' icons move along,
       mapping a scalar position to a position and size in screen coordinates.
       Icons are spaced along this track and rotated along it as the user makes a
       selection.

       The supplied trackFunction must map a scalar to a (position, height) tuple. The
       position must itself be a 2-tuple.

       If iconSpacing is None, icons are spaced evenly along the interval [0,1]. Otherwise,
       it is an absolute spacing value in track coordinates.

       selectionIndex is the index of the currently selected icon. It may be
       floating point.
       """
    def __init__(self, viewport, trackFunction,
                 icons             = [],
                 iconSpacing       = None,
                 iconStyle         = LargeIconStyle,
                 iconApproachSpeed = 8,
                 selectionIndex    = 0.0,
                 ):
        self.dockIconMap = {}
        self.viewport = viewport
        self.trackFunction = trackFunction
        self.iconSpacing = iconSpacing
        self.iconStyle = iconStyle
        self.iconApproachSpeed = iconApproachSpeed
        self._selectionIndex = selectionIndex

        self.empty()
        self.add(*icons)
        self.jump()

        self.time = Animated.Timekeeper()
        self.viewport.fov = None
        self.viewport.onDrawFrame.observe(self.drawFrame)
        self.viewport.onSetupFrame.observe(self.setupFrame)

    def empty(self):
        """Remove all icons from the dock"""
        self.icons = []

    def add(self, *icons):
        """Add a list of icons to the dock, animating their entry."""
        for icon in icons:
            dockIcon = DockIcon(self, icon)
            self.dockIconMap[icon] = dockIcon
            self.icons.append(dockIcon)
        self.respaceIcons()

    def remove(self, *icons):
        """Remove the given icons from the menu"""
        for icon in icons:
            self.icons.remove(self.dockIconMap[icon])
            del self.dockIconMap[icon]
        self.respaceIcons()

    def jump(self):
        """Instantly jump all current animation to its next resting point"""
        for icon in self.icons:
            icon.jump()

    def setupFrame(self):
        self.integrate(self.time.step())

    def integrate(self, dt):
        """Move animation forward by the given time step"""
        for icon in self.icons:
            icon.integrate(dt)

    def drawFrame(self):
        GLOrtho.setup()
        for icon in self.icons:
            icon.draw()

    def respaceIcons(self):
        """Using the current iconSpacing and selectionIndex, change the position
           targets on all icons.
           """
        if self.iconSpacing is None:
            spacing = 1.0 / len(self.icons)
        else:
            spacing = self.iconSpacing

        position = -self.selectionIndex * spacing

        for icon in self.icons:
            icon.position = position
            position += spacing

    def setSelectionIndex(self, i):
        """Set the index of the currently selected icon. It may be a floating point
           number to show the icons at positions in between selections.
           """
        self._selectionIndex = i
        self.respaceIcons()

    def getSelectionIndex(self):
        return self._selectionIndex

    selectionIndex = property(getSelectionIndex, setSelectionIndex)


iconDict = None

def getIconDict():
    """Returns a dictionary mapping icon names to icon instances.
       This executes the 'icons.py' datafile, saves the icons created
       in its namespace, then caches the result.
       """
    global iconDict
    if not iconDict:
        iconDict = {}

        # Run icons.py. It needs the 'Icon' class in its namespace.
        ns = {'Icon': Icon}
        exec open(Util.dataFile('icons.py')) in ns

        # Pick out all the icon instances from the namespace
        for key, value in ns.items():
            if isinstance(value, Icon):
                iconDict[key] = value
    return iconDict


def load(name):
    """Load an icon by name"""
    return getIconDict()[name]

### The End ###

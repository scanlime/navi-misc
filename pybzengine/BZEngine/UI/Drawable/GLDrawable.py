""" BZEngine.UI.Drawable.GLDrawable

Abstract base class for an object that can be drawn to an OpenGL context.
This is the basic unit used by the ThreeDView to sort objects by texture,
so it is required that the drawable have at most one texture.
The blended flag is used to put objects that need blending into a
second rendering pass.
"""
#
# Python BZEngine Package
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
from BZEngine.UI import Texture, GLExtension

__all__ = ['GLDrawable', 'RenderSettings', 'RenderState']


class RenderSettings:
    """Settings that affect how ThreeDRender processes a a drawable"""
    def __init__(self):
        self.textures = []
        self.blended = False      # This primitive uses blending, and should be rendered after solid objects
        self.overlay = False      # This object is on top of the scene, rather than part of it
        self.static = True        # This object doesn't change, so it can be cached in a display list
        self.decal = False        # This object is a decal, to be drawn with a depth offset
	self.camera = False       # This object is a camera-induced effect
        self.reflection = False   # This object is a cube-mapped reflection
        self.background = False   # This object is part of a location-independent background behind the scene

    def __repr__(self):
        settings = [key for key in self.__dict__.iterkeys() if key[0] != '_']
        pairs = ["%s=%s" % (key, self.__dict__[key]) for key in settings]
        return "<%s %s>" % (self.__class__.__name__, ", ".join(pairs))


class RenderState:
    """Holder for state information passed down to all objects being rendered"""
    def __init__(self, view):
        self.view = view
        self.viewport = view.viewport

        self.picking = None       # This is set when we're doing a picking render, to determine what was clicked
        self.cubeMap = None       # This is set when we're rendering to a cube-map texture


class GLDrawable(object):
    """Abstract base class for an object that can be drawn to an OpenGL context.
       This is the basic unit used by the ThreeDView to sort objects by texture,
       so it is required that the drawable have at most one texture.
       The blended flag is used to put objects that need blending into a
       second rendering pass.
       """

    # The majority of object will only need one texture, so give them a simpler
    # syntax. Note that textureNames is for multitexturing. Textures are never
    # bound during the rendering of a drawable.
    textureName = None
    textureNames = []

    def __init__(self):
        self.render = RenderSettings()
	self.object = None
        self.loadTextures()

    def loadTextures(self):
        textures = []
        # textureNames overrides textureName
        if self.textureNames:
            names = self.textureNames
        else:
            names = (self.textureName,)

        # Each texture can be a string to indicate a name to load,
        # or a texture instance that is stored as-is.
        for name in names:
            if type(name) == str or type(name) == unicode:
                textures.append(Texture.load(name))
            elif name:
                textures.append(name)

        self.render.textures = tuple(textures)

    def parent(self, parent):
        self.object = parent

    def draw(self, rstate):
        pass

    def drawToList(self, rstate):
        """An alternate drawing function may be provided
           for using this drawable inside a display list
           """
        self.draw(rstate)

### The End ###

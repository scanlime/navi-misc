""" BZFlag.UI.ThreeDRender.Pass

Abstract implementation of a rendering pass, as well as several
concrete rendering passes. Provides an interface for instantiating
the available rendering passes with the proper priorities.
"""
#
# Python BZFlag Package
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

from BZFlag.UI import Drawable, GLExtension, Texture
from OpenGL.GL import *
from OpenGL.GL.ARB.multitexture import *
import LinearAlgebra, Numeric
from OpenGL.GL.EXT.texture_cube_map import *


class RenderPass:
    """A rendering pass defines the highest level of grouping for drawables.
       Rendering passes should separate blended and unblended objects, or
       groups of objects that require expensive OpenGL state changes.
       This is an abstract base class.
       """
    # Rendering passes with a higher priority get a chance to filter drawables first
    priority = None

    def __init__(self, filterPriority, renderPriority):
        self.filterPriority = filterPriority
        self.renderPriority = renderPriority
        self.erase()

    def render(self, rstate):
        """Run this rendering pass- if picking is not none, this is a picking render
           and all objects shoudl be given names.
           """
        pass

    def filter(self, drawable):
        """Determine whether the given drawable belongs in this
           pass, return True or False
           """
        pass

    def preprocess(self):
        """This is called after one or more objects are modified, to perform
           any necessary preprocessing on the render pass before rendering.
           """
        pass

    def add(self, drawable):
        pass

    def erase(self):
        """Remove all drawables from this pass"""
        pass

    def isEmpty(self):
        pass


class TextureGroup(Drawable.DisplayList):
    """A container for all the objects drawn in the same texture. Objects
       with the 'static' flag are compiled into a display list for
       speedy rendering of static worlds.
       """
    def __init__(self, drawables=[]):
        self.dynamic = []
        self.static = []
        for drawable in drawables:
            self.add(drawable)
        Drawable.DisplayList.__init__(self)

    def add(self, drawable):
        if drawable.render.static:
            self.static.append(drawable)
        else:
            self.dynamic.append(drawable)

    def drawToList(self, rstate):
        for drawable in self.static:
            drawable.drawToList(rstate)

    def draw(self, rstate):
        Drawable.DisplayList.draw(self, rstate)
        for drawable in self.dynamic:
            drawable.draw(rstate)

    def pickingDraw(self, rstate):
        """Alternate draw function used in picking, that assigns names to all drawables"""
        for drawable in self.static:
            rstate.picking.name(drawable)
            drawable.drawToList(rstate)
        for drawable in self.dynamic:
            rstate.picking.name(drawable)
            drawable.drawToList(rstate)


class BasicRenderPass(RenderPass):
    """A simple render pass for the majority of objects. Sorts objects by texture.
       This is the lowest priority render pass, and will collect all drawables
       left over from other passes.
       """
    def filter(self, drawable):
        return True

    def erase(self):
        self.textureGroups = {}
        self.texGroupItems = []

    def add(self, drawable):
        """Files the given drawable according to texture group.
           We keep a separate texGroupItems list so that we can maintain
           the order of items added to this pass. It also seems to be
           a little faster.
           """
        groups = self.textureGroups
        if groups.has_key(drawable.render.textures):
            groups[drawable.render.textures].add(drawable)
        else:
            newGroup = TextureGroup([drawable])
            groups[drawable.render.textures] = newGroup
            self.texGroupItems.append((drawable.render.textures, newGroup))

    def preprocess(self):
        """This builds display lists for all our texture groups"""
        for texgroup in self.textureGroups.itervalues():
            texgroup.dirty = True

    def isEmpty(self):
        return len(self.textureGroups) == 0

    def render(self, rstate):
        """Perform one render pass- iterates through each texture group in the pass,
           binding the pass' texture and drawing the texture group.
           """
        if rstate.picking:
            for textures, group in self.texGroupItems:
                group.pickingDraw(rstate)
        else:
            for textures, group in self.texGroupItems:
                if not rstate.picking:
                    try:
                        self.bindTextures(textures, rstate)
                    except Texture.DynamicTextureException:
                        # The textures this drawable requires aren't ready yet
                        return
                group.draw(rstate)

    def bindTextures(self, textures, rstate):
        """Bind the given list of 2D textures in the current OpenGL context.
           If multitexturing is not supported, all but the first texture will
           be ignored.

           This routine is careful that only the texture targets and units specified
           are enabled, and all others are disabled.
           """
        if GLExtension.multitexture:
            # We have multitexturing. Enable and bind the texture units we'll be using,
            # make sure the rest are disabled.
            texIndex = 0
            for unit in GLExtension.textureUnits:
                glActiveTextureARB(unit)
                for target in GLExtension.textureTargets:
                    glDisable(target)
                if texIndex < len(textures):
                    t = textures[texIndex]
                    if t:
                        t.bind(rstate)
                        glEnable(t.glTarget)
                texIndex += 1

            # Leave the first texture unit active
            glActiveTextureARB(GLExtension.textureUnits[0])
        else:
            # No multitexturing, only enable the current texture unit
            for target in GLExtension.textureTargets:
                glDisable(target)
            if textures:
                glEnable(textures[0].glTarget)
                textures[0].bind(rstate)


class BlendedRenderPass(BasicRenderPass):
    """A rendering pass that collects blended objects and renders them after most other objects."""
    def filter(self, drawable):
        return drawable.render.blended

    def render(self, rstate):
        glDepthMask(0)
        glEnable(GL_BLEND)
        BasicRenderPass.render(self, rstate)
        glDisable(GL_BLEND)
        glDepthMask(1)


class DecalRenderPass(BasicRenderPass):
    """A rendering pass that collects decal objects and renders them after solid objects."""
    def filter(self, drawable):
        return drawable.render.decal

    def render(self, rstate):
        glDepthMask(0)
        glEnable(GL_POLYGON_OFFSET_FILL)
        glEnable(GL_BLEND)
        glPolygonOffset(-2, -4)
        BasicRenderPass.render(self, rstate)
        glDisable(GL_BLEND)
        glDisable(GL_POLYGON_OFFSET_FILL)
        glDepthMask(1)


class OverlayRenderPass(BasicRenderPass):
    """A rendering pass that draws objects overlaid on the current scene."""
    def filter(self, drawable):
        return drawable.render.overlay

    def render(self, rstate):
        # Don't render overlay objects into environment maps
        if rstate.cubeMap:
            return

        glClear(GL_DEPTH_BUFFER_BIT)
        BasicRenderPass.render(self, rstate)


class BackgroundRenderPass(BasicRenderPass):
    """A rendering pass that draws objects behind the scene, with the camera's
       translation removed and Z-buffer writes disabled.
       """
    def filter(self, drawable):
        return drawable.render.background

    def render(self, rstate):
        # Since the background's depth values will not be consistent with
        # the rest of the scene, it will disrupt picking.
        if rstate.picking:
            return

        glPushMatrix()
        # Strip out all but the top 3x3 square of the modelview matrix.
        # This preserves the camera's location, but keeps it centered at 0,0,0
        rotation = Numeric.identity(4, Numeric.Float32)
        rotation[:3,:3] = glGetFloatv(GL_MODELVIEW_MATRIX)[:3,:3]
        glLoadMatrixf(rotation)

        glDepthMask(0)
        glDisable(GL_LIGHTING)
        BasicRenderPass.render(self, rstate)
        glEnable(GL_LIGHTING)
        glDepthMask(1)
        glPopMatrix()


class ReflectionRenderPass(DecalRenderPass):
    """A rendering pass for reflections. Reflections are blended onto reflective
       objects in a separate pass, and require the texture matrix to be set up as
       the inverse of the modelview matrix's 3x3 top-left corner.
       """
    def filter(self, drawable):
        return drawable.render.reflection

    def render(self, rstate):
        # Set up the texture matrix as the modelview inverse
        m = glGetFloatv(GL_MODELVIEW_MATRIX)
        glMatrixMode(GL_TEXTURE)
        m = LinearAlgebra.inverse(m)
        m[3][0] = 0
        m[3][1] = 0
        m[3][2] = 0
        glLoadMatrixf(m)
        glMatrixMode(GL_MODELVIEW)

        # Set up texture coordinate generation
        glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE)
        glTexGenfv(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_EXT)
        glTexGenfv(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_EXT)
        glTexGenfv(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_EXT)
        glEnable(GL_TEXTURE_GEN_S)
        glEnable(GL_TEXTURE_GEN_T)
        glEnable(GL_TEXTURE_GEN_R)

        # We're blending on top of existing polygons, so use the same tricks as the decal pass
        DecalRenderPass.render(self, rstate)

        # Clean up
        glMatrixMode(GL_TEXTURE)
        glLoadIdentity()
        glMatrixMode(GL_MODELVIEW)
        glDisable(GL_TEXTURE_GEN_S)
        glDisable(GL_TEXTURE_GEN_T)
        glDisable(GL_TEXTURE_GEN_R)


class CameraRenderPass(BasicRenderPass):
    """A rendering pass for any camera-induced artifacts. Most of these will be
       lense flares, but this could include things such as the cracked glass effect
       from BZFlag proper
       """
    def filter(self, drawable):
        return drawable.render.camera

    def render(self, rstate):
        # Don't render camera effects into environment maps
        if rstate.cubeMap:
            return

        glPushMatrix()
        # Strip out all but the top 3x3 square of the modelview matrix.
        # This preserves the camera's location, but keeps it centered at 0,0,0
        rotation = Numeric.identity(4, Numeric.Float32)
        rotation[:3,:3] = glGetFloatv(GL_MODELVIEW_MATRIX)[:3,:3]
        rotation[3,3] = 1
        glLoadMatrixf(rotation)

        BasicRenderPass.render(self, rstate)

        glPopMatrix()


def getRenderPasses():
    """Return a list of freshly created render pass instances. This function serves mostly
       as a way to keep the list of render passes and the priorities for each in one place,
       to make it easier to see the big picture regarding how the various passes interoperate.
       """
    return [
        BackgroundRenderPass (filterPriority= 20, renderPriority=110),
        BasicRenderPass      (filterPriority=  0, renderPriority=100),
        ReflectionRenderPass (filterPriority= 10, renderPriority= 95),
        DecalRenderPass      (filterPriority= 10, renderPriority= 90),
        BlendedRenderPass    (filterPriority= 10, renderPriority= 80),
        OverlayRenderPass    (filterPriority= 20, renderPriority= 10),
        CameraRenderPass     (filterPriority= 20, renderPriority=  5),
        ]

### The End ###

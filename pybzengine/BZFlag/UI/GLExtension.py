""" BZFlag.UI.GLExtension

This module supports querying OpenGL for support of various extensions,
then containing those query results in an easy to find place.
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

# Detected settings. All default to None so it will be easier to detect
# if a value is being used before the test has been performed.
multitexture = None
textureUnits = None
maxAnisotropy = None
cubeMap = None
textureTargets = None
nvPointSprite = None


def test():
    """Test for extension support."""
    import OpenGL.GL.ARB.multitexture
    from OpenGL.GL.EXT import texture_filter_anisotropic
    from OpenGL.GL.EXT import texture_cube_map
    from OpenGL import GL

    # Determine whether we have multitexturing
    global multitexture
    multitexture = OpenGL.GL.ARB.multitexture.glInitMultitextureARB()

    # If we have multitexturing, make a list of supported texture units
    global textureUnits
    if multitexture:
        try:
            textureUnits = []
            unit = OpenGL.GL.ARB.multitexture.GL_TEXTURE0_ARB
            while True:
                OpenGL.GL.ARB.multitexture.glActiveTextureARB(unit)
                textureUnits.append(unit)
                unit += 1
        except:
            pass
        try:
            OpenGL.GL.ARB.multitexture.glActiveTextureARB(textureUnits[0])
        except:
            pass

        # If we have less than 2 multitexture units, declare it broken
        if len(textureUnits) < 2:
            multitexture = False

    # Determine whether we have anisotropic filtering
    global maxAnisotropy
    try:
        maxAnisotropy = GL.glGetFloatv(texture_filter_anisotropic.GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT)
    except:
        maxAnisotropy = 1.0

    # Start a list of supported texture targets, we'll add to it later
    global textureTargets
    textureTargets = [GL.GL_TEXTURE_1D, GL.GL_TEXTURE_2D]

    # Test for the cube map extension
    global cubeMap
    texture_cube_map.glInitTextureCubeMapEXT()
    cubeMap = GL.glGetString(GL.GL_EXTENSIONS).find("GL_EXT_texture_cube_map") >= 0
    if cubeMap:
        textureTargets.append(texture_cube_map.GL_TEXTURE_CUBE_MAP_EXT)

    # Test for the NVidia point sprite extension
    global nvPointSprite
    nvPointSprite = GL.glGetString(GL.GL_EXTENSIONS).find("GL_NV_point_sprite") >= 0


def disableMultitex():
    """Turn off all texture units except the first, and select the first.
       If multitexture isn't supported, this has no effect.
       """
    from OpenGL.GL.ARB.multitexture import glActiveTextureARB
    from OpenGL.GL import glDisable, GL_TEXTURE_2D
    if not multitexture:
        return
    for unit in textureUnits[1:]:
        glActiveTextureARB(unit)
        for target in textureTargets:
            glDisable(target)
    glActiveTextureARB(textureUnits[0])

### The End ###

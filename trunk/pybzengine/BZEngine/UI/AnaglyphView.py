""" BZEngine.UI.AnaglyphView

A 3d scene renderer that does anaglyph stereo
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

import math
from BZEngine.UI.ThreeDRender import View
from BZEngine.UI import Drawable
from OpenGL.GL import *

class AnaglyphView(View):
    """Shows an anaglyph-stereo 3d view"""
    def __init__(self, viewport, scene=None):
        View.__init__(self, viewport, scene)

    def render(self):
        """Render the view to the given surface. This includes the game
           world, with transient objects such as players and flags
           """
	rstate = Drawable.RenderState(self)
        eyesep = 25
        angle = math.atan(eyesep / self.camera.distance)
        # draw left eye (blue-green)
        self.camera.azimuthOffset = -angle
        glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE)
        self.camera.load()
        for light in self.lights:
            light.set()
        self.scene.render(rstate)
        glClear(GL_DEPTH_BUFFER_BIT)
        # draw right eye (red)
        self.camera = self.camera
        self.camera.azimuthOffset = angle
        glColorMask(GL_FALSE, GL_TRUE, GL_TRUE, GL_FALSE)
        self.camera.load()
        for light in self.lights:
            light.set()
        self.scene.render(rstate)
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE)

### The End ###

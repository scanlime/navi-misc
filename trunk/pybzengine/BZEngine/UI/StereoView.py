""" BZFlag.UI.StereoView

A 3d scene renderer that does side-by-side 2-channel stereo (usually for SGI
visualization hardware)
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

from BZFlag.UI import Viewport, ThreeDView, ThreeDControl, ThreeDRender, Layout, HUD
import BZFlag, math
from OpenGL.GL import *

class EyeCamera(ThreeDRender.Camera):
    """A camera that will track the location of a master camera, offsetting its azimuth
       to simulate an eye separation of the given amount.
       """
    def __init__(self, masterView, eyesep):
        self.masterView = masterView
        self.eyesep = eyesep

    def load(self):
        master = getattr(self.masterView, 'camera')
        angle = math.atan(self.eyesep / master.distance)
        master.azimuthOffset = angle
        master.load()


class StereoView:
    def __init__(self, viewport, scene=None):
        if not scene:
            scene = ThreeDRender.Scene()
        self.scene = scene
        self.viewport = viewport
        self.camera = ThreeDRender.Camera()

        self.lefteye  = ThreeDRender.View(viewport.region(Layout.Rect(viewport).left(0.5)), self.scene)
        self.righteye = ThreeDRender.View(viewport.region(Layout.Rect(viewport).right(0.5)), self.scene)
        self.lefteye.camera  = EyeCamera(self, -30)
        self.righteye.camera = EyeCamera(self,  30)

        self.lefteye.viewport.onDrawFrame.observe(self.setupLeft)
        self.righteye.viewport.onDrawFrame.observe(self.setupRight)

    def setupLeft(self):
        if glGetBooleanv(GL_STEREO):
            glDrawBuffer(GL_LEFT_BACK)

    def setupRight(self):
        if glGetBooleanv(GL_STEREO):
            glDrawBuffer(GL_RIGHT_BACK)

### The End ###


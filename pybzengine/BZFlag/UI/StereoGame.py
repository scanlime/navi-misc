""" BZFlag.UI.StereoGame

A 3d game renderer that does side-by-side 2-channel stereo
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

from BZFlag.UI import Viewport, ThreeDView, ThreeDControl, Layout
from BZFlag.UI.StereoView import RightCamera
import BZFlag, math


class StereoGame:
    """Shows a side-by-side stereo view of the BZFlag game, renderable
       to an OpenGLViewport.
       """
    def __init__(self, game, viewport):
        self.lefteye  = ThreeDView.ThreeDView(game, viewport.region(Layout.Rect(viewport).left(0.5)))
        self.righteye = ThreeDView.ThreeDView(game, viewport.region(Layout.Rect(viewport).right(0.5)))
        ThreeDControl.Viewing(self.lefteye, viewport)
        self.righteye.camera = RightCamera(self.lefteye.camera, 30)


class Setup:
    def __init__(self, game, eventLoop):
        self.viewport = Viewport.OpenGLViewport(eventLoop, (1600, 600))
        self.view = StereoGame(game, self.viewport)

### The End ###

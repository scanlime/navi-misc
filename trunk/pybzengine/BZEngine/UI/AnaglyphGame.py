""" BZFlag.UI.AnaglyphGame

A 3d game renderer that does anaglyph stereo
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
import pygame, BZFlag
from pygame.locals import *
from BZFlag.UI.AnaglyphView import *
from BZFlag.UI.ThreeDView import Scene
import sys

class AnaglyphGame(AnaglyphView):
    """Shows an anaglyph-stereo 3d view of the BZFlag game, renderable to an
       OpenGLViewport. Probably not too useful, as anaglyph gives people headaches fast
       """
    def __init__(self, game, viewport):
        AnaglyphView.__init__(self, viewport, Scene(game))
        viewport.setCaption("%s Anaglyph View" % BZFlag.name)

        viewport.onDrawFrame.observe(game.update)


class Setup:
    def __init__(self, game, eventLoop):
        from BZFlag.UI import Viewport, ThreeDControl
        self.viewport = Viewport.OpenGLViewport(eventLoop)
        self.view = AnaglyphGame(game, self.viewport)
        self.control = ThreeDControl.Viewing(self.view, self.viewport)

### The End ###

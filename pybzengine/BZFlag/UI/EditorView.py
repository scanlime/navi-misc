""" BZFlag.UI.EditorView

Provides a UI for editing BZFlag worlds, built mostly upon ThreeDView
and ThreeDControl.Editing.
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

from BZFlag.UI import Viewport, ThreeDView, ThreeDControl
from BZFlag.UI.Drawable import TestWidget


class Widgetness:
    def __init__(self):
        self.drawables = None


    def getGLDrawables(self, position):
        from BZFlag.UI import Drawable
        if self.drawables == None:
            self.drawables = [TestWidget(position)]
            for drawable in self.drawables:
                drawable.parent(self)
        return self.drawables


class Setup:
    def __init__(self, game, eventLoop):
        self.viewport = Viewport.OpenGLViewport(eventLoop)

        self.view   = ThreeDView.ThreeDView(game, self.viewport)
        widget = Widgetness()
        self.view.scene.objects[widget] = widget.getGLDrawables((4.5, -3.5, -10))
        widget = Widgetness()
        self.view.scene.objects[widget] = widget.getGLDrawables((-4.5, -3.5, -10))
        widget = Widgetness()
        self.view.scene.objects[widget] = widget.getGLDrawables((4.5, 3.5, -10))
        widget = Widgetness()
        self.view.scene.objects[widget] = widget.getGLDrawables((-4.5, 3.5, -10))
        ThreeDControl.Editing(self.view, self.viewport)

### The End ###

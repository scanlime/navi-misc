""" BZFlag.Base

BZFlag.Object implementing a base.
"""
#
# Copyright (C) 2005 David Trowbridge
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#
from Box import Box
from Object import createTexture
import Object
import Blender

try:
    boxWallMaterial = Blender.Material.Get('BoxWall')
except:
    boxWallMaterial = createTexture('BoxWall', '/usr/share/bzedit/boxwall.png')

class Base(Box):
    type = 'base'

    def __init__(self, list=None):
        # Load defaults
        self.set_color()
        Box.__init__(self, list)

    def set_color(self, color=1):
        self.color = color

    def setBlenderProperties(self, object):
        Box.setBlenderProperties(self, object)
        object.addProperty('color', self.color, 'INT')

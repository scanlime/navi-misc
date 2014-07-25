""" BZFlag.Teleporter

BZFlag.Object implementing a teleporter.
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

from Object import createTexture
from Box import Box
import Blender

try:
    borderMaterial = Blender.Material.Get('TeleporterBorder')
    fieldMaterial = Blender.Material.Get('TeleporterField')
except:
    borderMaterial = createTexture('TeleporterBorder', '/usr/share/bzedit/caution.png')
    fieldMaterial = Blender.Material.New('TeleporterField')
    fieldMaterial.rgbCol = [0.0, 0.0, 0.0]
    fieldMaterial.alpha = 0.6
    fieldMaterial.mode |= Blender.Material.Modes['ZTRANSP']

class Teleporter(Box):
    type = 'teleporter'
    verts = [(0,  1, 1),
             (0, -1, 1),
             (0, -1, 0),
             (0,  1, 0),
            ]
    faces = [(0, 1, 2, 3)]

    materials = [fieldMaterial, borderMaterial]
    materialIndex = [0]

    def __init__(self, list=None):
        self.set_border()
        if list:
            if type(list[1]) is str:
                Box.__init__(self, [list[0]] + list[2:])
                self.set_name(list[1])
            else:
                Box.__init__(self, list)
        else:
            Box.__init__(self)

    def serialize(self, writer):
        Box.serialize(self, writer)
        writer(('border', self.border))

    def set_size(self, size=[1, 4.48, 20.16]):
        self.size = [1.0] + [float(n) for n in size[1:]]

    def set_border(self, border=[1.0]):
        self.border = float(border[0])

    def setBlenderProperties(self, object):
        Box.setBlenderProperties(self, object)
        object.addProperty('border', self.border, 'FLOAT')

    def loadBlenderProperties(self, object):
        Box.loadBlenderProperties(self, object)
        try:
            self.border = object.getProperty('border').getData()
        except AttributeError:
            # No property, set default
            self.border = 1.0

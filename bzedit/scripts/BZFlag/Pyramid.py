""" BZFlag.Pyramid

BZFlag.Object implementing a pyramid.
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
import Object
import Blender

try:
    pyramidMaterial = Blender.Material.Get('Pyramid')
except NameError:
    pyramidMaterial = Object.createTexture('Ground', '/usr/share/bzedit/pyrwall.png')

# FIXME - flipz
class Pyramid(Box):
    type = 'pyramid'

    verts = [( 1,  1, 0),
             (-1,  1, 0),
             (-1, -1, 0),
             ( 1, -1, 0),
             ( 0,  0, 1),
            ]

    faces = [(0, 4, 3),    # X+
             (2, 4, 1),    # X-
             (1, 4, 0),    # Y+
             (3, 4, 2),    # Y-
             (0, 1, 2, 3), # Z-
            ]

    materials = [pyramidMaterial]
    materialIndex = [0, 0, 0, 0, 0]

    def set_size(self, size=[8.2, 8.2, 10.25]):
        self.size = [float(n) for n in size]

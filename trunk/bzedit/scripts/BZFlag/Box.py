""" BZFlag.Box

Subclass of BZFlag.Object implementing a Box.
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

from Object import Object

class Box(Object):
    type = 'box'

    verts = [( 1,  1, 1),
             ( 1,  1, 0),
             ( 1, -1, 1),
             ( 1, -1, 0),
             (-1,  1, 1),
             (-1,  1, 0),
             (-1, -1, 1),
             (-1, -1, 0),
            ]
    faces = [(0, 1, 3, 2), # X+
             (6, 7, 5, 4), # X-
             (4, 5, 1, 0), # Y+
             (2, 3, 7, 6), # Y-
             (0, 2, 6, 4), # Z+
             (1, 5, 7, 3), # Z-
            ]

    def __init__(self):
        # Load defaults
        self.drivethrough = 0
        self.shootthrough = 0
        self.position = [0, 0, 0]
        self.size = [10, 10, 9.42]
        self.rotation = 0
        self.scale = [1, 1, 1]
        self.shear = [0, 0, 0]
        self.spin = [0, 0, 0, 0]

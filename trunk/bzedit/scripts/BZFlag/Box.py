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

    def __init__(self, list=None):
        # Load defaults
        self.drivethrough = 0
        self.shootthrough = 0
        self.set_position()
        self.set_size()
        self.set_rotation()
        self.set_scale()
        self.set_shear()
        self.set_spin()
        if list is not None:
            for property in list[1:]:
                getattr(self, "set_%s" % property[0])(list[1:])

    def set_drivethrough(self):
        self.drivethrough = 1

    def set_shootthrough(self):
        self.shootthrough = 1

    def set_position(self, pos=[0, 0, 0]):
        self.position = [float(n) for n in pos]

    def set_size(self, size=[10, 10, 9.42]):
        self.size = [float(n) for n in size]

    def set_rotation(self, *r):
        self.rotation = float(r[0])

    def set_scale(self, scale=[1, 1, 1]):
        self.scale = [float(n) for n in scale]

    def set_shear(self, shear=[0, 0, 0]):
        self.shear = [float(n) for n in shear]

    def set_spin(self, spin=[0, 0, 0, 0]):
        self.spin = [float(n) for n in spin]

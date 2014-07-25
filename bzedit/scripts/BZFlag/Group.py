""" BZFlag.Group

Subclass of BZFlag.Object implementing a group instance.
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
import Blender

class Group(Box):
    type = 'group'

    def __init__(self, list=None):
        Box.__init__(self, None)
        self.set_size([1.0, 1.0, 1.0])
        if list is not None:
            self.set_instance(list[1])

    def set_instance(self, name):
        self.instance = name
        self.name = name+'_instance'

    def createBlenderObject(self):
        scene = Blender.Scene.GetCurrent()

        obj = Blender.Object.New('Empty')
        scene.link(obj)
        obj.setName(self.name)

        # hrm, this is gonna be ugly
        world_objects = Blender.Object.Get()
        print world_objects

        return obj

    def transformBlenderObject(self, obj):
        # WorldFileLocation applies transformations in the order shift, scale, shear, spin
        # Technically old stuff like position, rotation are supported here, but for now I'm
        # not doing them, 'cuz they suck
        mat = Blender.Mathutils.Matrix(
                [1, 0, 0, 0],
                [0, 1, 0, 0],
                [0, 0, 1, 0],
                [0, 0, 0, 1],
                )

        # apply the world's size transformation
        transform = self.world.getBzToBlendMatrix()
        transform.resize4x4()
        mat *= transform

        mat *= Blender.Mathutils.TranslationMatrix(Blender.Mathutils.Vector(list(self.shift)))
        mat *= Blender.Mathutils.ScaleMatrix(self.scale[0], 4, Blender.Mathutils.Vector([1, 0, 0]))
        mat *= Blender.Mathutils.ScaleMatrix(self.scale[1], 4, Blender.Mathutils.Vector([0, 1, 0]))
        mat *= Blender.Mathutils.ScaleMatrix(self.scale[2], 4, Blender.Mathutils.Vector([0, 0, 1]))
        # mat *= Blender.Mathutils.ShearMatrix(Blender.Mathutils.Vector(shear))
        mat *= Blender.Mathutils.RotationMatrix(self.spin[3], 4, 'r', Blender.Mathutils.Vector(list(self.spin[:3])))

        obj.setMatrix(mat)

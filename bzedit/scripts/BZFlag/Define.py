""" BZFlag.Define

Subclass of BZFlag.Object implementing group definitions.
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
import Blender

class Define(Object):
    type = 'define'

    def __init__(self, list=None):
        self.set_name(list[1])
        self.objects = []

        from Types import typeMap

        children = list[2:len(list) - 1]
        if len(children):
            for child in children:
                objtype = typeMap[child[0]]
                if objtype is not None:
                    self.objects.append(objtype(child))
                else:
                    print 'unable to create object "%s"' % child[0]

    def toBlender(self):
        obj = Object.toBlender(self)
        for object in self.objects:
            object.world = self.world
        self.children = [x.toBlender() for x in self.objects]
        obj.makeParent(self.children, 0, 0)

        obj.setName(self.name)

        scene = Blender.Scene.GetCurrent()
        transform = self.world.getBzToBlendMatrix()
        transform.resize4x4()

        # move this object and all its children to layer 20
        for child in self.children:
            child.Layer = 1 << 19
        scene.link(obj)
        obj.Layer = 1 << 19
        return obj

    def createBlenderObject(self):
        return Blender.Object.New('Empty')

    def transformBlenderObject(self, obj):
        # FIXME - do we need this?
        transform = self.world.getBzToBlendMatrix()
        transform.resize4x4()
        obj.setMatrix(transform)

""" BZFlag.Mesh

BZFlag.Object implementing a mesh.
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

class Mesh(Object):
    type = 'mesh'

    verts = []
    faces = []

    def __init__(self, list=None):
        if list is not None:
            for property in list[1:]:
                try:
                    getattr(self, "set_%s" % property[0])(property[1:])
                except Exception, e:
                    print 'error!',property[0],e

    def set_face(self, face):
        pass

    def set_vertex(self, vertex):
        self.verts.append([float(n) for n in vertex])

    def set_normal(self, normal):
        pass

    def set_texcoord(self, texcoord):
        pass

    def set_matref(self, matref):
        pass

    def transformBlenderObject(self, obj):
        """Set the transformation on the given Blender object
           to match our position, size, and rotation. This will
           be used both by the Box object and by other objects
           with similar interfaces that subclass Box.
           """
        if not self.world:
            return

        transform = self.world.getBzToBlendMatrix()
        transform.resize4x4()
        transform *= Blender.Mathutils.TranslationMatrix(self.world.blendObject.mat.translationPart())
        obj.setMatrix(transform)

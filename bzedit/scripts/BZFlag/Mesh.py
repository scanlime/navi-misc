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

class Face:
    vertices = []
    normals = []
    texcoords = []

    def __init__(self, list=None):
        if list is not None:
            for property in list:
                getattr(self, "set_%s" % property[0])(property[1:])

    def set_vertices(self, vertices):
        self.vertices = [int(n) for n in vertices]

    def set_normals(self, normals):
        self.normals = [int(n) for n in normals]

    def set_texcoords(self, texcoords):
        self.texcoords = [int(n) for n in texcoords]

    def set_drivethrough(self, drivethrough):
        pass

    def set_shootthrough(self, shootthrough):
        pass

    def set_smoothbounce(self, smoothbounce):
        pass

    def set_noclusters(self, noclusters):
        pass

    def set_passable(self, passable):
        pass

    def set_matref(self, matref):
        pass

    def set_phydrv(self, phydrv):
        pass

class Mesh(Object):
    type = 'mesh'

    def __init__(self, list=None):
        self.verts = []
        self.faces = []
        if list is not None:
            for property in list[1:]:
                    getattr(self, "set_%s" % property[0])(property[1:])
        if not self.name:
            self.name = "Mesh"

    def serialize(self, writer):
        Object.serialize(self, writer)

    def set_face(self, face):
        f = Face(face)
        self.faces.append(f.vertices)

    def set_inside(self, inside):
        pass

    def set_outside(self, outside):
        pass

    def set_vertex(self, vertex):
        self.verts.append([float(n) for n in vertex])

    def set_normal(self, normal):
        pass

    def set_texcoord(self, texcoord):
        pass

    def set_smoothbounce(self, s):
        pass

    def set_passable(self, p):
        pass

    def set_matref(self, matref):
        pass

    def set_noclusters(self, noclusters):
        pass

    def set_phydrv(self, phydrv):
        pass

    def set_drivethrough(self, drivethrough):
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

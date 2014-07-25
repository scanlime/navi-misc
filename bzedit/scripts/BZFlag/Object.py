""" BZFlag.Object

Abstract base class for BZFlag objects. Each object has a named type,
can serialize itself and be converted to/from blender objects.
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
from Blender import NMesh, Material, Image, Texture
import string

def createTexture(name, filename):
    material = Material.New(name)
    texture = Texture.New(name)
    image = Image.Load(filename)
    texture.image = image
    texture.setType('Image')
    material.setTexture(0, texture, Texture.TexCo['OBJECT'])
    material.spec = 0.1
    return material

def meshify(vertex, face, material, name=None):
    mesh = NMesh.GetRaw()
    verts = []
    for v in vertex:
        v = NMesh.Vert(v[0], v[1], v[2])
        mesh.verts.append(v)
        verts.append(v)
    for f in face:
        face = NMesh.Face()
        for v in f:
            face.v.append(verts[v])
            if len(material) > v:
                face.materialIndex = material[v]
        mesh.faces.append(face)
    object = NMesh.PutRaw(mesh)
    for i in range(len(material)):
        object.colbits |= 1 << i
    if name:
        object.setName(name)
    return object

class Object:
    type = None
    comment = None
    name = None
    blendObject = None

    verts = []
    faces = []
    materials = []
    materialIndex = []

    # If a world is associated with this object, it will be used
    # in computing transformations between blender coordinates
    # and bzflag coordinates such that all other objects are positioned
    # relative to the world
    world = None

    def serialize(self, writer):
        if (self.name):
            writer(('name', self.name))

    def set_name(self, name):
        if type(self.name) is str:
            self.name = name
        else:
            self.name = string.join(name, ' ')

    def toBlender(self):
        """Create a new blender object representing this one and return it."""
        obj = self.createBlenderObject()
        self.transformBlenderObject(obj)
        self.setBlenderProperties(obj)
        self.blendObject = obj
        return obj

    def fromBlender(self, object):
        """Load this object's settings from the given Blender object.
           The default implementation uses loadBlenderTransform() to retrieve
           information from the object's location and loadBlenderProperties
           to retrieve property info.
           """
        self.blendObject = object
        self.loadBlenderTransform(object)
        self.loadBlenderProperties(object)

    def createBlenderObject(self):
        """Create a new blender object representing this BZFlag object.
           The default implementation creates a mesh using our 'verts',
           'faces', and 'materials' attributes.
           """
        obj = meshify (self.verts, self.faces, self.materialIndex, self.name)
        obj.setMaterials(self.materials)
        return obj

    def transformBlenderObject(self, object):
        """Transform the given Blender object using our BZFlag properties.
           This must be implemented by subclasses in an object-dependent way.
           """
        pass

    def loadBlenderTransform(self, object):
        """The inverse of transformBlenderObject- looks at an existing
           Blender object, and figures out our object-specific BZFlag properties.
           """
        pass

    def setBlenderProperties(self, object):
        """This sets extra properties on our corresponding Blender object.
           The default implementation only sets 'bztype', but subclasses
           may add more properties to this.
           """
        object.addProperty('bztype', self.type, 'STRING')
        if self.name:
            object.addProperty('name', self.name, 'STRING')

    def loadBlenderProperties(self, object):
        """The inverse of setBlenderProperties, loading info from the Blender
           object's extra properties.
           """
        try:
            self.name = object.getProperty('name').getData()
        except AttributeError:
            self.name = None

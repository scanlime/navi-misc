""" BZFlag.World

Subclass of BZFlag.Object implementing the world.
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

class World(Object):
    """Represents the size of the bzflag world and other global attributes"""
    type = 'world'

    verts = [( 1,  1, 0),
             (-1,  1, 0),
             (-1, -1, 0),
             ( 1, -1, 0),
            ]

    faces = [(0, 1, 2, 3)]

    def __init__(self):
        self.set_size()
        self.name = ''

    def set_size(self, size=400)
        self.size = size

    def transformBlenderObject(self, world):
        # The world always appears to be a constant size in blender, and other objects
        # are measured relative to it. The world's actual size in bzflag units comes
        # from it's 'size' property. This default size matches blender's grid.
        world.setSize(16, 16, 16)
        world.setLocation(0, 0, 0)

    def setBlenderProperties(self, object):
        Object.setBlenderProperties(self, object)
        object.addProperty('size', float(self.size), 'FLOAT')
        if self.name != '':
            object.setName(self.name)

    def loadBlenderProperties(self, object):
        Object.loadBlenderProperties(self, object):
        try:
            self.size = object.getProperty('size').getData()
        except AttributeError:
            # use the default size
            self.set_size()
        self.name = object.getName()

    def getBzToBlendMatrix(self):
        """Get a 3x3 matrix that converts BZFlag coordinates to Blender coordinates
           relative to this world. Requires that the world have an associated
           Blender object.
           """
        scale = 1.0 / self.size
        return self.blendObject.mat.rotationPart() * scale

    def getBlendToBzMatrix(self):
        """Get a 3x3 matrix that converts Blender coordintes back to BZFlag coordinates,
           relative to this world. Requires that the world have an associated
           Blender object.
           """
        inv = self.getBzToBlendMatrix()
        inv.invert()
        return inv

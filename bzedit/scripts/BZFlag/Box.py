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

from Object import Object, createTexture
import math
import Blender

try:
    boxTopMaterial = Blender.Material.Get('BoxTop')
    boxWallMaterial = Blender.Material.Get('BoxWall')
except:
    boxTopMaterial = createTexture('BoxTop', '/usr/share/bzedit/tetrawall.png')
    boxWallMaterial = createTexture('BoxWall', '/usr/share/bzedit/boxwall.png')

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

    materials = [boxWallMaterial, boxTopMaterial]
    materialIndex = [0, 0, 0, 0, 1, 1]

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
        self.set_shift()
        if list is not None:
            for property in list[1:]:
                getattr(self, "set_%s" % property[0])(property[1:])
        if not self.name:
            self.name = 'Box'

    def serialize(self, writer):
        Object.serialize(self, writer)
        writer(('position',) + tuple(self.position))
        writer(('size',) + tuple(self.size))
        writer(('rotation', self.rotation))
        writer(('scale',) + tuple(self.scale))
        writer(('shear',) + tuple(self.shear))
        writer(('spin',) + tuple(self.spin))
        if self.drivethrough == 1:
            writer('drivethrough')
        if self.shootthrough == 1:
            writer('shootthrough')

    def set_drivethrough(self):
        self.drivethrough = 1

    def set_shootthrough(self):
        self.shootthrough = 1

    def set_position(self, pos=[0, 0, 0]):
        self.position = [float(n) for n in pos]

    def set_size(self, size=[10, 10, 9.42]):
        self.size = [float(n) for n in size]

    def set_rotation(self, r=[0]):
        self.rotation = float(r[0])

    def set_scale(self, scale=[1, 1, 1]):
        self.scale = [float(n) for n in scale]

    def set_shear(self, shear=[0, 0, 0]):
        self.shear = [float(n) for n in shear]

    def set_spin(self, spin=[1, 0, 0, 0]):
        self.spin = [float(n) for n in spin]

    def set_shift(self, shift=[0, 0, 0]):
        self.shift = [float(n) for n in shift]

    def setBlenderProperties(self, object):
        Object.setBlenderProperties(self, object)
        object.addProperty('drivethrough', self.drivethrough, 'INT')
        object.addProperty('shootthrough', self.shootthrough, 'INT')

    def loadBlenderProperties(self, object):
        Object.loadBlenderProperties(self, object)
        try:
            self.drive_through = object.getProperty('driveThrough').getData()
        except AttributeError:
            # No property, set default
            self.drive_through = 0

        try:
            self.shoot_through = object.getProperty('shootThrough').getData()
        except AttributeError:
            # No property, set default
            self.shoot_through = 0

    def transformBlenderObject(self, obj):
        """Set the transformation on the given Blender object
           to match our position, size, and rotation. This will
           be used both by the Box object and by other objects
           with similar interfaces that subclass Box.
           """
        if not self.world:
            return

        size = list(self.size)
        position = list(self.position)
        shift = list(self.shift)

        # Fix-up objects with negative scales - upside-down objects
        # are represented in blender with a positive scale and a rotation
        upsideDown = size[2] < 0
        if upsideDown:
            size[2] = -size[2]
            position[2] += size[2]

        mat = Blender.Mathutils.Matrix(
            [size[0], 0,       0,       0],
            [0,       size[1], 0,       0],
            [0,       0,       size[2], 0],
            [0,       0,       0,       1])

        theta = self.rotation * math.pi / 180.0
        cos = math.cos(theta)
        sin = math.sin(theta)
        mat *= Blender.Mathutils.Matrix(
            [ cos, sin, 0, 0],
            [-sin, cos, 0, 0],
            [ 0,   0,   1, 0],
            [ 0,   0,   0, 1])

        if upsideDown:
            # 180 degree rotation around the X axis
            mat *= Blender.Mathutils.Matrix(
                [0, 1,  0, 0],
                [1, 0,  0, 0],
                [0, 0, -1, 0],
                [0, 0,  0, 1])

        mat *= Blender.Mathutils.TranslationMatrix(Blender.Mathutils.Vector(position))
        transform = self.world.getBzToBlendMatrix()
        transform.resize4x4()
        mat *= transform

        mat *= Blender.Mathutils.TranslationMatrix(self.world.blendObject.mat.translationPart())

        # spin
        mat *= Blender.Mathutils.RotationMatrix(self.spin[3], 4, 'r', Blender.Mathutils.Vector(list(self.spin[:3])))

        # shear
        # mat *= Blender.Mathutils.ShearMatrix(Blender.Mathutils.Vector(shear))

        # shift
        mat *= Blender.Mathutils.TranslationMatrix(Blender.Mathutils.Vector(shift))

        # scale
        mat *= Blender.Mathutils.ScaleMatrix(self.scale[0], 4, Blender.Mathutils.Vector([1, 0, 0]))
        mat *= Blender.Mathutils.ScaleMatrix(self.scale[1], 4, Blender.Mathutils.Vector([0, 1, 0]))
        mat *= Blender.Mathutils.ScaleMatrix(self.scale[2], 4, Blender.Mathutils.Vector([0, 0, 1]))

        obj.setMatrix(mat)

    def loadBlenderTransform(self, obj):
        """Retrieves the object's position, size, and rotation
           from a Blender object- the inverse of transformBlenderObject().
           """

        # Before anything else, subtract the world origin
        mat = obj.mat * Blender.Mathutils.TranslationMatrix(self.world.blendObject.mat.translationPart() * -1.0)

        # Convert to BZFlag coordinates, relative to the World object
        transform = self.world.getBlendToBzMatrix()
        transform.resize4x4()
        mat *= transform

        # Extract translation, leave a 3x3 matrix with rotation and scale
        self.position = list(mat.translationPart())
        mat = mat.rotationPart()

        # Extract the rotation using blender's handy quaternion-fu
        euler = mat.toEuler()
        self.rotation = euler[2]

        # We could determine scale by multiplying our three basis vectors
        # each by the matrix and measuring their length. This is the same
        # as treating each column of the 3x3 matrix as a vector and taking
        # their length.
        self.size = [
                     math.sqrt(mat[0][0]**2 + mat[0][1]**2 + mat[0][2]**2),
                     math.sqrt(mat[1][0]**2 + mat[1][1]**2 + mat[1][2]**2),
                     math.sqrt(mat[2][0]**2 + mat[2][1]**2 + mat[2][2]**2)
                    ]

        # There are only two allowed orientations in bzflag:
        # (0, 0, x) and (180, 0, x). We don't strictly enforce these,
        # but we do assume that if euler[0] > 90, it's in this second
        # orientation and it's an upside-down object. This only makes
        # sense in the case of pyramids- the origin is moved back to the
        # bottom, and the Z size is flipped.
        if euler[0] > 90 or euler[0] < -90:
            self.position[2] -= self.size[2]
            self.size[2] *= -1

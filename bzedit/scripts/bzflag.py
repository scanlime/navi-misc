#
# bzflag.py - This is a module shared by all blender scripts,
#             providing a library of bzflag world objects that
#             can be serialized/deserialized and converted
#             to/from blender objects.
#
# Blender-based BZFlag World Editor
# Copyright (C) 2004-2005 David Trowbridge and Micah Dowty
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

import Blender
from Blender import NMesh, Material, Image, Texture
import math, string

# create textures
def createTexture(name, filename):
    material = Material.New(name)
    texture = Texture.New(name)
    image = Image.Load(filename)
    texture.image = image
    texture.setType('Image')
    material.setTexture(0, texture, Texture.TexCo['OBJECT'])
    material.spec = 0.1
    return material

try:
    boxTopMaterial = Material.Get('BoxTop')
except NameError:
    boxTopMaterial = createTexture('BoxTop', '/usr/share/bzedit/tetrawall.png')

try:
    boxWallMaterial = Material.Get('BoxWall')
except NameError:
    boxWallMaterial = createTexture('BoxWall', '/usr/share/bzedit/boxwall.png')

try:
    groundMaterial = Material.Get('Ground')
except NameError:
    groundMaterial = createTexture('Ground', '/usr/share/bzedit/std_ground.png')

try:
    pyramidMaterial = Material.Get('Pyramid')
except NameError:
    pyramidMaterial = createTexture('Ground', '/usr/share/bzedit/pyrwall.png')

try:
    teleporterFieldMaterial = Material.Get('TeleporterField')
except NameError:
    teleporterFieldMaterial = Material.New('TeleporterField')
    teleporterFieldMaterial.rgbCol = [0.0, 0.0, 0.0]
    teleporterFieldMaterial.alpha = 0.6
    teleporterFieldMaterial.mode |= Material.Modes['ZTRANSP']

try:
    teleporterBorderMaterial = Material.Get('TeleporterBorder')
except NameError:
    teleporterBorderMaterial = createTexture('TeleporterBorder', '/usr/share/bzedit/caution.png')

def meshify(vertex, face, material, name = None):
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


class CommentLine(str):
    """Placeholder for a comment line, loaded or saved in a BZFlag world"""
    pass


class Logger:
    """This is a singleton that collects error and warning messages, presenting
       them to the user in batches.
       """
    def __init__(self):
        self.clear()

    def clear(self):
        self.log = []
        self.numErrors = 0
        self.numMessages = 0
        self.numWarnings = 0

    def msg(self, s):
        """Log a nonfatal message"""
        self.log.append(s)
        self.numMessages += 1

    def err(self, s):
        """Log an error"""
        self.log.append("ERROR: %s" % s)
        self.numErrors += 1

    def warn(self, s):
        """Log a nonfatal warning"""
        self.log.append("Warning: %s" % s)
        self.numWarnings += 1

    def report(self, title="Errors reported"):
        """Report errors to the user"""
        Blender.Draw.PupMenu("%s:%%t|%s" % (title, "|".join(self.log)))
        self.clear()

log = Logger()


class WorldReader:
    """Reads lines, blocks, or objects from a BZFlag world file utilizing generators"""
    def __init__(self, filename):
        self.stream = open(filename)

    def readLines(self):
        """Iterates over the lines of content in a BZFlag world.
           This returns a tuple for most lines, or a CommentLine
           instance to represent comments.
           """
        for line in self.stream:
            line = line.strip()
            if line:
                if line[0] == '#':
                    yield CommentLine(line[1:].strip())
                if line.split()[0] == 'name':
                    yield line.split()
                else:
                    # Split the line into tokens, converting them to
                    # floating point numbers where possible.
                    tokens = []
                    for token in line.split():
                        try:
                            tokens.append(float(token))
                        except ValueError:
                            if token:
                                tokens.append(token)
                    yield tuple(tokens)

    def readBlocks(self):
        """Iterates over the blocks in a BZFlag world. For
           each block, this yields a (type, iterator) tuple.
           The iterator returns all lines within the block,
           and must be exhausted before moving on to the
           next block.
           """
        lineIter = self.readLines()
        for line in lineIter:
            # Ignore comments outside of blocks, ignore
            # odd-looking lines.
            if type(line) == tuple and len(line) == 1:
                yield line[0], self._readBlockContent(lineIter)

    def _readBlockContent(self, lines):
        """Used internally in readBlocks, this yields all lines
           from the 'lines' iterator until it sees an 'end'.
           """
        for line in lines:
            if line == ("end",):
                break
            else:
                yield line

    def readObjects(self, typeRegistry=None):
        """Iterates over the BZObject instances in a bzflag world.
           Uses the provided ObjectTypeRegistry, or the default
           one if none is specified.
           """
        if not typeRegistry:
            typeRegistry = getTypeRegistry()

        for type, content in self.readBlocks():
            try:
                obj = typeRegistry.dict[type]()
            except KeyError:
                log.warn("skipping unknown object type %s in world" % type)
                for line in content:
                    pass
            else:
                obj.deserialize(content)
                yield obj


class WorldWriter:
    """Writes lines, blocks, or objects to a BZFlag world file using callbacks"""
    def __init__(self, filename):
        self.stream = open(filename, "w")
        self.writeHeading()

    def writeHeading(self):
        """Write a comment heading identifying the generator of this file"""
        self.stream.write("# Created with Blender\n")

    def writeLine(self, content):
        """Write one line to a BZFlag file. 'content' can be a CommentLine instance,
           a tuple, or None (for a blank line).
           """
        if content is None:
            self.stream.write("\n")
        elif type(content) is tuple:
            self.stream.write(" ".join([str(i) for i in content]) + "\n")
        elif isinstance(content, CommentLine):
            self.stream.write("# %s\n" % content)
        else:
            raise TypeError("Unsupported type in WorldWriter.writeLine()")

    def writeBlock(self, type, callback):
        """Write one block of data of the given type, using a callback to generate
           it. The callback is given a function to call to add lines to the block.
           """
        self.writeLine(None)
        self.writeLine((type,))
        callback(self.writeLine)
        self.writeLine(("end",))

    def writeObject(self, object):
        """Write a BZObject instance to the world file"""
        self.writeBlock(object.type, object.serialize)


class BZObject(object):
    """Abstract base class for BZFlag objects. Each BZFlag object
       has a named type, can serialize/deserialize itself, and
       can be converted to and from blender objects.
       """
    type = None
    comment = None
    name = None

    # Attributes controlling how this BZObject is represented in Blender
    verts = []
    faces = []
    materials = []
    materialIndex = []

    # If this BZObject has been associated with its Blender object,
    # that object should be accessable here.
    blendObject = None

    # If a world is associated with this object, it will be used
    # in computing transformations between blender coordinates
    # and bzflag coordinates such that all other objects are sized
    # and positioned relative to the world.
    world = None

    def set_name(self, *name):
        self.name = string.join(name, ' ')

    def serialize(self, writer):
        """Store this BZFlag object to a world file by calling the
           given writer for each tuple or comment to be included
           in the world block. The default implementation writes
           comments, but subclasses must add to this to write useful
           data.
           """
        if self.comment:
            for line in self.comment.split("\n"):
                writer(CommentLine(line))

    def deserialize(self, reader):
        """Read information for this BZFlag object from the given
           iterator. The iterator is expected to return either comment
           objects or tuples.
           The default implementation of this stores comment lines,
           and dispatches other types of lines to set_* handlers.
           """
        commentLines = []
        for line in reader:
            if isinstance(line, CommentLine):
                commentLines.append(str(line))
            else:
                f = getattr(self, "set_%s" % line[0], None)
                if f:
                    try:
                        f(*line[1:])
                    except TypeError:
                        # This probably means the number of arguments
                        # were wrong, but we don't verify this yet.
                        log.warn("line %r probably has the wrong number of parameters" % line)
                else:
                    log.warn("ignoring unknown line type %r in %r object" % (line[0], self.type))
        self.comment = "\n".join(commentLines)

    def toBlender(self):
        """Create a new Blender object representing this one,
           and return it.
           """
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
        obj = meshify(self.verts, self.faces, self.materialIndex, self.name)
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
        if self.comment:
            object.addProperty('comment', self.comment, 'STRING')
        if self.name:
            object.addProperty('name', self.name, 'STRING')

    def loadBlenderProperties(self, object):
        """The inverse of setBlenderProperties, loading info from the Blender
           object's extra properties.
           """
        try:
            self.comment = object.getProperty('comment').getData()
        except AttributeError:
            self.comment = None
        try:
            self.name = object.getProperty('name').getData()
        except AttributeError:
            self.name = None


class ObjectTypeRegistry:
    """This is a singleton that creates a mapping between BZFlag
       object types and the classes that implement those types.
       """
    def __init__(self, *classes):
        self.dict = {}
        for cls in classes:
            self.register(cls)

    def register(self, cls):
        """Register a single Packet subclass, a dictionary that may include
           Packet subclasses, or a module that may include Packet subclasses.
           """
        if type(cls) == type:
            # Register a single class
            self.dict[cls.type] = cls
        else:
            # Find subclasses and register recursively
            if type(cls) == dict:
                d = cls
            else:
                d = cls.__dict__
            for value in d.itervalues():
                if type(value) == type and issubclass(value, BZObject):
                    self.register(value)

    def fromBlender(self, object, world=None):
        """Find the proper BZObject subclass for the given Blender object,
           and load its parameters from the blender object. Returns a new
           BZObject instance.
           """
        bztype = object.getProperty('bztype').getData()
        bzo = self.dict[bztype]()
        bzo.world = world
        bzo.fromBlender(object)
        return bzo

_typeRegistry = None

def getTypeRegistry():
    """Return the ObjectTypeRegistry singleton for this module"""
    global _typeRegistry
    if not _typeRegistry:
        _typeRegistry = ObjectTypeRegistry(globals())
    return _typeRegistry


class World(BZObject):
    """Represents the size of the bzflag world and other global attributes"""
    type = 'world'

    verts = [( 1,  1, 0),
             (-1,  1, 0),
             (-1, -1, 0),
             ( 1, -1, 0),
             ]

    faces = [(0, 1, 2, 3)]

    materials = [groundMaterial]
    materialIndex = [0]

    def __init__(self):
        self.set_size()
        self.name = ''

    def set_size(self, size=400):
        self.size = size

    def serialize(self, writer):
        BZObject.serialize(self, writer)
        writer(("size", self.size))
        if self.name != '':
            writer(("name", self.name))

    def transformBlenderObject(self, world):
        # The world always appears to be a constant size in Blender, and other objects
        # are measured relative to it. The world's actual size in bzflag units
        # comes from its 'size' property. This default size matches Blender's grid.
        world.setSize(16, 16, 16)
        world.setLocation(0,0,0)

    def setBlenderProperties(self, object):
        BZObject.setBlenderProperties(self, object)
        object.addProperty('size', float(self.size), 'FLOAT')
        if self.name != '':
            object.setName(self.name)

    def loadBlenderProperties(self, object):
        BZObject.loadBlenderProperties(self, object)
        try:
            self.size = object.getProperty('size').getData()
        except AttributeError:
            # No property, use the default size
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


class Box(BZObject):
    """A rectangular prism, with translation and with rotation in the Z axis"""
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

    def __init__(self):
        # Load defaults
        self.drive_through = 0
        self.shoot_through = 0
        self.set_position()
        self.set_rotation()
        self.set_size()

    def set_position(self, x=0, y=0, z=0):
        self.position = [x,y,z]

    def set_rotation(self, degrees=0):
        self.rotation = degrees

    def set_size(self, x=10, y=10, z=9.42):
        self.size = [x,y,z]

    def set_drivethrough(self):
        self.drive_through = 1

    def set_shootthrough(self):
        self.shoot_through = 1

    def set_passable(self):
        self.drive_through = 1
        self.shoot_through = 1

    def serialize(self, writer):
        BZObject.serialize(self, writer)
        writer(("position",) + tuple(self.position))
        writer(("size",) + tuple(self.size))
        writer(("rotation", self.rotation))
        if (self.drive_through == 1):
            writer("drivethrough")
        if (self.shoot_through == 1):
            writer("shootthrough")

    def setBlenderProperties(self, object):
        BZObject.setBlenderProperties(self, object)
        object.addProperty('driveThrough', self.drive_through, 'INT')
        object.addProperty('shootThrough', self.shoot_through, 'INT')

    def loadBlenderProperties(self, object):
        BZObject.loadBlenderProperties(self, object)
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

        # Fixup objects with negative scales- upside-down objects
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

        mat *= Blender.Mathutils.TranslationMatrix(
            Blender.Mathutils.Vector(position))

        transform = self.world.getBzToBlendMatrix()
        transform.resize4x4()
        mat *= transform

        mat *= Blender.Mathutils.TranslationMatrix(
            self.world.blendObject.mat.translationPart())

        obj.setMatrix(mat)

    def loadBlenderTransform(self, obj):
        """Retrieves the object's position, size, and rotation
           from a Blender object- the inverse of transformBlenderObject().
           """
        # Before anything else, subtract the world origin
        mat = obj.mat * Blender.Mathutils.TranslationMatrix(
            self.world.blendObject.mat.translationPart() * -1.0)

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
            math.sqrt(mat[2][0]**2 + mat[2][1]**2 + mat[2][2]**2)]

        # There are only two allowed orientations in bzflag:
        # (0, 0, x) and (180, 0, x). We don't strictly enforce these,
        # but we do assume that if euler[0] > 90, it's in this second
        # orientation and it's an upside-down object. This only makes
        # sense in the case of pyramids- the origin is moved back to the
        # bottom, and the Z size is flipped.
        if euler[0] > 90 or euler[0] < -90:
            self.position[2] -= self.size[2]
            self.size[2] *= -1


class Pyramid(Box):
    """A tetrahedron, pointing straight up or down, with rotation in the Z axis"""
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

    def set_size(self, x=8.2, y=8.2, z=10.25):
        # Pyramids have different size defaults than the box
        self.size = [x,y,z]

class Teleporter(Box):
    """A teleporter, with translation, 2 degrees of field size, a border size and rotation"""
    type = 'teleporter'

    verts = [(0,  1, 1),
             (0, -1, 1),
             (0, -1, 0),
             (0,  1, 0),
            ]

    faces = [(0, 1, 2, 3),
            ]
    materials = [teleporterFieldMaterial, teleporterBorderMaterial]
    materialIndex = []

    def __init__(self):
        # Load defaults
        self.set_position()
        self.set_rotation()
        self.set_size()
        self.set_border()

    def set_position(self, x=0, y=0, z=0):
        self.position = [x,y,z]

    def set_rotation(self, degrees=0):
        self.rotation = degrees

    def set_size(self, x=1, y=4.48, z=20.16):
        self.size = [1.0,y,z]

    def set_border(self, border=1.0):
        self.border = border

    def serialize(self, writer):
        BZObject.serialize(self, writer)
        writer(("position",) + tuple(self.position))
        writer(("size", 0.0, self.size[1], self.size[2]))
        writer(("rotation", self.rotation))
        writer(("border", self.border))

    def setBlenderProperties(self, object):
        BZObject.setBlenderProperties(self, object)
        object.addProperty('border', float(self.border), 'FLOAT')

    def loadBlenderProperties(self, object):
        BZObject.loadBlenderProperties(self, object)
        try:
            self.border = object.getProperty('border').getData()
        except AttributeError:
            # No property, use the default border
            self.set_border()

class Zone(Box):
    """A rectangular area with special properties (flag entry, tank entry, etc)"""
    type = 'zone'

    def __init__(self):
        Box.__init__(self)
        self.subtype = 'team' # one of 'flag', 'team' or 'safety'

    def setBlenderProperties(self, object):
        BZObject.setBlenderProperties(self, object)
#        if self.subtype == 'flag':
#        else if self.subtype == 'team':
#        else if self.subtype == 'safety':
#            object.addProperty()

    def loadBlenderProperties(self, object):
        BZObject.loadBlenderProperties(self, object)

### The End ###

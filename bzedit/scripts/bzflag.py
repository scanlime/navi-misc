#
# bzflag.py - This is a module shared by all blender scripts,
#             providing a library of bzflag world objects that
#             can be serialized/deserialized and converted
#             to/from blender objects.
#
# Blender-based BZFlag World Editor
# Copyright (C) 2004 David Trowbridge and Micah Dowty
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
from Blender import NMesh
import math


def meshify(vertex, face):
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
        mesh.faces.append(face)
    return NMesh.PutRaw(mesh)


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
            self.stream.write("# %s" % content)
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

    def serialize(self, writer):
        """Store this BZFlag object to a world file by calling the
           given writer for each tuple or comment to be included
           in the world block. The default implementation writes
           comments, but subclasses must add to this to write useful
           data.
           """
        if self.comment:
            for line in comment.split("\n"):
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
                commentLines = str(line)
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

    def toBlender(self):
        """Create a new Blender object representing this one,
           and return it. The default implementation creates the
           object and assigns its 'bztype' property, but further
           object creation must be performed by subclasses.
           """
        pass

    def fromBlender(self, object):
        """Load this object's settings from the given Blender object.
           Must be implemented by subclasses.
           """
        pass


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

    def fromBlender(self, object):
        """Find the proper BZObject subclass for the given Blender object,
           and load its parameters from the blender object. Returns a new
           BZObject instance.
           """
        bztype = object.getProperty('bztype').getData()
        bzo = self.dict[bztype]()
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

    def __init__(self):
        self.set_size()

    def set_size(self, size=400):
        self.size = size

    def serialize(self, writer):
        writer(("size", self.size))


class Box(BZObject):
    """A rectangular prism, with translation and with rotation in the Z axis"""
    type = 'box'

    def __init__(self):
        # Load defaults
        self.set_position()
        self.set_rotation()
        self.set_size()

    def set_position(self, x=0, y=0, z=0):
        self.position = [x,y,z]

    def set_rotation(self, degrees=0):
        self.rotation = degrees

    def set_size(self, x=1, y=1, z=0.942):
        self.size = [x,y,z]

    def serialize(self, writer):
        writer(("position",) + tuple(self.position))
        writer(("size",) + tuple(self.size))
        writer(("rotation", self.rotation))

    def toBlender(self):
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
                 (0, 4, 5, 1), # Y+
                 (6, 2, 3, 7), # Y-
                 (0, 2, 6, 4), # Z+
                 (1, 5, 7, 3), # Z-
                ]

        box = meshify(verts, faces)

        box.setSize(
            self.size[0] / 10.0,
            self.size[1] / 10.0,
            self.size[2] / 10.0
            )
        box.setLocation(
            self.position[0] / 10.0,
            self.position[1] / 10.0,
            self.position[2] / 10.0
            )
        box.setEuler((0, 0, self.rotation / 180.0 * math.pi))


class Pyramid(Box):
    """A tetrahedron, pointing straight up or down, with rotation in the Z axis"""
    type = 'pyramid'

    def __init__(self):
        # Load defaults
        self.set_position()
        self.set_rotation()
        self.set_size()

    def set_position(self, x=0, y=0, z=0):
        self.position = [x,y,z]

    def set_rotation(self, degrees=0):
        self.rotation = degrees

    def set_size(self, x=0.82, y=0.82, z=1.025):
        self.size = [x,y,z]

    def toBlender(self):
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

        pyramid = meshify(verts, faces)

        pyramid.setSize(
            self.size[0] / 10.0,
            self.size[1] / 10.0,
            self.size[2] / 10.0
            )
        pyramid.setLocation(
            self.position[0] / 10.0,
            self.position[1] / 10.0,
            self.position[2] / 10.0
            )
        pyramid.setEuler((0, 0, self.rotation / 180.0 * math.pi))

### The End ###

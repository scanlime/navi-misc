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


class CommentLine(str):
    """Placeholder for a comment line, loaded or saved in a BZFlag world"""
    pass


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
                    yield tokens

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
            obj = typeRegistry.dict[type]()
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
        self.writeBlock(object.serialize)


class BZObject:
    """Abstract base class for BZFlag objects. Each BZFlag object
       has a named type, can serialize/deserialize itself, and
       can be converted to and from blender objects.
       """
    type = None
    comment = None

    def serialize(self, writer):
        """Store this BZFlag object to a world file by calling the
           given writer for each tuple or comment to be included
           in the world block. This must be implemented by
           subclasses.
           """
        pass

    def deserialize(self, reader):
        """Read information for this BZFlag object from the given
           iterator. The iterator is expected to return either comment
           objects or tuples. This must be implemented by subclasses.
           """
        pass

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
        bztype = self.object.getProperty('bztype').getData()
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

### The End ###

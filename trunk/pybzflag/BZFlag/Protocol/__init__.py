""" BZFlag.Protocol

This package contains utilities for representing BZFlag's binary
structures, particularly the wire protocol, cleanly in Python.

This module defines a base class and associated utilities that will be
used to generate the actual wrappers. The wrappers are separated into
packets from client to server, packets from server to client, and
common structures used within the packets.

This module should not contain any details specific to the BZFlag
protocol. All such details should go in Protocol.Common

Conventions for the classes herein:

  - A class can be created from a structure's packed representation by
    using that representation as the sole argument when constructing
    that class

  - A packed representation can be created from the class by calling
    str() on that class

  - Unless there's a good reason otherwise (which must be documented),
    the structure's members, as named in the appropriate BZFlag
    headers, must be accessable as attributes of that class.

This module is safe for use with 'from BZFlag.Protocol import *'
"""
# 
# Python BZFlag Protocol Package
# Copyright (C) 2003 Micah Dowty <micahjd@users.sourceforge.net>
# 
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#  
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#  
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
#

import struct

__all__ = [
    'EntryType', 'ScalarType', 'VectorType', 'SubStruct', 'StructEntry',
    'StructPadding', 'Struct', 'Int8', 'UInt8', 'Int16', 'UInt16', 'Int32',
    'UInt32', 'Float', 'Double', 'StringField', 'Enum', 'Bitfield',
    'ConstStructEntry',
    ]


class EntryType:
    """Abstract base class for a data type used in structure entries"""
    def unmarshall(self, packed):
        pass

    def marshall(self, object):
        pass

    def getSize(self, packed=None):
        """Get the size of a packed instance of this type. The supplied
           string may contain extra data after the packed instance.
           """
        pass


class ScalarType(EntryType):
    """A scalar EntryType, implemented using the standard 'struct' module.
       Format strings here are assumed to only specify one value.
       """
    def __init__(self, fmt):
        self.fmt = fmt
        self.size = struct.calcsize(fmt)

    def unmarshall(self, packed):
        return struct.unpack(self.fmt, packed)[0]

    def marshall(self, object):
        return struct.pack(self.fmt, object)

    def getSize(self, packed=None):
        return self.size
    

class VectorType(EntryType):
    """A vector EntryType, implemented using the standard 'struct' module.
       Format strings here are assumed to only specify one value.
       """
    def __init__(self, fmt):
        self.fmt = fmt
        self.size = struct.calcsize(fmt)

    def unmarshall(self, packed):
        return struct.unpack(self.fmt, packed)

    def marshall(self, object):
        args = [self.fmt] + list(object)
        return struct.pack(*args)

    def getSize(self, packed=None):
        return self.size


class SubStruct(EntryType):
    """An EntryType for embedding Struct classes within other Struct classes.
       Note that this is constructed using a Struct subclass, not a Struct
       instance. The unmarshalled type will be an instance of the provided class.
       """
    def __init__(self, struct):
        self.struct = struct
        self.size = struct().getSize()

    def unmarshall(self, packed):
        return self.struct(packed)

    def marshall(self, object):
        return str(object)

    def getSize(self, packed=None):
        return self.size

    
class StructEntry:
    """(Un)marshalls one structure entry, using an instance of EntryType."""
    def __init__(self, entryType, entryName):
        self.entryType = entryType
        self.entryName = entryName 

    def unmarshall(self, struct, packed):
        """Given a string beginning with the packed representation
           of this entry, store the entry in struct and return
           the substring of packed ending after this entry.
           """
        bytes = self.entryType.getSize(packed)
        setattr(struct, self.entryName, self.entryType.unmarshall(packed[:bytes]))
        return packed[bytes:]

    def marshall(self, struct, packed):
        """Return a packed representation of a structure containing
           this as the last entry, given the packed representation of
           the previous entries.
           """
        return packed + self.entryType.marshall(getattr(struct, self.entryName))

    def getSize(self, packed=None):
        return self.entryType.getSize(packed)


class ConstStructEntry:
    """A class that can be used in place of a StructEntry when the value
       is always constant, for example magic numbers or synchronization bytes.
       By default, unmarshalling is skipped. If an exception instance is supplied,
       it will be raised if the unmarshalled value doesn't match the given value.
       """
    def __init__(self, entryType, entryValue, badMatchException=None):
        self.entryType = entryType
        self.entryValue = entryValue
        self.badMatchException = badMatchException

    def unmarshall(self, struct, packed):
        bytes = self.entryType.getSize(packed)
        if self.badMatchException:
            if self.entryType.unmarshall(packed[:bytes]) != self.entryValue:
                raise self.badMatchException
        return packed[bytes:]

    def marshall(self, struct, packed):
        return packed + self.entryType.marshall(self.entryValue)

    def getSize(self, packed=None):
        return self.entryType.getSize(packed)


class StructPadding:
    """A class that can be used in place of StructEntry to waste a particular
       number of bytes without actually marshalling any data.
       """
    def __init__(self, size, padByte=0):
        self.size = size
        self.padByte = 0

    def unmarshall(self, struct, packed):
        return packed[self.size:]

    def marshall(self, struct, packed):
        return packed + (chr(self.padByte) * self.size)

    def getSize(self, packed=None):
        return self.size


class Enum(EntryType):
    """A class that can be used to wrap another EntryType instance,
       translating between enumerated types and strings or other
       objects. Any objects not found in the provided dictionary
       will be left alone.
       """
    def __init__(self, baseType, enums, firstNumber=0):
        """The enums dict here should translate from marshalled to unmarshalled.
           A corresponding dictionary for the reverse will be generated.
           enums can also be a list, to imply that the enumeration is sequential,
           starting at firstNumber.
           """
        self.baseType = baseType
        if type(enums) != type({}):
            self.list = enums
            self.dict = {}
            self.firstNumber = firstNumber
            for i in xrange(len(self.list)):
                self.dict[i + firstNumber] = self.list[i]
        else:
            self.dict = enums
            self.list = self.dict.values()
        self.reverseDict = {}
        for key in self.dict:
            self.reverseDict[self.dict[key]] = key
 
    def unmarshall(self, packed):
        object = self.baseType.unmarshall(packed)
        try:
            return self.dict[object]
        except KeyError:
            return object

    def marshall(self, object):
        try:
            object = self.reverseDict[object]
        except KeyError:
            pass
        return self.baseType.marshall(object)

    def getSize(self, packed=None):
        return self.baseType.getSize(packed)


class Bitfield(EntryType):
    """A class that can be used to wrap another EntryType instance,
       translating between a bitfield and a list of strings or other
       objects.
       """
    def __init__(self, baseType, bitMap):
        """The bitMap should have integer bitmasks as values, with
           strings or other objects as keys. Note that the bitmasks
           can overlap- all matches for that bitmask will be included
           in the resulting list, so the 'in' operator can be used
           to easily test whether a particular bitmask is set.
           In this case, a match means that any of the bitmask's bits
           are set, not that all of them are set.
           """
        self.baseType = baseType
        self.bitMap = bitMap
 
    def unmarshall(self, packed):
        bits = self.baseType.unmarshall(packed)
        matches = []
        for key in self.bitMap:
            if bits & self.bitMap[key]:
                matches.append(key)
        return matches

    def marshall(self, object):
        try:
            object = self.bitMap[object]
        except KeyError:
            pass
        return self.baseType.marshall(object)

    def getSize(self, packed=None):
        return self.baseType.getSize(packed)


class StringField(EntryType):
    """A type that decodes and encodes zero-terminated strings,
       padded to a fixed field width. ScalarType("10s") for example
       would encode strings fine, but the zero termination would
       not be stripped from decoded strings.
       """
    def __init__(self, length):
        self.length = length
 
    def unmarshall(self, packed):
        # Remove everything after and including the zero termination
        zeroPos = packed.find(chr(0))
        if zeroPos >= 0:
            return packed[:zeroPos]
        else:
            # The string takes up the entire field
            return packed

    def marshall(self, object):
        # Truncate to the field width and pad out with zeroes
        object = object[:self.length-1]
        object += chr(0) * (self.length - len(object))
        return object

    def getSize(self, packed=None):
        return self.length


class Struct:
    """Base class for protocol structures that will be somewhat
       automatically marshalled and unmarshalled. Base classes should
       define 'entries' as a list of StructEntry instances, in the
       order they occur in the packed structure. Those StructEntry
       instances will be responsible for marshalling and unmarshalling
       individual variables.
       """
    entries = []
    
    def __init__(self, packed=None, **kw):
        if packed:
            self.unmarshall(packed)
        self.__dict__.update(kw)
      
    def __str__(self):
        return self.marshall()

    def unmarshall(self, packed):
        for entry in self.entries:
            packed = entry.unmarshall(self, packed)
        self.postUnmarshall()
        return packed
        
    def marshall(self):
        self.preMarshall()
        packed = ''
        for entry in self.entries:
            packed = entry.marshall(self, packed)
        return packed

    def getSize(self, packed=None):
        total = 0
        for entry in self.entries:
            total += entry.getSize()
        return total

    def preMarshall(self):
        """This is a hook subclasses can use to add constant fields"""
        pass

    def postUnmarshall(self):
        """This is a hook subclasses can use to verify constant fields"""
        pass

    def read(self, f):
        """Read this struct in from a file-like object"""
        self.unmarshall(f.read(self.getSize()))


#  EntryType instances for scalar types, all in network byte order
Int8    = ScalarType("!b")
UInt8   = ScalarType("!B")
Int16   = ScalarType("!h")
UInt16  = ScalarType("!H")
Int32   = ScalarType("!i")
UInt32  = ScalarType("!I")
Float   = ScalarType("!f")
Double  = ScalarType("!d")

### The End ###
        
    

""" BZFlag.Protocol

This package contains utilities for representing BZFlag's binary
structures, particularly the wire protocol, cleanly in Python.

This module defines a base class and associated utilities that will be
used to generate the actual wrappers. The wrappers are separated into
packets from client to server, packets from server to client, and
common structures used within the packets.

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
    'UInt32', 'Float', 'Double'
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


class Struct:
    """Base class for protocol structures that will be somewhat
       automatically marshalled and unmarshalled. Base classes should
       define 'entries' as a list of StructEntry instances, in the
       order they occur in the packed structure. Those StructEntry
       instances will be responsible for marshalling and unmarshalling
       individual variables.
       """
    def __init__(self, packed=None):
        if packed:
            for entry in self.entries:
                packed = entry.unmarshall(self, packed)
      
    def __str__(self):
        packed = ''
        for entry in self.entries:
            packed = entry.marshall(self, packed)
        return packed

    def getSize(self, packed=None):
        total = 0
        for entry in self.entries:
            total += entry.getSize()
        return total


#  EntryType instances for scalar types, all in network byte order
Int8    = ScalarType("!b")
UInt8   = ScalarType("!B")
Int16   = ScalarType("!h")
UInt16  = ScalarType("!H")
Int32   = ScalarType("!i")
UInt32  = ScalarType("!I")
Float   = ScalarType("f")
Double  = ScalarType("d")

### The End ###
        
    

""" RioKarma.Property

Property lists are dictionaries of metadata or settings. They are
represented in Python as simple dictionaries, and on the wire in a
simple human-readable encoding. This module implements file-like classes
for reading and writing property lists, as well as encoding/decoding
for the more complex property values.

"""
#
# Copyright (C) 2005 Micah Dowty <micah@navi.cx>
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

import struct


def fromUTF8(s):
    return unicode(s, 'utf-8')

def toUTF8(s):
    return unicode(s).encode('utf-8')

def fromEscaped(s):
    """Properties that contain binary data are escaped using a backslash system.
       This converts from the escaped data back to a plain string.
       """
    # Quickly handle all the easy ones, in order of frequency
    s = s.replace("\\x00", "\x00").replace("\\n", "\n")

    # Now we have a generic way to handle \\ and \x## escapes
    segments = s.split('\\')
    results = segments[:1]
    for i in xrange(1, len(segments)):
        segment = segments[i]
        if segment[0] == 'x':
            # Parse a 2-digit hex character
            results.append(chr(int(segment[1:3], 16)) + segment[3:])
        else:
            results.append(segment)
    return ''.join(results)

def toEscaped(s):
    """Properties that contain binary data are escaped using a backslash system.
       This converts from a plain string, possibly containing binary data,
       to the escaped format.
       """
    results = []
    for char in s:
        n = ord(char)
        if char == '\n':
            results.append('\\n')
        elif (n>=0 and n<32) or (n==127):
            results.append('\\x%02X' % n)
        elif char == '\\':
            results.append('\\\\')
        else:
            results.append(char)
    return ''.join(results)

def fromPlaylist(s):
    """Decode an escaped binary playlist into a list
       of partial file details dictionaries. These dictionaries
       will only include 'fid' and 'fid_generation' keys currently.
       """
    s = fromEscaped(s)
    ints = struct.unpack("<%dI" % (len(s)//4), s)

    # This hint comes from karmalib, but I've never seen
    # a playlist have a header of anything other than 0x000002FF.
    header = ints[0]
    hasGenerations = (header & 0xF0000000L) == 0

    results = []
    i = 1
    finishAt = len(ints)
    while i < finishAt:
        d = {}
        d['fid'] = ints[i]
        i += 1
        if hasGenerations and i < finishAt:
            # Zero seems to be a "don't care" for the fid_generation.
            # Leave it out so we don't try to use it as a search key.
            if ints[i]:
                d['fid_generation'] = ints[i]
            i += 1
        results.append(d)
    return results

def toPlaylist(s):
    """Encode a list of partial file details dictionaries into
       an escaped binary playlist.
       """
    ints = [0x000002FF]

    for details in s:
        ints.append(details.get('fid', 0))
        ints.append(details.get('fid_generation', 0))

    return toEscaped(struct.pack("<%dI" % len(ints), *ints))

# Decoding functions for properties that aren't plain strings
propertyDecoders = {
    'artist':         fromUTF8,
    'title':          fromUTF8,
    'source':         fromUTF8,
    'length':         int,
    'ctime':          int,
    'fid_generation': int,
    'fid':            int,
    'file_id':        int,
    'duration':       int,
    'samplerate':     int,
    'tracknr':        int,
    'profile':        fromEscaped,
    'playlist':       fromPlaylist,
    }

# Encoding functions for properties that aren't plain strings
propertyEncoders = {
    'artist':         toUTF8,
    'title':          toUTF8,
    'source':         toUTF8,
    'profile':        toEscaped,
    'playlist':       toPlaylist,
    }


class LineBufferedWriter:
    """Base class for file-like objects whose write() method must be line buffered"""
    _writeBuffer = ''

    def write(self, data):
        """Write an arbitrary amount of data, buffering any incomplete lines"""
        buffer = self._writeBuffer + data
        fragments = buffer.split("\n")
        self._writeBuffer = fragments[-1]
        for line in fragments[:-1]:
            self.writeLine(line)


class LineBufferedReader:
    """Base class for file-like objects whose read() method must be line buffered"""
    _readBuffer = ''

    def read(self, size=None):
        """Read up to 'size' bytes. If 'size' is None, this will read all
           available data.
           """
        buffer = self._readBuffer

        # Read all we can, within our size limits
        while (size is None) or (len(self._readBuffer) < size):
            line = self.readLine()
            if not line:
                break
            buffer += line

        # If we had no size limit, return the whole thing
        if size is None:
            self._readBuffer = ''
            return buffer

        # Otherwise, truncate it
        self._readBuffer = buffer[size:]
        return buffer[:size]


class PropertyFileReader(LineBufferedReader):
    """This is a readable object that serializes a dictionary-like object
       using the 'properties file' key-value format.
       """
    _readIter = None

    def __init__(self, db):
        self.db = db

    def readLine(self):
        """Read exactly one key=value line, with \n termination. Returns the
           empty string if we have no more data.
           """
        if self._readIter is None:
            self._readIter = self.db.iteritems()
        try:
            key, value = self._readIter.next()
        except StopIteration:
            return ""

        coder = propertyEncoders.get(key, str)
        return "%s=%s\n" % (key, coder(value))

    def rewind(self):
        """Seek back to the beginning of the buffer. This also revalidates
           a bad iterator if the dictionary was modified while reading.
           """
        self._readIter = None


class PropertyFileWriter(LineBufferedWriter):
    """This is a writeable object that populates a dictionary-like object
       using the 'properties file' key-value format.
       """

    def __init__(self, db):
        self.db = db

    def writeLine(self, line):
        """Write a complete key=value line"""
        key, value = line.strip().split("=", 1)

        coder = propertyDecoders.get(key, str)
        self.db[key] = coder(value)


class FileDatabaseWriter(LineBufferedWriter):
    """This class implements a writeable object that parses a file database,
       sending each completed dictionary to a user-provided callback.
       """
    def __init__(self, callback):
        self.callback = callback
        self._current = PropertyFileWriter({})

    def writeLine(self, line):
        line = line.strip()
        if line:
            # Add a key-value pair to our current dict
            self._current.writeLine(line)
        else:
            # Commit this dict
            self.callback(self._current.db)
            self._current = PropertyFileWriter({})

### The End ###

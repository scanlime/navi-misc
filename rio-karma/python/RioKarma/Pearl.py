""" RioKarma.Pearl

This module implements the core of the Pearl protocol- Twisted Protocol
objects, utilities to convert to and from Pearl's data formats, error
codes, and such. The actual requests used in this protocol are defined
in Request.py

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
from cStringIO import StringIO
from twisted.internet import protocol, defer
from twisted.python import failure


class ProtocolError(Exception):
    pass

class AuthenticationError(Exception):
    pass


# Most of these errors were guessed based on expected behavior, might be incorrect
statusExceptionMap = {
    0x8004005E: (ProtocolError,       "A lock required for this operation is not held or can't be acquired"),
    0xC0040002: (ProtocolError,       "Nonexistant file ID"),
    0xC0041002: (ProtocolError,       "File content can't be read"),
    0x8004005B: (AuthenticationError, "Password is incorrect"),
    }


class Protocol(protocol.Protocol):
    """The 'Pearl' protocol consists of client-initiated request/response
       pairs. Each request/response has a common header, but the data may be
       variable length and different requests may require different algorithms
       for buffering the received responses. We maintain a queue of outstanding
       requests, and the oldest one determines which read algorithm we use.

       The queue itself is unbounded, but bulk writes to it, for the file
       transfers themselves, are throttled such that the queue remains full
       enough to avoid wasting transmit or receive time, but that we don't
       waste all our CPU filling it. This ideal fill level is set in the
       constructor- this number should stay pretty low, or we spend far too
       much time waiting for Twisted to buffer outgoing packets. Appending
       to the buffer takes longer the more items are in it currently.
       """
    def __init__(self, preferredBacklog=4):
        self.buffer = ''

        # When the queue is less than preferredBacklog deep, we have
        # a list of clients waiting to be notified so they can add more.
        self.preferredBacklog = preferredBacklog
        self.queueListeners = []

        # A list is really a suboptimal data type for this. If we feel like
        # requiring python 2.4, a collections.dequeue would be great.
        self.requestQueue = []

    def sendRequest(self, request):
        """Send a request, and add it to the queue of requests
           waiting for their response. Returns a Deferred that
           eventually yields the request's result.
           """
        self.requestQueue.append(request)
        request.sendTo(self.transport)
        return request.result

    def throttle(self, callable):
        """Add the provided callable to a list of clients that will be
           notified when the queue has fewer than the preferred number
           of requests in it. If the queue is already empty enough, the
           callable will be invoked immediately.
           """
        self.queueListeners.append(callable)
        self._checkQueueListeners()

    def _checkQueueListeners(self):
        """If the queue is empty enough, call everyone on the queueListeners list"""
        if len(self.requestQueue) < self.preferredBacklog:
            # Create the new listener list first, so it can be
            # safely written to by our callbacks.
            listeners = self.queueListeners
            self.queueListeners = []
            for callable in listeners:
                callable()

    def dataReceived(self, data):
        # Our new buffer goes into a StringIO, so individual requests
        # can read pieces of it efficiently.
        bufferFile = StringIO()
        bufferFile.write(self.buffer)
        bufferFile.write(data)
        bufferFileLength = bufferFile.tell()
        bufferFile.reset()

        while self.requestQueue:
            currentRequest = self.requestQueue[0]

            try:
                currentRequest.readResponse(bufferFile)
            except:
                # Any error in this handler belongs to the request, not us
                currentRequest.result.errback(failure.Failure())

            if currentRequest.result.called:
                # This request is done, on to the next
                del self.requestQueue[0]
                self._checkQueueListeners()
            else:
                # This request needs more data. If it did complete, we go around
                # and let the next request look at our buffer.
                break

        # All data remaining in our bufferFile after this round gets
        # stowed in self.buffer. The data we already read is discarded.
        self.buffer = bufferFile.read()


class BaseRequest:
    """This is the abstract base class for requests in the Pearl protocol.
       Every request must know how to send itself, and know how to process
       received data. Every request signals its completion via a Deferred.
       """
    MAGIC = "Ri\xc5\x8d"
    id = None

    def __init__(self):
        self.result = defer.Deferred()

    def sendTo(self, fileObj):
        """Send this request to the provided file-like object"""
        raise NotImplementedError

    def readResponse(self, fileObj):
        """Read response data from the provided file-like object.
           This should read only the data that actually belongs to this
           request. If the request can be completed with the provided
           data, self.result.called must be True after this returns.
           """
        raise NotImplementedError

    def decodeStatus(self, status):
        """Decode the 'status' word common to many requests, generating
           an exception if appropriate.
           """
        # Get out fast if we were successful
        if status == 0:
            return
        try:
            exc = statusExceptionMap[status]
        except KeyError:
            raise ProtocolError("Unexpected status 0x%08X for request %r" %
                                (status, self))
        raise exc[0](*exc[1:])


class StatefulRequest(BaseRequest):
    """This class implements requests that have several different states
       in which received data means different things. State transitions can
       occur while reading, such that the new reader gets a chance to finish
       the current buffer, and it will be called later for future buffer
       updates.

       This implements default states to read the packet's magic and ID,
       validate them, and take appropriate action for NAK or Busy packets.
       """
    def __init__(self):
        BaseRequest.__init__(self)
        self.responseBuffer = ''
        self.stateTransition(self.state_beginResponse)

    def stateTransition(self, newReader, fileObj=None):
        self.readResponse = newReader
        if fileObj is not None:
            newReader(fileObj)

    def fillResponseBuffer(self, fileObj, size):
        """Read from fileObj, trying to fill self.responseBuffer
           up to exactly 'size' bytes. Returns True if the buffer is
           the right size, raises a ProtocolError if somehow it gets too big.
           """
        remaining = size - len(self.responseBuffer)
        if remaining > 0:
            self.responseBuffer += fileObj.read(remaining)

        responseLen = len(self.responseBuffer)
        if responseLen > size:
            raise ProtocolError("Request %r read too much data (have %d, need %d)" %
                                (self, responseLen, size))
        return responseLen == size

    def state_beginResponse(self, fileObj):
        """This is the initial state, as well as the state we return to after a NAK
           or Busy packet. Read the magic number and ID.
           """
        if self.fillResponseBuffer(fileObj, 8):
            # Check the magic
            if not self.responseBuffer.startswith(self.MAGIC):
                raise ProtocolError("Connection is out of sync, didn't receive header for %r" % self)

            # Decode the ID
            responseId = struct.unpack("<I", self.responseBuffer[4:])[0]
            self.responseBuffer = ''

            if responseId == self.id:
                # This is the reply we were looking for
                self.stateTransition(self.state_normalReply, fileObj)

            elif responseId == 1:
                # This is a NAK response, the device rejected our command
                raise ProtocolError("NAK response to %r" % self)

            elif responseId == 2:
                # This is a "Busy" response, we have a state for that
                self.stateTransition(self.state_busyReply, fileObj)

            else:
                # Something wonky...
                raise ProtocolError("Connection is out of sync, request #%d received a reply for #%d" %
                                    (self.id, responseId))

    def state_busyReply(self, fileObj):
        """We received a 'Busy' packet. Wait for its body, make note of it,
           then go back to waiting for our real packet.
           """
        if self.fillResponseBuffer(fileObj, 8):
            step, numSteps = struct.unpack("<II", self.responseBuffer)
            self.responseBuffer = ''

            # FIXME: Once we have progress reporting, this will go somewhere useful
            #print "Busy: %d/%d" % (step, numSteps)

            self.stateTransition(self.state_beginResponse, fileObj)

    def state_normalReply(self, fileObj):
        """This state is implemented by subclasses to receive normal replies"""
        raise NotImplementedError


class StructRequest(StatefulRequest):
    """This is an abstract base class for requests where the paclet bodies
       can be described, at least in part, using the struct module.
       """
    requestFormat = ''
    responseFormat = ''
    parameters = ()

    def __init__(self):
        StatefulRequest.__init__(self)
        self._responseLength = struct.calcsize(self.responseFormat)

    def sendTo(self, fileObj):
        # Send the header, then our formatted parameters
        fileObj.write(self.MAGIC + struct.pack("<I", self.id) +
                      struct.pack(self.requestFormat, *self.parameters))

    def state_normalReply(self, fileObj):
        if self.fillResponseBuffer(fileObj, self._responseLength):
            response = struct.unpack(self.responseFormat, self.responseBuffer)
            self.responseBuffer = ''
            self.receivedResponse(fileObj, *response)

    def receivedResponse(self, source, *args):
        """This is called when a complete decoded response is received, with arguments
           as specified by the responseFormat. It is expected to call self.result with some
           sort of return value, None if necessary.

           The default implementation just calls the callback with all args we got- packed
           into a tuple if there are multiple arguments. This will work if the caller
           doesn't expect any special processing to happen on the response.
           """
        if len(args) > 1:
            self.result.callback(args)
        else:
            self.result.callback(args[0])


class AlignedStringReader:
    """This object copies one 4-byte-aligned null terminated string
       from source to destination. The last read it performs is the
       4-byte block containing the null terminator, and the last write
       includes the character preceeding the null terminator but not
       the terminator itself.

       This returns True as long as it needs more data.
       """
    totalLength = 0
    foundNull = False

    def next(self, source, destination):
        while self.totalLength & 0x03 or not self.foundNull:
            # We can only read up until the end of the next 4-byte block,
            # to avoid overshooting the end of the string.
            block = source.read(4 - (self.totalLength & 0x03))
            if not block:
                return True
            self.totalLength += len(block)

            # Look for the null, and write everything before it but nothing
            # after it or including it.
            if not self.foundNull:
                i = block.find('\x00')
                if i < 0:
                    destination.write(block)
                else:
                    destination.write(block[:i])
                    self.foundNull = True
        return False


class AlignedStringWriter:
    """This object copies all data from the source into one
       4-byte-aligned null terminated string in the destination.
       """
    bufferSize = 4096

    def next(self, source, destination):
        totalLength = 0

        # Copy as many complete blocks as we can, tracking the total size
        while 1:
            block = source.read(self.bufferSize)
            totalLength += len(block)
            if len(block) == self.bufferSize:
                destination.write(block)
            else:
                break

        # Null-terminate the last block
        block += "\x00" * (4 - (totalLength & 0x03))
        destination.write(block)


class AlignedBlockReader:
    """This object copies one 4-byte-aligned block from source to
       destination, in which the length is known beforehand.
       This returns True as long as it needs more data.
       """
    def __init__(self, size, bufferSize=16384):
        self.size = size
        self.bufferSize = bufferSize
        self.paddedSize = size + ((4 - (size & 0x03)) & 0x03)
        self.transferred = 0

    def next(self, source, destination):
        while self.transferred < self.paddedSize:
            block = source.read(min(self.bufferSize, self.paddedSize - self.transferred))
            if not block:
                return True
            self.transferred += len(block)

            if self.transferred >= self.bufferSize:
                # We've transferred all the real data already, the rest is padding
                continue
            else:
                # Write up until we hit the end of the real data
                destination.write(block[:self.bufferSize - self.transferred])
        return False


class AlignedBlockWriter:
    """This object copies a fixed-size block of data from the
       source into one 4-byte-aligned block in the destination.
       """
    def __init__(self, size):
        self.size = size

    def next(self, source, destination):
        # This is probably the most speed-crucial part of the whole program-
        # so start of really simple. If we actually get oddly sized reads
        # here, we'll have to add a special case later.
        # buffering.
        block = source.read(self.size)
        if len(block) != self.size:
            raise IOError("Unexpected end of file")

        destination.write(block + "\x00" * ((4 - (len(block) & 0x03)) & 0x03))


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

        # If we get a unicode value, encode it in UTF-8
        if type(value) is unicode:
            value = value.encode('utf-8')
        return "%s=%s\n" % (key, value)

    def rewind(self):
        """Seek back to the beginning of the buffer. This also revalidates
           a bad iterator if the dictionary was modified while reading.
           """
        self._readIter = None


class PropertyFileWriter(LineBufferedWriter):
    """This is a writeable object that populates a dictionary-like object
       using the 'properties file' key-value format.
       """

    # By default we decode as strings, but this table ensures
    # that our human-readable metadata is unicode and we get integers
    # where we should.
    keyTypes = {
        'artist':         unicode,
        'title':          unicode,
        'source':         unicode,
        'length':         int,
        'ctime':          int,
        'fid_generation': int,
        'fid':            int,
        'file_id':        int,
        'duration':       int,
        'samplerate':     int,
        'tracknr':        int,
        }

    def __init__(self, db):
        self.db = db

    def writeLine(self, line):
        """Write a complete key=value line"""
        key, value = line.strip().split("=", 1)

        keyType = self.keyTypes.get(key)
        if keyType is unicode:
            value = unicode(value, 'utf-8')
        elif keyType:
            value = keyType(value)
        self.db[key] = value


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

""" RioKarma

This module implements the 'Pearl' protocol used to communicate with
the Rio Karma portable audio player over TCP/IP, as well as higher-level
abstractions necessary to transfer files to and from the device.

This module wouldn't have been possible without the reverse-engineering
work done by the Karmalib and libkarma projects:

    http://karmalib.sourceforge.net/docs/protocol.php
    http://www.freakysoft.de/html/libkarma/

"""
#
# Copyright (C) 2005 Micah Dowty <micah@navi.cx>
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

import struct, md5, os, random
import shelve, cPickle
from cStringIO import StringIO
from twisted.internet import protocol, defer, reactor
from twisted.python import failure, log


############################################################################
############################################################  Protocols  ###
############################################################################

class ProtocolError(Exception):
    pass

class AuthenticationError(Exception):
    pass


# Most of these errors were guessed based on expected behavior, might be incorrect
statusExceptionMap = {
    0x8004005E: (ProtocolError,       "A lock required for this operation is not currently held"),
    0xC0040002: (ProtocolError,       "Nonexistant file ID"),
    0x8004005B: (AuthenticationError, "Password is incorrect"),
    }


class RequestResponseProtocol(protocol.Protocol):
    """The 'Pearl' protocol consists of client-initiated request/response
       pairs. Each request/response has a common header, but the data may be
       variable length and different requests may require different algorithms
       for buffering the received responses. We maintain a queue of outstanding
       requests, and the oldest one determines which read algorithm we use.
       """
    def __init__(self):
        self.buffer = ''

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
            else:
                # This request needs more data. If it did complete, we go around
                # and let the next request look at our buffer.
                break

        # All data remaining in our bufferFile after this round gets
        # stowed in self.buffer. The data we already read is discarded.
        self.buffer = bufferFile.read()


class AuthenticatedProtocol(RequestResponseProtocol):
    """This builds on the RequestResponseProtocol to include authentication,
       initiated when the protocol connects successfully. This reports
       authentication status via the factory's 'authResult' deferred, returning
       a reference to this Protocol on success.
       """
    def connectionMade(self):
        self.sendRequest(Request_GetAuthenticationSalt()).addCallback(
            self._sendAuthDigest).addErrback(self.factory.authResult.errback)

    def _sendAuthDigest(self, salt):
        self.sendRequest(Request_Authenticate(salt, self.factory.password)).addCallback(
            self._reportAuthResult).addErrback(self.factory.authResult.errback)

    def _reportAuthResult(self, rights):
        self.authRights = rights
        self.factory.authResult.callback(self)


def connect(host, port=8302, password='', cls=AuthenticatedProtocol):
    """Connect to a Rio Karma device. Returns a Deferred that results in
       an AuthenticatedProtocol instance on success.
       """
    factory = protocol.ClientFactory()
    factory.authResult = defer.Deferred()
    factory.protocol = cls
    factory.password = password
    factory.clientConnectionFailed = lambda connector, reason: factory.authResult.errback(reason)
    reactor.connectTCP(host, port, factory)
    return factory.authResult


############################################################################
#################################################  Request Base Classes  ###
############################################################################

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
                raise ProtocolError("NAK response")

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
            print "Busy: %d/%d" % (step, numSteps)

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


############################################################################
##############################################  Request Implementations  ###
############################################################################

class Request_GetProtocolVersion(StructRequest):
    """Return the major and minor version of this device's protocol"""
    id = 0
    parameters = (0,0)
    requestFormat = '<II'
    responseFormat = '<II'


class Request_GetAuthenticationSalt(StructRequest):
    """Returns a 16-byte random string that is to be prepended to the password when hashed"""
    id = 3
    responseFormat = '16s'


class Request_Authenticate(StructRequest):
    """The MD5 hash of the salt+password is sent. On success, returns a list of access rights
       as a tuple of strings containing 'read' and/or 'write'. Raises an AuthenticationError
       if the password was incorrect.
       """
    id = 4
    requestFormat = '16s'
    responseFormat = '<II'

    def __init__(self, salt, password):
        StructRequest.__init__(self)
        self.parameters = (md5.md5(salt + password).digest(),)

    def receivedResponse(self, source, status, accessRights):
        self.decodeStatus(status)
        if accessRights == 0:
            self.result.callback(('read',))
        elif accessRights == 1:
            self.result.callback(('read', 'write'))
        else:
            raise ProtocolError("Unexpected access rights code: %d" % accessRights)


class Request_GetDeviceDetails(StructRequest):
    """Returns the device's name, version, and number of storage devices"""
    id = 5
    responseFormat = '<32s32sI'

    def receivedResponse(self, source, name, version, numStorageDevices):
        """Trim NULs from the received strings"""
        self.result.callback(( name.replace("\x00", ""),
                               version.replace("\x00", ""),
                               numStorageDevices ))


class Request_GetStorageDetails(StructRequest):
    """For any storage device, returns the number of files, storage size,
       free space, and the highest file ID.
       """
    id = 6
    requestFormat = '<I'
    responseFormat = '<IQQI'

    def __init__(self, storageId):
        self.parameters = (storageId,)
        StructRequest.__init__(self)


class Request_GetDeviceSettings(StructRequest):
    """Return a dictionary with the device's settings"""
    id = 7
    responseFormat = '<I'

    def receivedResponse(self, source, status):
        """After receiving the fixed portion of this reply, we swap
           over our readResponse() callback to readSettings, which
           buffers them into a PropertyFileAdaptor object.
           """
        self.decodeStatus(status)
        self.properties = PropertyFileAdaptor({})

        self.readResponse = self.readSettings
        self.reader = AlignedStringReader()
        self.readSettings(source)

    def readSettings(self, fileObj):
        if not self.reader.next(fileObj, self.properties):
            self.result.callback(self.properties.db)


class Request_RequestIOLock(StructRequest):
    """Request an I/O lock. The lock type must be 'read' or 'write'.
       You must have at least read lock to retrieve information from the audio database,
       and at least a write lock to upload new data.
       """
    id = 9
    requestFormat = '<I'
    responseFormat = '<I'
    lockTypes = {
        'read': 0,
        'write': 1,
        }

    def __init__(self, lockType):
        try:
            self.parameters = (self.lockTypes[lockType],)
        except KeyError:
            raise ProtocolError("Unknown lock type %r" % lockType)
        StructRequest.__init__(self)

    def receivedResponse(self, source, status):
        self.decodeStatus(status)
        self.result.callback(None)


class Request_ReleaseIOLock(StructRequest):
    """Release any I/O locks that are currently held"""
    id = 10
    responseFormat = '<I'

    def receivedResponse(self, source, status):
        self.decodeStatus(status)
        self.result.callback(None)


class Request_WriteFileChunk(StructRequest):
    id = 12
    def __init__(self):
        raise NotImplementedError


class Request_GetAllFileDetails(StructRequest):
    """Read details for all files, sending each file properties
       dictionary to a provided callback.
       """
    id = 13
    responseFormat = '<I'

    def __init__(self, callback):
        self.callback = callback
        StructRequest.__init__(self)

    def receivedResponse(self, source, status):
        self.decodeStatus(status)
        self.fileDatabase = FileDatabaseWriter(self.callback)

        self.readResponse = self.readSettings
        self.reader = AlignedStringReader()
        self.readSettings(source)

    def readSettings(self, fileObj):
        if not self.reader.next(fileObj, self.fileDatabase):
            self.result.callback(None)


class Request_GetFileDetails(StructRequest):
    """Reads metadata for one file, by ID. Optionally a user-defined
       dictionary-like object may be provided as storage.
       """
    id = 14
    requestFormat = '<I'
    responseFormat = '<I'

    def __init__(self, fileID, storage=None):
        if storage is None:
            storage = {}
        self.storage = storage

        self.parameters = (fileID,)
        StructRequest.__init__(self)

    def receivedResponse(self, source, status):
        self.decodeStatus(status)
        self.properties = PropertyFileWriter(self.storage)

        self.readResponse = self.readDetails
        self.reader = AlignedStringReader()
        self.readDetails(source)

    def readDetails(self, fileObj):
        if not self.reader.next(fileObj, self.properties):
            self.result.callback(self.storage)


class Request_UpdateFileDetails(StructRequest):
    """Update metadata for one file, by ID. The metadata is specified
       as a dictionary-like object.
       """
    id = 15
    requestFormat = '<I'
    responseFormat = '<I'

    def __init__(self, fileId, data):
        self.parameters = (fileId,)
        self.properties = PropertyFileReader(data)
        StructRequest.__init__(self)

    def receivedResponse(self, source, status):
        self.decodeStatus(status)
        self.result.callback(None)

    def sendTo(self, fileObj):
        StructRequest.sendTo(self, fileObj)
        AlignedStringWriter().next(self.properties, fileObj)


class Request_ReadFileChunk(StructRequest):
    id = 16
    def __init__(self):
        raise NotImplementedError


class Request_DeleteFile(StructRequest):
    """Delete a file from storage, given its ID"""
    id = 17
    requestFormat = '<I'

    def __init__(self, fileId):
        self.parameters = (fileId,)
        StructRequest.__init__(self)


class Request_Hangup(StructRequest):
    """Indicates that the conversation is over"""
    id = 19
    responseFormat = '<I'

    def receivedResponse(self, source, status):
        self.decodeStatus(status)


############################################################################
#####################################################  Property Storage  ###
############################################################################

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


class AlignedStringWriter:
    """This object copies all data from the source into one
       4-byte-aligned null terminated string in the destination.
       """
    blockSize = 4096

    def next(self, source, destination):
        totalLength = 0

        # Copy as many complete blocks as we can, tracking the total size
        while 1:
            block = source.read(self.blockSize)
            totalLength += len(block)
            if len(block) == self.blockSize:
                destination.write(block)
            else:
                break

        # Null-terminate the last block
        block += "\x00" * (4 - (totalLength & 0x03))
        destination.write(block)


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
            return "%s=%s\n" % self._readIter.next()
        except StopIteration:
            return ""

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


############################################################################
######################################################  File Management  ###
############################################################################

class AllocationTree:
    """This is a data structure for keeping track of unused areas of an
       abstract address space, marking areas as used, and looking for unused
       areas. It is a binary tree, with maximum depth equal to the number
       of bits in the address space. Every node (represented by a list) can
       either lead to another node or to a boolean indicating whether that
       space is used.

       If the tree were fully populated, it would be no better than a lookup
       table holding a flag for every address, scanned linearly every time we
       wanted to find an available one. To avoid this, the tree is optimized-
       a node with leaves for both children may only exist if the leaves have
       different values. Initially the tree's root node is a False leaf,
       indicating that the entire address space is empty. When we allocate
       space, leaves split into nodes as necessary. When traversing the tree
       looking for available space, this lets us jump over large blocks of
       unavailable space very quickly. This is very similar to the Karnaugh
       maps used to optimize simple digital logic circuits.
       """
    def __init__(self, bits):
        self.mask = 1 << (bits - 1)
        self.data = False

    def isAllocated(self, address):
        """Lookup the boolean value of a particular address"""
        data = self.data
        mask = self.mask

        while type(data) is list:
            data = data[bool(address & mask)]
            mask >>= 1

        return data

    def findFirst(self):
        """Find the first free location"""
        return self._findFirst(self.data, self.mask, 0)

    def _findFirst(self, data, mask, address):
        if type(data) is list:
            # A node- if the zero child isn't True, we can always follow
            # it and find a free address.
            if data[0] == True:
                return self._findFirst(data[1], mask >> 1, address | mask)
            else:
                return self._findFirst(data[0], mask >> 1, address)
        elif data:
            # Everything's allocated
            return None
        else:
            # Everything's free, return the first available
            # address by refraining from setting any additional
            # bits to the right of 'mask'
            return address

    def allocate(self, address):
        """Mark the given address as used"""
        mask = self.mask
        node = self.data
        getNode = lambda: self.data
        setNode = lambda x: setattr(self, 'data', x)
        stack = [(getNode, setNode)]

        while 1:
            if type(node) is not list:
                if node:
                    # Everything down this path is already allocated and we're done.
                    return
                else:
                    # Are we at the bottom of the tree yet?
                    if mask == 0:
                        # Yes. This leaf describes our address and only our address.
                        # We can safely mark it as True.
                        node = True
                        setNode(node)
                        break
                    else:
                        # Nope. We should split this leaf into a node with two
                        # False children. This isn't a legal condition for our
                        # tree, but it will be fixed up below.
                        node = [False, False]
                        setNode(node)

            # Traverse down a level
            i = bool(address & mask)
            getNode = lambda node=node, i=i: node.__getitem__(i)
            setNode = lambda x, node=node, i=i: node.__setitem__(i, x)
            node = node[i]
            mask >>= 1
            stack.append((getNode, setNode))

        # Now wander back up the tree, looking for nodes we can collapse into leaves
        stack.reverse()
        for getNode, setNode in stack:
            node = getNode()
            if node == [False, False]:
                setNode(False)
            elif node == [True, True]:
                setNode(True)


class FileIdAllocator:
    """File IDs, from experimentation, seem to be multiples of 0x10 between 0x110 and
       0xFFFF0. This uses a 16-bit-deep AllocationTree, with bit shifting to hide the
       requirement that IDs be multiples of 0x10. Additionally, we pre-mark IDs below
       0x110 as unusable.
       """
    def __init__(self):
        self.tree = AllocationTree(16)
        for i in xrange(0x11):
            self.tree.allocate(i)

    def isAllocated(self, address):
        return self.tree.isAllocated(address >> 4)

    def allocate(self, address):
        return self.tree.allocate(address >> 4)

    def findFirst(self):
        return self.tree.findFirst() << 4

    def next(self):
        i = self.findFirst()
        if i is not None:
            self.allocate(i)
        return i


class Cache:
    """This is a container for the several individual databases that make up
       our cache of the Rio's file database. It is responsible for opening,
       closing, synchronizing, and resetting all caches in unison.
       """
    def __init__(self, path):
        self.path = os.path.expanduser(path)

    def _openShelve(self, name, mode):
        """This is a convenience function for opening shelve databases using
           a common naming convention and parent directory.
           """
        return shelve.open(os.path.join(self.path, name + '.db'), mode)

    def _openPickle(self, name, mode, factory, *args, **kwargs):
        """This is a convenience function for opening pickled objects in the same
           fashion as an anydbm database.
           """
        filename = os.path.join(self.path, name + '.p')
        if mode == 'n' or not os.path.isfile(filename):
            return factory(*args, **kwargs)
        else:
            return cPickle.load(open(filename, "rb"))

    def _savePickle(self, obj, name):
        """A convenience function for saving pickled objects using
           our common naming convention
           """
        filename = os.path.join(self.path, name + '.p')
        cPickle.dump(obj, open(filename, "wb"), cPickle.HIGHEST_PROTOCOL)

    def open(self, mode='c'):
        """Open all cache components, creating them if necessary"""
        if not os.path.isdir(self.path):
            os.makedirs(self.path)

        self.fileDetails     = self._openShelve('file-details', mode)
        self.fileIdAllocator = self._openPickle('file-id-allocator', mode, FileIdAllocator)

    def close(self):
        self.sync()
        self.quickClose()

    def sync(self):
        """Synchronize in-memory parts of the cache with disk"""
        self.fileDetails.sync()
        self._savePickle(self.fileIdAllocator, 'file-id-allocator')

    def quickClose(self):
        """Close without regard to data integrity! This will close our
           databases, but does not flush in-memory caches to disk.
           This should only be used when we're throwing the cache
           away soon anyway.
           """
        self.fileDetails.close()

    def empty(self):
        """Discard the contents of the cache"""
        self.quickClose()
        self.open('n')


class Filesystem:
    """The Filesystem is a high-level abstraction for the Rio's file database.
       This includes a local cache of the device's database, and interfaces
       for obtaining and storing File objects.
       """
    def __init__(self, protocol, cacheDir="~/.riokarma-py/cache"):
        self.protocol = protocol
        self.cache = Cache(cacheDir)
        self.cache.open()

    def synchronize(self):
        """Update our local database if necessary. Returns a Deferred
           signalling completion. This has the side-effect of acquiring
           a read lock.
           """
        d = defer.Deferred()
        self.protocol.sendRequest(Request_RequestIOLock('read')).addCallback(
            self._startSynchronize, d).addErrback(d.errback)
        return d

    def _startSynchronize(self, retval, d):
        if self.isCacheDirty():
            # Empty and rebuild all database files
            self.cache.empty()
            self.protocol.sendRequest(Request_GetAllFileDetails(
                self._storeFileDetails)).addCallback(
                self._finishSynchronize, d).addErrback(d.errback)
        else:
            d.callback(None)

    def _finishSynchronize(self, retval, d):
        # Before signalling completion, sync the cache
        self.cache.sync()
        d.callback(None)

    def isCacheDirty(self):
        """This function determines whether our local cache of the database
           is still valid. If not, we should retrieve a new copy.
           """
        # FIXME
        if len(self.cache.fileDetails) == 0:
            return True
        else:
            return False

    def _storeFileDetails(self, details):
        """Add a file details dictionary to our database cache. This is used to populate
           the database when synchronizing from the device.
           """
        self.cache.fileIdAllocator.allocate(int(details['fid']))
        self.cache.fileDetails[details['fid']] = details

        print hex(int(details['fid']))


class File:
    """A File represents one media or data file corresponding to an entry in
       the device's database and our copy of that database. File objects
       have lifetimes disjoint from that of the actual files on disk- if a
       file ID is provided, existing metadata is looked up. If an ID isn't
       provided, we generate a new ID and metadata will be uploaded.
       """
    def __init__(self, filesystem, fileID=None):

        if fileID is None:
            fileID = self.filesystem.newFileID()


### The End ###

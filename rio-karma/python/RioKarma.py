""" RioKarma

This module implements the 'Pearl' protocol used to communicate with
the Rio Karma portable audio player over TCP/IP.

This module wouldn't have been possible without the reverse-engineering
work done by the Karmalib project:
    http://karmalib.sourceforge.net/docs/protocol.php

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

import Queue, struct, md5
from cStringIO import StringIO
from twisted.internet import protocol, defer, reactor
from twisted.python import failure, log


############################################################################
###################################################  Low-level Protocol  ###
############################################################################

class ProtocolError(Exception):
    pass

class AuthenticationError(Exception):
    pass


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


class PearlRequest:
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

        if status == 0x8004005B:
            raise AuthenticationError("Password is incorrect")

        raise ProtocolError("Unexpected status 0x%08X for request %r" %
                            (status, self))


class StructRequest(PearlRequest):
    """This is an abstract base class for PearlRequests where the requests
       and replies can be described, at least in part, using the struct module.
       """
    requestFormat = ''
    responseFormat = ''
    parameters = ()

    def __init__(self):
        PearlRequest.__init__(self)

        # Add the implied header
        self._requestFormat = "<I" + self.requestFormat
        self._responseFormat = "<I" + self.responseFormat

        self._responseLength = struct.calcsize(self._responseFormat) + len(self.MAGIC)
        self._responseBuffer = ''

        # By default, our response handler is readStructResponse. If subclasses
        # need to read extra data after what we can handle, they can reassign
        # this to point to a new handler in receivedResponse.
        self.readResponse = self.readStructResponse

    def sendTo(self, fileObj):
        fileObj.write(self.MAGIC + struct.pack(self._requestFormat,
                                               self.id, *self.parameters))

    def readStructResponse(self, fileObj):
        # Try to read just enough data to complete this response
        remaining = self._responseLength - len(self._responseBuffer)
        if remaining > 0:
            self._responseBuffer += fileObj.read(remaining)

        responseLen = len(self._responseBuffer)
        if responseLen > self._responseLength:
            raise ProtocolError("Request %r read too much data (have %d, need %d)" %
                                (self, responseLen, self._responseLength))

        if responseLen == self._responseLength:
            # Check the magic
            if not self._responseBuffer.startswith(self.MAGIC):
                raise ProtocolError("Connection is out of sync, didn't receive header for %r" % self)

            # Decode it and check the ID
            response = struct.unpack(self._responseFormat, self._responseBuffer[len(self.MAGIC):])
            if response[0] != self.id:
                raise ProtocolError("Connection is out of sync, request #%d received a reply for #%d" %
                                    (self.id, response[0]))

            # Yay, pass the rest on
            self.receivedResponse(*response[1:])

    def receivedResponse(self, *args):
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


class VariableRequest(StructRequest):
    """This class builds on StructRequest by providing alternate read and write handlers
       for sending and receiving variable-length data, both sized and null-terminated.
       All variable-length blocks are padded to 4-byte boundaries, and all variable-length
       data is read/written using file-like objects.
       """
    def readNullTerminated(self, fileObj, destination):
        """Read any pending data from fileObj, writing it to destination.
           This won't read past the first 4-byte block containing a null
           terminator. Returns True if we're finished, False if we're out
           of data.
           """
        while 1:
            block = fileObj.read(4)
            if not block:
                return False

            i = block.find('\x00')
            if i >= 0:
                destination.write(block[:i])
                return True
            else:
                destination.write(block)

    def writeNullTerminated(self, fileObj, source, blockSize=4096):
        """Write data from source to destination, null-terminating and
           padding to a 4-byte boundary.
           """
        totalLength = 0

        # Copy as many complete blocks as we can, tracking the total size
        while 1:
            block = source.read(blockSize)
            totalLength += block
            if block == blockSize:
                fileObj.write(block)
            else:
                break

        # Null-terminate the last block
        block += "\x00" * (4 - (totalLength & 0x03))
        fileObj.write(block)


############################################################################
##############################################  Request Implementations  ###
############################################################################

class Request_GetProtocolVersion(StructRequest):
    """Return the major and minor version of this device's protocol"""
    id = 0
    parameters = (0,0)
    requestFormat = 'II'
    responseFormat = 'II'


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
    responseFormat = 'II'

    def __init__(self, salt, password):
        StructRequest.__init__(self)
        self.parameters = (md5.md5(salt + password).digest(),)

    def receivedResponse(self, status, accessRights):
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
    responseFormat = '32s32sI'

    def receivedResponse(self, name, version, numStorageDevices):
        """Trim NULs from the received strings"""
        self.result.callback(( name.replace("\x00", ""),
                               version.replace("\x00", ""),
                               numStorageDevices ))


class Request_GetStorageDetails(StructRequest):
    """For any storage device, returns the number of files, storage size,
       free space, and the highest file ID.
       """
    id = 6
    requestFormat = 'I'
    responseFormat = 'IQQI'

    def __init__(self, storageId):
        self.parameters = (storageId,)
        StructRequest.__init__(self)


class Request_GetDeviceSettings(VariableRequest):
    """Return a PropertiesFile object with the device's settings"""
    id = 7
    responseFormat = 'I'

    def receivedResponse(self, status):
        """After receiving the fixed portion of this reply, we swap
           over our readResponse() callback to readSettings, which
           buffers them into a PropertiesFile object.
           """
        self.decodeStatus(status)
        self.properties = PropertiesFile()
        self.readResponse = self.readSettings

    def readSettings(self, fileObj):
        if self.readNullTerminated(fileObj, self.properties):
            self.result.callback(self.properties)


class FIXME:
    pass


class Request_UpdateDeviceSettings(FIXME):
    id = 8


class Request_RequestIOLock(StructRequest):
    """Request an I/O lock. The lock type must be 'read' or 'write'.
       You must have at least read lock to retrieve information from the audio database,
       and at least a write lock to upload new data.
       """
    id = 9
    requestFormat = 'I'
    responseFormat = 'I'
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

    def receivedResponse(self, status):
        self.decodeStatus(status)


class Request_ReleaseIOLock(StructRequest):
    """Release any I/O locks that are currently held"""
    id = 10
    responseFormat = 'I'

    def receivedResponse(self, status):
        self.decodeStatus(status)


class Request_WriteFileChunk(FIXME):
    id = 12


class Request_GetAllFileDetails(FIXME):
    id = 13


class Request_GetFileDetails(FIXME):
    id = 14


class Request_UpdateFileDetails(FIXME):
    id = 15


class Request_ReadFileChunk(FIXME):
    id = 16


class Request_DeleteFile(StructRequest):
    """Delete a file from storage, given its ID"""
    id = 17
    requestFormat = 'I'

    def __init__(self, fileId):
        self.parameters = (fileId,)
        StructRequest.__init__(self)


############################################################################
#################################################  High-level Protocols  ###
############################################################################

class PropertiesFile(dict):
    """This class implements the common 'properties file' key-value databases
       used by the Rio Karma. It is a dictionary that can be read or written
       like a file, using this protocol.
       """
    _writeBuffer = ''
    _readBuffer = ''
    _readIter = None

    def write(self, data):
        """Write an arbitrary amount of data, buffering any incomplete lines"""
        buffer = self._writeBuffer + data
        fragments = buffer.split("\n")
        self._writeBuffer = fragments[-1]
        for line in fragments[:-1]:
            self.writeLine(line)

    def writeLine(self, line):
        """Write a complete key=value line"""
        key, value = line.strip().split("=", 1)
        self[key] = value

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

    def readLine(self):
        """Read exactly one key=value line, with \n termination. Returns the
           empty string if we have no more data.
           """
        if self._readIter is None:
            self._readIter = self.iteritems()
        try:
            return "%s=%s\n" % self._readIter.next()
        except StopIteration:
            return ""

    def rewind(self):
        """Seek back to the beginning of the buffer. This also revalidates
           a bad iterator if the dictionary was modified while reading.
           """
        self._readIter = None


class AuthenticatedProtocol(RequestResponseProtocol):
    """This builds on the RequestResponseProtocol to include authentication,
       initiated when the protocol connects successfully.
       """
    def connectionMade(self):
        self.sendRequest(Request_GetAuthenticationSalt()).addCallback(
            self._finishAuthentication).addErrback(self.authenticationFailed)

    def authenticationFailed(self, err):
        """Subclasses can override this to handle errors in the authentication process"""
        log.err(err)

    def _finishAuthentication(self, salt):
        self.sendRequest(Request_Authenticate(salt, self.factory.password)).addCallback(
            self.authenticated).addErrback(self.authenticationFailed)

    def authenticated(self, rights):
        """Authentication has finished. Subclasses can override this to receive
           notification when the connection is ready to use, and to see the available
           access rights.
           """
        pass


############################################################################
#################################################################  Demo  ###
############################################################################

class Request_Hangup(StructRequest):
    """Indicates that the conversation is over"""
    id = 19
    responseFormat = 'I'

    def receivedResponse(self, status):
        self.decodeStatus(status)


if __name__ == "__main__":
    def show(v):
        print "Response: %r" % (v,)

    class TestProtocol(AuthenticatedProtocol):
        def authenticated(self, rights):
            print "Authenticated"
            self.sendRequest(Request_GetDeviceSettings()
                             ).addCallback(show).addErrback(log.err)

    factory = protocol.ClientFactory()
    factory.password = "orange"
    factory.protocol = TestProtocol
    reactor.connectTCP("10.0.0.80", 8302, factory)
    reactor.run()

### The End ###

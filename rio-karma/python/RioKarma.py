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
        if status != 0:
            raise ProtocolError("Unexpected status 0x%08X for request %r" %
                                (status, self))


class FixedRequest(PearlRequest):
    """This is an abstract base class for PearlRequests with fixed-length
       requests and responses encoded using struct.
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

    def sendTo(self, fileObj):
        fileObj.write(self.MAGIC + struct.pack(self._requestFormat,
                                               self.id, *self.parameters))

    def readResponse(self, fileObj):
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


class Request_GetProtocolVersion(FixedRequest):
    """Return the major and minor version of this device's protocol"""
    id = 0
    parameters = (0,0)
    requestFormat = 'II'
    responseFormat = 'II'

class Request_GetAuthenticationSalt(FixedRequest):
    """Returns a 16-byte random string that is to be prepended to the password when hashed"""
    id = 3
    responseFormat = '16s'

class Request_Authenticate(FixedRequest):
    """The MD5 hash of the salt+password is sent. On success, returns a list of access rights
       as a tuple of strings containing 'read' and/or 'write'. Raises an AuthenticationError
       if the password was incorrect.
       """
    id = 4
    requestFormat = '16s'
    responseFormat = 'II'

    def __init__(self, salt, password):
        FixedRequest.__init__(self)
        self.parameters = (md5.md5(salt + password).digest(),)

    def receivedResponse(self, status, accessRights):
        if status == 0x8004005B:
            raise AuthenticationError("Password is incorrect")

        self.decodeStatus(status)
        if accessRights == 0:
            self.result.callback(('read',))
        elif accessRights == 1:
            self.result.callback(('read', 'write'))
        else:
            raise ProtocolError("Unexpected access rights code: %d" % accessRights)

class Request_GetDeviceDetails(FixedRequest):
    """Returns the device's name, version, and number of storage devices"""
    id = 5
    responseFormat = '32s32sI'

    def receivedResponse(self, name, version, numStorageDevices):
        """Trim NULs from the received strings"""
        self.result.callback(( name.replace("\x00", ""),
                               version.replace("\x00", ""),
                               numStorageDevices ))


if __name__ == "__main__":
    def show(v):
        print "Response: %r" % (v,)

    class TestProtocol(AuthenticatedProtocol):
        def authenticated(self, rights):
            print "Authenticated"
            self.sendRequest(Request_GetDeviceDetails()
                             ).addCallback(show).addErrback(log.err)

    factory = protocol.ClientFactory()
    factory.password = "orange"
    factory.protocol = TestProtocol
    reactor.connectTCP("10.0.0.80", 8302, factory)
    reactor.run()

### The End ###

""" BZFlag.Network

Network transport layer for BZFlag clients and servers. This
provides methods for transporting messges across TCP and UDP
links.
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

from BZFlag.Protocol import Common
from BZFlag import Protocol, Errors
import socket, select, sys


class Socket:
    """This is a socket wrapper that can be used for normal socket
       operations, but also supports sending BZFlag messages.
       """
    def __init__(self, protocol='TCP'):
        self.readBuffer = ''
        self.socket = getattr(self, "new%sSocket" % protocol)()
    
    def newTCPSocket(self):
        """Create a new TCP socket, setting the proper options"""
        tcp = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

        # Disable the nagle algorithm. This is necessary to get
        # anything near reasonable latency when sending small packets.        
        try:
            tcp.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
        except:
            pass
        return tcp

    def newUDPSocket(self):
        return socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    def close(self):
        self.socket.close()
        self.socket = None

    def setBlocking(self, flag):
        self.socket.setblocking(flag)
    
    def write(self, data):
        self.socket.send(str(data))

    def recv(self, size):
        """Low-level recv() wrapper that just provides a little error handling."""
        try:
            data = self.socket.recv(size)
        except socket.error:
            # Assume this is "resource temporarily unavailable"
            return None
        if not data:
            raise Errors.ConnectionLost()
        return data
                
    def read(self, size=None, bufferSize=64*1024):
        """High level interface for reading from the socket,
           includes a buffering scheme that works well for
           receiving fixed size messages.
           """
        if size is None:
            # Keep reading until there's no more to read
            received = self.readBuffer
            self.readBuffer = ''
            try:
                while 1:
                    received += self.recv(bufferSize)
            except Errors.ConnectionLost:
                pass
            return received
        elif size != 0:
            # Read the amount specified, first from our
            # local read buffer then from the socket itself.
            # If we can't read the complete packet, return
            # None and keep the partial packet buffered.
            while len(self.readBuffer) < size:
                chunk = self.recv(size - len(self.readBuffer))
                if not chunk:
                    return None
                self.readBuffer += chunk
            received = self.readBuffer[:size]
            self.readBuffer = self.readBuffer[size:]
            return received
        else:
            # Zero size
            return ''

    def unread(self, data):
        """Push the supplied data back into the read buffer,
           so that it will be the next thing read.
           """
        self.readBuffer = str(data) + self.readBuffer

    def connect(self, host, port=None):
        """The port can be specified either as part of the host using
           a hostname:port format, or as a separate argument. A port
           included in the host overrides the port in the arguments
           if present, so you can treat the port argument as a default.
           """
        if host.find(":") >= 0:
            (host, port) = host.split(":")
            port = int(port)
        self.socket.connect((host, port))

    def poll(self, eventLoop):
        """This is designed for event driven programming with
           one or more sockets. The event loop should call this
           method when it detects activity on the socket, then
           this method will call the current socket handler.
           If there really wasn't any data available (assuming
           this is a nonblocking socket) it will return
           uneventfully.
           """
        self.handler(self, eventLoop)

    def getSelectable(self):
        """Return an object for this socket that can be passed
           to select()
           """
        return self.socket.fileno()
        
    def readStruct(self, structClass):
        struct = structClass()
        packed = self.read(struct.getSize())
        if not packed:
            return None
        struct.unmarshall(packed)
        return struct

    def readMessage(self):
        header = self.readStruct(Common.MessageHeader)
        if not header:
            return None
        if header.length > 0:
            body = self.read(header.length)
            if not body:
                # Save the header for later if we can't read the body yet
                self.unread(header)
                return None
        else:
            body = ''
        try:
            msgClass = Common.getMessageDict()[header.id]
        except KeyError:
            raise Errors.ProtocolException("Received unknown message type 0x%04X" % header.id)
        return msgClass(str(header) + body)


class EventLoop:
    def run(self, sockets):
        self.running = 1

        # Make a dictionary for quickly detecting which socket has activity
        selectDict = {}
        for socket in sockets:
            selectable = socket.getSelectable()
            selectDict[selectable] = socket
        selectables = selectDict.keys()
            
        while self.running:
            (iwtd, owtd, ewtd) = select.select(selectables, [], [])
            readyList = iwtd + owtd + ewtd
            for ready in readyList:
                try:
                    selectDict[ready].poll(self)
                except Errors.NonfatalException:
                    # Catch nonfatal exceptions
                    import sys
                    print "*** %s : %s" % (sys.exc_info()[1].__class__.__name__, sys.exc_info()[1])

    def stop(self):
        self.running = 0

### The End ###
        
    

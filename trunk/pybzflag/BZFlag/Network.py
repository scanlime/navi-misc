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

import BZFlag.Protocol.Common
import socket, select


class Socket:
    """This is a socket wrapper that can be used for normal socket
       operations, but also supports sending BZFlag messages.
       """
    def __init__(self, protocol='TCP'):
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

    def read(self, size=None, bufferSize=64*1024):
        if size:
            return self.socket.recv(size)
        else:
            # Keep reading until there's no more to read
            received = ''
            while 1:
                buffer = self.socket.recv(bufferSize)
                if not buffer:
                    break
                received += buffer
            return received

    def connect(self, host, port=None):
        """The port can be specified either as part of the host using
           a hostname:port format, or as a separate argument. A port
           included in the host overrides the port in the arguments
           if present, so you can treat the port argument as a default.
           """
        if host.find(":"):
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
        try:
            self.handler(self, eventLoop)
        except socket.error:
            pass

    def getSelectable(self):
        """Return an object for this socket that can be passed
           to select()
           """
        return self.socket.fileno()
        
    def readStruct(self, structClass):
        struct = structClass()
        struct.unmarshall(self.read(struct.getSize()))
        return struct

    def readMessage(self):
        pass


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
            (iwtd, owtd, ewtd) = select.select(selectables, [], selectables)
            readyList = iwtd + owtd + ewtd
            for ready in readyList:
                selectDict[ready].poll(self)

    def stop(self):
        self.running = 0

### The End ###
        
    

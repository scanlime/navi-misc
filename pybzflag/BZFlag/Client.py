""" BZFlag.Client

Provides the BaseClient class, which implements basic communication
with the server and provides hooks for adding more functionality
in subclasses.
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

import BZFlag
from BZFlag import Network, Protocol
from BZFlag.Protocol import FromServer, ToServer, Common


class BaseClient:
    """Implements the simplest possible BZFlag client. Subclasses
       can build on this with functionality to implement a player
       or a UI.
       """
    def __init__(self, server=None):
        self.tcp = None
        if server:
            self.connect(server)

    def connect(self, server):
        """This does the bare minimum necessary to connect to the
           BZFlag server. It does not negotiate flags, obtain the
           world database, or join the game. After this function
           returns, the client is connected to the server and can
           receive and transmit messages.
           """
        # Establish the TCP socket
        if self.tcp:
            self.disconnect()
        self.tcp = Network.Socket()
        self.tcp.connect(server, Common.defaultPort)

        # Wait for the Hello packet, with the
        # server version and cilent ID
        hello = self.tcp.readStruct(FromServer.HelloPacket)
        if hello.version != BZFlag.protocolVersion:
            raise Protocol.ProtocolError(
                "Protocol version mismatch: The server is version " +
                "'%s', this client is version '%s'." % (
                hello.version, BZFlag.protocolVersion))
        self.id = hello.clientId

    def disconnect(self):
        # Send a MsgExit first as a courtesy
        self.tcp.write(ToServer.MsgExit())
        if self.tcp:
            self.tcp.close()
            self.tcp = None        

### The End ###
        
    

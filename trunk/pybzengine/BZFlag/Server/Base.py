""" BZFlag.Server.Base

Provides the BaseServer class, which implements basic communication
with clients and provides hooks for adding more functionality
in subclasses.
"""
#
# Python BZFlag Package
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
from BZFlag import Network, Protocol, Event, Errors


class ClientInfo:
    """A container for the sockets and IDs related to one client"""
    def __init__(self, server, id, socket):
        self.id  = id
        self.server = server
        self.socket = socket
        self.address = socket.address
        self.udpPort = None

    def __str__(self):
        return "<Client #%d at %s:%s>" % (self.id, self.address[0], self.address[1])

    def write(self, data, protocol='tcp'):
        if protocol == 'udp' and self.udpPort:
            self.server.udp.writeTo(data, (self.address[0], self.udpPort))
        else:
            self.socket.write(data)


class BaseServer(Network.Endpoint):
    """On top of the basic message processing provided by Network.Endpoint,
       this class manages connections from clients.
       """
    outgoing = Protocol.FromServer
    incoming = Protocol.ToServer
    clientInfoClass = ClientInfo

    def init(self):
        Network.Endpoint.init(self)
        self.protocolVersion = BZFlag.protocolVersion
        self.clientsByID = {}
        self.clientsBySocket = {}
        self.clientsByIP = {}
        self.nextClientID = 0
        self.clientIDIncrement = 1
        self.options.update({
            'interface': '',
            })
        Event.attach(self, 'onListen', 'onConnect', 'onDisconnect')

    def setOptions(self, **options):
        Network.Endpoint.setOptions(self, **options)
        if 'interface' in options.keys():
            self.listen(options['interface'])

    def listen(self, interface):
        """Listen for multiple protocols on the same interface and port"""

        self.tcp = Network.TCPSocket()
        self.tcp.listen(interface, Protocol.Common.defaultPort)
        self.tcp.handler = self.handleConnection
        self.eventLoop.add(self.tcp)

        self.udp = Network.UDPSocket()
        self.udp.listen(interface, Protocol.Common.defaultPort)
        self.udp.handler = self.handleMessage
        self.eventLoop.add(self.udp)

        self.onListen()

    def getNewClientID(self):
        id = self.nextClientID
        self.nextClientID += self.clientIDIncrement
        return id

    def handleConnection(self, socket, eventLoop):
        """This is called when a new connection to the server is made.
           It sends the hello packet, and sets up a new client socket.
           """
        # Open a new client connection, add to our event loop
        clientSocket = socket.accept()
        clientSocket.handler = self.handleMessage
        self.eventLoop.add(clientSocket)

        # Generate a new client ID, and tell the client in a hello packet
        clientId = self.getNewClientID()
        hello = self.outgoing.HelloPacket(version = self.protocolVersion,
                                          clientId = clientId)
        clientSocket.write(hello)

        # Create a class to contain our client info, and file
        # it by ID, TCP socket, and by address.
        info = self.clientInfoClass(self, clientId, clientSocket)
        self.clientsByID[info.id] = info
        self.clientsBySocket[info.socket] = info

        # Keep a list of all clients at any particular IP address
        ip = info.address[0]
        if not self.clientsByIP.has_key(ip):
            self.clientsByIP[ip] = []
        self.clientsByIP[ip].append(info)

        self.onConnect(info)

    def handleMessage(self, socket, eventLoop):
        """This is called when new data is received from the client.
           It is a wrapper around Network.Endpoint.handleMessage which
           catches ConnectionLost exceptions and disconnects the client
           safely.
           """
        try:
            Network.Endpoint.handleMessage(self, socket, eventLoop)
        except Errors.ConnectionLost:
            try:
                info = self.clientsBySocket[socket]
                del self.clientsByID[info.id]
                del self.clientsBySocket[info.socket]

                # clientsByID might still contain other clients at the same IP
                ip = info.address[0]
                l = self.clientsByIP[ip]
                l.remove(info)
                if not l:
                    del self.clientsByIP[ip]

                self.eventLoop.remove(socket)
                self.onDisconnect(info)
            except KeyError:
                # Already disconnected
                pass

    def getClientByUDPAddress(self, udpAddress):
        """Given the from address of a UDP message try to determine
           what client it came from.
           """
        # We can narrow it down to all the clients at one IP easily
        try:
            clients = self.clientsByIP[udpAddress[0]]
        except KeyError:
            return None

        # If there's only one, we're done. This is helpful for NAT-tolerance
        if len(clients) == 1:
            return clients[0]

        # Now find the one that has the correct UDP port
        for client in clients:
            if client.udpPort == udpAddress[1]:
                return client
        return None

    def dispatchMessage(self, msg):
        """Before sending off our message, look up what client it came from"""
        if msg.socket == self.udp:
            # Figure out where this message came from by its address.
            # If we can't, assume it's just UDP spam and ignore it.
            msg.client = self.getClientByUDPAddress(msg.fromAddress)
            if not msg.client:
                return
        else:
            msg.client = self.clientsBySocket[msg.socket]
        Network.Endpoint.dispatchMessage(self, msg)

    def onMsgNetworkRelay(self, msg):
        """This is sent by the client when it can't do multicast.
           Since we don't do multicast yet, ignore this.
           """
        pass

    def onMsgSetTTL(self, msg):
        """Also ignored because we don't do multicast"""
        pass

    def onMsgUDPLinkRequest(self, msg):
        """Client just sent us its UDP port, and wants to know ours"""
        msg.client.udpPort = msg.port
        msg.socket.write(self.outgoing.MsgUDPLinkRequest(port = self.udp.port))

    def onMsgUDPLinkEstablished(self, msg):
        if not msg.client.udpPort:
            raise Errors.ProtocolWarning(
                "Client sent a MsgUDPLinkEstablished without sending MsgUDPLinkRequest")

### The End ###

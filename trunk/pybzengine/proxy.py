#!/usr/bin/env python
#
# A simple BZFlag proxy server. Currently only works for one connection at a time.
#
from BZFlag import CommandLine, Event, Server, Client, Util
import sys

# Create a server and a client, sharing command line options and event loop
eventLoop = Event.EventLoop()
(server, client) = CommandLine.Parser(Server.BaseServer, Client.BaseClient,
                                      eventLoop = eventLoop).parse()

# Trace our connection status
print "--- Server is listening on %s:%d" % (server.tcp.interface, server.tcp.port)
client.onConnect.trace(lambda: "--- Client connected to %s" % client.options['server'])
server.onConnect.trace(lambda socket: "--- Connected client %d, from %s:%s" %
                       (socket.id, socket.address[0], socket.address[1]))
server.onDisconnect.trace(lambda socket: "--- Disconnected client %d, from %s:%s" %
                          (socket.id, socket.address[0], socket.address[1]))

proxy = Util.Proxy(server, client)
eventLoop.run()

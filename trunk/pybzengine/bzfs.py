#!/usr/bin/env python
from BZFlag import CommandLine, Server, UI

(server, ui) = CommandLine.Parser(Server.StrictServer, UI.Any,
                                  world = "Random").parse()

# Trace connections and disconnections
print "Server is listening on %s:%d" % (server.tcp.interface, server.tcp.port)
server.onConnect.trace("Connected %(1)s")
server.onDisconnect.trace("Disconnected %(1)s")

ui = ui.Setup(server.game, server.eventLoop)
server.run()

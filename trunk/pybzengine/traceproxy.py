#!/usr/bin/env python
#
# A utility that acts as a BZFlag proxy server, showing all
# messages in a human readable form.
#
from BZFlag import CommandLine, Event, Server, Client, Util
import sys

# Create a server and a client, sharing command line options and event loop
eventLoop = Event.EventLoop()
argParser = CommandLine.Parser(Server.BaseServer, Client.BaseClient,
                               eventLoop = eventLoop)
argParser.add_option("-n", "--names", action="store_true", dest="names",
                     help="Disables the output of message contents, shows only their names.")
argParser.add_option("-d", "--hide", action="append", dest="hide",
                     help="Hides the given message type.")
argParser.add_option("-o", "--show", action="append", dest="show",
                     help="Shows only the given message type.")
(server, client) = argParser.parse()
options = client.cmdLineValues

# Trace client connection
client.onConnect.trace(lambda: "--- Client connected to %s\n--- Client ID is %d" %
                       (client.options['server'], client.id))

# Trace server connections and disconnections
print "--- Server is listening on %s:%d" % (server.tcp.interface, server.tcp.port)
server.onConnect.trace(lambda socket: "--- Connected client %d, from %s:%s" %
                       (socket.id, socket.address[0], socket.address[1]))
server.onDisconnect.trace(lambda socket: "--- Disconnected client %d, from %s:%s" %
                          (socket.id, socket.address[0], socket.address[1]))

# Dump the message using Util.messageDump, filtering it according to our options
def showMessage(msg):
    name = msg.__class__.__name__
    if options['hide'] and name in options['hide']:
        return
    if options['show'] and not name in options['show']:
        return
    msg.dump(sys.stdout, not options['names'])
server.onAnyMessage.observe(showMessage)
client.onAnyMessage.observe(showMessage)

proxy = Util.Proxy(server, client)
eventLoop.run()

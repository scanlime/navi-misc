#!/usr/bin/env python

import BZFlag.Network
import BZFlag.Client
import BZFlag.ListServer
import BZFlag.Player
import BZFlag.Protocol.FromServer
import sys

if len(sys.argv) > 1:
    # There was a server name on the command line
    serverName = sys.argv[1]
else:
    # Just pick the first server on the list that's compatible with us
    server = BZFlag.ListServer.getDefault().filteredList()[0]
    serverName = server.name
    print server.info()

class TestClient(BZFlag.Client.BaseClient):
    def onConnect(self):
        print "Connected, got client id %d" % self.id
        print "Trying to join game..."
        self.enterGame(BZFlag.Player.Identity("Bob the Avenger"))

    def expectMessage(self, socket, eventLoop):
        msg = socket.readMessage()
        print msg.__class__.__name__,
        if isinstance(msg, BZFlag.Protocol.FromServer.MsgPlayerUpdate):
            print "- Player %d at %s" % (msg.id, msg.position),
        if isinstance(msg, BZFlag.Protocol.FromServer.MsgMessage):
            message = msg.message[:msg.message.find(chr(0))]
            print "- From %d, To %d: '%s'" % (msg.fromId, msg.toId, message),
        print


TestClient(serverName).run()

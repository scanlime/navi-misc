#!/usr/bin/env python

import BZFlag.Network
import BZFlag.Client
import BZFlag.ListServer
import BZFlag.Protocol.FromServer

# Just pick the first server on the list that's compatible with us
server = BZFlag.ListServer.getDefault().filteredList()[0]
print server.info()

class TestClient(BZFlag.Client.BaseClient):
    def onConnect(self):
        print "Connected, got client id %d" % self.id
        print "Trying to join game..."
        self.enterGame(BZFlag.Client.PlayerIdentity("Bob the Avenger"))

    def expectMessage(self, socket, eventLoop):
        msg = socket.readMessage()
        print msg.__class__.__name__,
        if isinstance(msg, BZFlag.Protocol.FromServer.MsgPlayerUpdate):
            print "- Player %d at %s" % (msg.id, msg.position),
        print


TestClient(server.name).run()

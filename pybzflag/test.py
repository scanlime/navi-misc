#!/usr/bin/env python

import BZFlag.Network
import BZFlag.Client
import BZFlag.ListServer

# Just pick the first server on the list that's compatible with us
server = BZFlag.ListServer.getDefault().filteredList()[0]
print server.info()

class TestClient(BZFlag.Client.BaseClient):
    def onConnect(self):
        print "Connected, got client id %d" % self.id
        print "Trying to join game..."
        self.enterGame(BZFlag.Client.PlayerInfo("Bob the Avenger"))

    def expectMessage(self, socket, eventLoop):
        print repr(socket.readMessage())


TestClient(server.name).run()

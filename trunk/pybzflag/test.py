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

TestClient(server.name).run()

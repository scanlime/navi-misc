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

class TestClient(BZFlag.Client.PlayerClient):
    pass

playerIdent = BZFlag.Player.Identity("Bob the Avenger")
client = TestClient(serverName, playerIdent)
client.run()

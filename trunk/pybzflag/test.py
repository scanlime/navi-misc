#!/usr/bin/env python

from BZFlag.Client import PlayerClient
from BZFlag import ListServer, Player
import sys


if len(sys.argv) > 1:
    # There was a server name on the command line
    serverName = sys.argv[1]
else:
    # Just pick the first server on the list that's compatible with us
    server = ListServer.getDefault().filteredList()[0]
    serverName = server.name
    print server.info()


class TestClient(PlayerClient):
    def onConnect(self):
        print "Connected."
        PlayerClient.onConnect(self)

    def downloadWorld(self):
        print "Downloading world..."
        PlayerClient.downloadWorld(self)

    def onMsgMessage(self, msg):
        print "Message from %s to %s: %s" % (msg.fromId, msg.toId, msg.message)

    def onEnterGame(self):
        print "Entered the game."


playerIdent = Player.Identity("Bob the Avenger")
client = TestClient(serverName, playerIdent)
client.run()

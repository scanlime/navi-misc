#!/usr/bin/env python

import BZFlag.Client
import BZFlag.ListServer
import BZFlag.Player
import BZFlag.Protocol.FromServer
import BZFlag.Protocol.ToServer
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
    def onMsgMessage(self, msg):
        print "Message from %s to %s: %s" % (msg.fromId, msg.toId, msg.message)

    def onEnterGame(self):
        print "Entered the game"
        pos = [10, 10, 20]
        alive = BZFlag.Protocol.ToServer.MsgAlive()
        alive.position = pos
        alive.forward  = (1,0,0)
        self.tcp.write(alive)
        update = BZFlag.Protocol.ToServer.MsgPlayerUpdate()


playerIdent = BZFlag.Player.Identity("Bob the Avenger")
client = TestClient(serverName, playerIdent)
client.run()

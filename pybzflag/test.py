#!/usr/bin/env python

from BZFlag.Client import PlayerClient
from BZFlag import ListServer, Player

class TestClient(PlayerClient):
    def onConnect(self):
        print "Connected."
        PlayerClient.onConnect(self)

    def downloadWorld(self):
        print "Downloading world..."
        PlayerClient.downloadWorld(self)

    def onLoadWorld(self):
        print "World loaded - %d blocks" % (len(self.game.world.blocks))
        PlayerClient.onLoadWorld(self)

    def onMsgMessage(self, msg):
        print "Message from %s to %s: %s" % (msg.fromId, msg.toId, msg.message)

    def onEnterGame(self):
        print "Entered the game."


TestClient("brlcad.org:4242", Player.Identity("Bob the Avenger")).run()

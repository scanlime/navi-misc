#!/usr/bin/env python

from BZFlag.Client import PlayerClient
from BZFlag import Player

client = PlayerClient("brlcad.org:4242", Player.Identity("Bob the Avenger"))

# Stick some instrumentation in select events
client.onConnect.trace("Connected.")
client.onStartWorldDownload.trace("Downloading world...")
client.onLoadWorld.trace("World loaded.")
client.onEnterGame.trace("Entered the game.")
client.game.onAddPlayer.trace("Added player %(2)s")
client.game.onRemovePlayer.trace("Removed player %(2)s")

# Show messages
def message(msg):
    print "<%s> %s" % (msg.fromId, msg.message)
client.onMsgMessage.observe(message)

client.run()

#!/usr/bin/env python
from BZFlag import CommandLine, UI, Client

(client, ui) = CommandLine.Parser(Client.PlayerClient, UI.Any,
                                  callSign = "@PyObserver",
                                  ui = 'overhead').parse()

# Stick some instrumentation in select events
client.onConnect.trace("Connected.")
client.onConnectUDP.trace(lambda: "UDP link established, local port %s" % client.udp.port)
client.onStartWorldDownload.trace("Downloading world...")
client.onEnterGame.trace("Entered the game.")
client.game.world.onLoad.trace("World loaded.")
client.game.onAddPlayer.trace("Added player %(2)s")
client.game.onRemovePlayer.trace("Removed player %(2)s")

# Show messages
def message(msg):
    print "<%s> %s" % (msg.fromId, msg.message)
client.onMsgMessage.observe(message)

ui = ui.Setup(client.game, client.eventLoop)
client.run()

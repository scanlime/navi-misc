#!/usr/bin/env python
#
# A simple chat client.
# This is suitable for interactive use, or for piping commands into.
# Imagine cron jobs spitting uptimes and disk space stats into bzflag
# so you don't miss out on such things while playing :)
#
from BZFlag import CommandLine, Client, UI
from threading import *
import sys, time

argParser = CommandLine.Parser(Client.PlayerClient, UI.Any,
                               callSign = "@PyChat")
argParser.add_option("-q", "--quiet", action="store_true", dest="quiet",
                     help="Disables the output of connection status messages and protocol warnings.")
argParser.add_option("-i", "--silent", action="store_true", dest="silent",
                     help="Disables all output.")
argParser.add_option("-w", "--wait", dest="wait", metavar="SECONDS", default=0,
                     help="Waits the given number of seconds after an EOF before leaving the game.")
argParser.add_option("-v", "--view", dest="view", action="store_true", 
                     help="Shows an overhead view of the game during chat.")
(client, ui) = argParser.parse()

# Disable warnings by default, enable them below if we're not being quiet
client.eventLoop.showNonfatalExceptions = 0

if not client.cmdLineValues['silent']:
    if not client.cmdLineValues['quiet']:
        # Show connection events
        client.onConnect.trace("--- Connected to %s" % client.options['server'])
        client.onStartWorldDownload.trace("--- Downloading world...")
        client.onEnterGame.trace("--- Entered the game.")
        client.game.world.onLoad.trace("--- World loaded.")
        client.game.onAddPlayer.trace("--> Joined : %(2)s")
        client.game.onRemovePlayer.trace("<-- Parted : %(2)s")

        # Re-enable nonfatal exceptions
        client.eventLoop.showNonfatalExceptions = 1

    # Show incoming messages
    def message(msg):
        sender = msg.fromId
        if type(sender) == int:
            # This is from a player, look up the callsign
            sender = "<%s>" % client.game.players[sender].identity.callSign
        else:
            # This is from a special 'player', like the server
            sender = "[%s]" % sender
        print "%s %s" % (sender, msg.message)
    client.onMsgMessage.observe(message)

# A hack to prevent waiting in the main loop too long after client.eventLoop.stop()
client.eventLoop.pollTime = 0.5

# Thread to read keyboard input and send messages.
# It would generally be better to do this by making stdin nonblocking,
# but there's no portable way to do this in pyhton. Threads are slightly
# more portable :)
class ChatThread(Thread):
    def run(self):
        while client.eventLoop.running:
            line = sys.stdin.readline()
            if not line:
                # End of file
                time.sleep(float(client.cmdLineValues['wait']))
                client.eventLoop.stop()
                return
            # Strip off the newline and all other trailing space
            client.sendMessage(line.rstrip())

cthread = ChatThread()
client.onEnterGame.observe(cthread.start)

ui = ui.Setup(client.game, client.eventLoop)
client.run()

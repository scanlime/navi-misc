# -*- mode: python; -*-
#
# This is a .tac configuration file for the bot server, a separate
# daemon that maintains connections to IRC servers according to a
# list of requests.
#
# Start the server from the 'cia' directory by running:
#   twistd -oy conf/bot_server.tac -l bot_server.log --pidfile=bot_server.pid
#

from twisted.application import service, internet
from twisted.manhole.telnet import ShellFactory
from twisted.spread import pb
from LibCIA.IRC import Bots

application = service.Application("bot_server")

#
# The BotNetwork is the root object responsible for managing our
# collection of IRC bots. It is constructed with a NickAllocator
# object that specifies how it generates IRC nicks. Other styles
# of nick generation are available.
#
botNet = Bots.BotNetwork(Bots.SequentialNickAllocator("CIA-"))

# The bot server listens on a UNIX socket rather than TCP/IP, for security
botSocketName = "bots.socket"
internet.UNIXServer(botSocketName, pb.PBServerFactory(botNet)).setServiceParent(application)

# For maintaining the bot server without restarting, if necessary, run
# a twisted.manhole telnet console. We only start the server if a password
# has been provided in the "bots.passwd" file.
try:
    passwd = open("bots.passwd").read().strip()
except IOError:
    pass
else:
    console = ShellFactory()
    console.password = passwd
    console.namespace['botNet'] = botNet
    internet.TCPServer(2230, console).setServiceParent(application)

### The End ###

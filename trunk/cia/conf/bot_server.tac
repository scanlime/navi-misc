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
from LibCIA import Security
import os

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
#os.chmod(botSocketName, 0600)

# For maintaining the bot server without restarting, if necessary, run
# a twisted.manhole telnet console. The password is randomly generated
# and stored in the current directory.
consolePasswdFile = "bots.passwd"
console = ShellFactory()
console.password = Security.createRandomKey()
console.namespace['botNet'] = botNet
open(consolePasswdFile, "w").write(console.password + "\n")
internet.TCPServer(2230, console).setServiceParent(application)

### The End ###

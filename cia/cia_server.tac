#
# This is a .tac configuration file that sets up a CIA server.
# Start the server by running 'twistd -y' on this file.
#

from twisted.application import service, internet
from twisted.web import server
from LibCIA import Message, XMLRPC, IRC

application = service.Application("cia_server")

# The Hub is the central object responsible for delivering
# messages between components of CIA
hub = Message.Hub()

# The XMLRPC.SimpleCIAInterface is a simple XML-RPC interface
# to CIA, used for initially delivering messages
f = server.Site(XMLRPC.SimpleCIAInterface(hub))
internet.TCPServer(3910, f).setServiceParent(application)

botNet = IRC.BotNetwork()
srv = ('irc.freenode.net', 6667)
botNet.addChannel(srv, '#botpark')
def f(msg):
    botNet.msg(srv, '#botpark', str(msg.xml.source.project))
hub.addClient(f)


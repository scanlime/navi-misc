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

# Control a network of IRC bots using messages sent through the Hub
IRC.HubListener(hub)

# The XMLRPC.SimpleCIAInterface is a simple XML-RPC interface
# to CIA, used for initially delivering messages
f = server.Site(XMLRPC.SimpleCIAInterface(hub))
internet.TCPServer(3910, f).setServiceParent(application)

#
# This is a .tac configuration file that sets up a CIA server.
# Start the server by running 'twistd -y' on this file.
#

from twisted.application import service, internet
from twisted.web import server
from LibCIA import Message, XMLRPC

application = service.Application("cia_server")

# The Hub is the central object responsible for delivering
# messages between components of CIA
hub = Message.Hub()

# The XMLRPC.SimpleCIAInterface is a simple XML-RPC interface
# to CIA, used for initially delivering messages
f = server.Site(XMLRPC.SimpleCIAInterface(hub))
internet.TCPServer(3910, f).setServiceParent(application)

# For debugging, add a client to the hub that dumps all
# messages received through it to stdout
def f(msg):
    print msg
hub.addClient(f)


# -*- mode: python; -*-
#
# This is a .tac configuration file that sets up a minimal
# CIA server. It includes only what's necessary for delivering
# commits over IRC. The HTTP server is used only for RPC- there's
# no web interface.
#
# This stores all rulesets in a flat XML file and keeps no stats,
# so it doesn't need or utilize an SQL database.
#
# Start the server by running 'twistd -oy' on this file.
#

from twisted.application import service, internet
from twisted.web import error
from twisted.internet import ssl
from LibCIA import Message, Ruleset, IRC, IncomingMail
from LibCIA import Debug, Security, RpcServer, RpcClient, Web


application = service.Application("cia_server")
hub = Message.Hub()

# Connect to IRC bots running in a separate process
remoteBots = IRC.Handler.RemoteBots("bots.socket")

# Include URI handlers for IRC, and for relaying messages over XML-RPC
uriRegistry = Ruleset.URIRegistry(
    IRC.Handler.IrcURIHandler(remoteBots),
    RpcClient.XmlrpcURIHandler(),
    )

# Use a persistent set of rulesets to filter and format messages.
# We keep the rulesets in a flat XML file, updating them according
# to requests from the ruleset editor.
rulesetStorage = Ruleset.FileRulesetStorage(hub, uriRegistry, "conf/rulesets.xml")

# Give the default user a 'universe' capability and save its key,
# so it can be used later by the administrative tools. This effectively
# bootstraps our security system by creating a powerful user.
Security.User().saveKey('~/.cia_key', 'universe')

# Our XML-RPC interface is at /RPC2, just stick an error page at our
# web site's root. You might want to replace this with a Web.Server.File()
# instance pointing to a real web page.
webRoot = error.ErrorPage(404, "Nothing here", "This CIA server has no web interface")
site = Web.Server.Site(webRoot)

# Create a root XML-RPC object, with interfaces attached for each subsystem
rpc = RpcServer.getRootInterface()
rpc.putSubHandler('hub', Message.HubInterface(hub))
rpc.putSubHandler('mail', IncomingMail.MailInterface(hub))
rpc.putSubHandler('ruleset', Ruleset.RulesetInterface(rulesetStorage))
rpc.putSubHandler('security', Security.SecurityInterface())
rpc.putSubHandler('debug', Debug.DebugInterface())
webRoot.putChild('RPC2', rpc)

# Now create an HTTP server for our XML-RPC interfaces.
# If you want HTTPS also, copy the relevant code form cia_full.tac
internet.TCPServer(3910, site).setServiceParent(application)

### The End ###

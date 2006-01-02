# -*- mode: python; -*-
#
# This is a .tac configuration file that sets up a CIA server.
# Start the server by running 'twistd -oy' on this file.
#

from twisted.application import service, internet
from twisted.web import vhost
from LibCIA import Message, Ruleset, IRC, Stats, IncomingMail
from LibCIA import Debug, Security, RpcServer, RpcClient, Web

application = service.Application("cia_server")
hub = Message.Hub()

# A network of IRC bots used to handle irc:// URIs
botNet = IRC.Bots.BotNetwork(IRC.Bots.SequentialNickAllocator("CIA-"))

# Set up periodic maintenance of our stats database
Stats.Maintenance().run()

# A list of URI handlers that can be used as targets for rulesets
uriRegistry = Ruleset.URIRegistry(
    IRC.Handler.IrcURIHandler(botNet),
    Stats.StatsURIHandler(),
    RpcClient.XmlrpcURIHandler(),
    )

# Use a persistent set of rulesets to filter and format messages
rulesetStorage = Ruleset.RulesetStorage(hub, uriRegistry)

# Save the 'universe' capability key so it can be used later by the administrative tools
Security.caps.saveKey('universe', '~/.cia_key')

# Our front page renders the documentation root, but unless otherwise specified
# we load other pages form the 'htdocs' directory, as static files.
doc = Web.Doc.Component('doc')
webRoot = Web.Server.StaticJoiner('htdocs', doc.resource)
site = Web.Server.Site(webRoot)

# Add a VHostMonster we can use to safely proxy requests from Apache running on a different port
webRoot.putChild('vhost', vhost.VHostMonsterResource())

# Create a root XML-RPC object, with interfaces attached for each subsystem
rpc = RpcServer.Interface()
rpc.putSubHandler('hub', Message.HubInterface(hub))
rpc.putSubHandler('mail', IncomingMail.MailInterface(hub))
rpc.putSubHandler('ruleset', Ruleset.RulesetInterface(rulesetStorage))
rpc.putSubHandler('stats', Stats.StatsInterface())
rpc.putSubHandler('security', Security.SecurityInterface())
rpc.putSubHandler('debug', Debug.DebugInterface())
webRoot.putChild('RPC2', rpc)

# The user-navigable areas of our site are all Component instances
site.putComponent('stats', Web.Stats.Component())
site.putComponent('doc', doc)
site.putComponent('irc', Web.BotStatus.Component(botNet))
site.putComponent('rulesets', Web.RulesetBrowser.Component(rulesetStorage))
site.putComponent('info', Web.Info.Component())

# Now create an HTTP server holding both our XML-RPC and web interfaces
internet.TCPServer(3910, site).setServiceParent(application)
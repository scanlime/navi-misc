# -*- mode: python; -*-
#
# This is a .tac configuration file that sets up a CIA server.
# Start the server by running 'twistd -oy' on this file.
#

from twisted.application import service, internet
from twisted.web import static, vhost
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

# Create the web interface. We start with all the static
# files in 'htdocs' and add dynamic content from there.
webRoot = static.File("htdocs")
webRoot.putChild('rulesets', Web.RulesetBrowser.RulesetList(rulesetStorage))
webRoot.putChild('stats', Web.Stats.Browser.Page())
webRoot.putChild('info', Web.Info.Page())
webRoot.putChild('irc', Web.BotStatus.IRCBotPage(botNet))
webRoot.putChild('foo', Web.Doc.Page('foo.txt'))

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

# Now create an HTTP server holding both our XML-RPC and web interfaces
internet.TCPServer(3910, Web.Server.Site(webRoot)).setServiceParent(application)

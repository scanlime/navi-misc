#
# This is a .tac configuration file that sets up a CIA server.
# Start the server by running 'twistd -oy' on this file.
#

from twisted.application import service, internet
from twisted.web import server, xmlrpc, static, vhost
from LibCIA import Message, Ruleset, IRC, Stats
from LibCIA import IncomingMail, Interface, Security

application = service.Application("cia_server")
hub = Message.Hub()

# A place to store stats, written to by the StatsURIHandler and queried
# by the StatsInterface over XML-RPC
statsStorage = Stats.StatsStorage('data/stats')

# A list of URI handlers that can be used as targets for rulesets
uriRegistry = Ruleset.URIRegistry(
    IRC.IrcURIHandler(IRC.BotNetwork("CIA-%d")),
    Stats.StatsURIHandler(statsStorage),
    )

# Use a persistent set of rulesets to filter and format messages
rulesetStorage = Ruleset.RulesetStorage("data/rulesets.xml", hub, uriRegistry)

# Create the web interface. We start with all the static
# files in 'htdocs' and add dynamic content from there.
webRoot = static.File("htdocs")

# Add VHostMonster support, so we can have CIA serve web requests
# behind our main Apache web server without getting horribly confused.
webRoot.putChild('vhost', vhost.VHostMonsterResource())

# Create the capabilities database used to add some security to the XML-RPC interfaces
capDb = Security.CapabilityDB("data/security.db")

# Create a root XML-RPC object, with interfaces attached for each subsystem
rpc = xmlrpc.XMLRPC()
rpc.putSubHandler('sys', Interface.SysInterface(capDb))
rpc.putSubHandler('hub', Message.HubInterface(hub))
rpc.putSubHandler('ruleset', Ruleset.RulesetInterface(rulesetStorage))
rpc.putSubHandler('mail', IncomingMail.MailInterface(hub))
rpc.putSubHandler('stats', Stats.StatsInterface(statsStorage))
webRoot.putChild('RPC2', rpc)

# Now create an HTTP server holding both our XML-RPC and web interfaces
internet.TCPServer(3910, server.Site(webRoot)).setServiceParent(application)

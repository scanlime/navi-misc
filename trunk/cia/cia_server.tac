#
# This is a .tac configuration file that sets up a CIA server.
# Start the server by running 'twistd -oy' on this file.
#

from twisted.application import service, internet
from twisted.web import server, xmlrpc, static
from LibCIA import Message, Ruleset, IRC, Stats
from LibCIA import IncomingMail, Interface, Security
from LibCIA.Web import StatsBrowser, RulesetBrowser

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

# Create the security module's capabilities database and save the 'universe'
# capability's key so it can be used to retrieve additional keys later.
caps = Security.CapabilityDB("data/security.db")
caps.saveKey('universe', 'data/universe.key')

# Create the web interface. We start with all the static
# files in 'htdocs' and add dynamic content from there.
webRoot = static.File("htdocs")
webRoot.putChild('rulesets', RulesetBrowser.RulesetPage(caps=caps, storage=rulesetStorage))
webRoot.putChild('stats', StatsBrowser.StatsPage(caps=caps, storage=statsStorage))

# Create a root XML-RPC object, with interfaces attached for each subsystem
rpc = xmlrpc.XMLRPC()
rpc.putSubHandler('sys', Interface.SysInterface(caps))
rpc.putSubHandler('hub', Message.HubInterface(caps, hub))
rpc.putSubHandler('ruleset', Ruleset.RulesetInterface(caps, rulesetStorage))
rpc.putSubHandler('mail', IncomingMail.MailInterface(caps, hub))
rpc.putSubHandler('stats', Stats.StatsInterface(caps, statsStorage))
rpc.putSubHandler('security', Security.SecurityInterface(caps))
webRoot.putChild('RPC2', rpc)

# Now create an HTTP server holding both our XML-RPC and web interfaces
internet.TCPServer(3910, server.Site(webRoot)).setServiceParent(application)

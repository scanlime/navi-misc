#
# This is a .tac configuration file that sets up a CIA server.
# Start the server by running 'twistd -oy' on this file.
#

from twisted.application import service, internet
from twisted.web import server, xmlrpc
from twisted.persisted import sob
from LibCIA import Message, Ruleset, IRC, Stats, IncomingMail, Interface

application = service.Application("cia_server")

# The central point all messages pass through
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

# Add XML-RPC interfaces for the components that need them
root = xmlrpc.XMLRPC()
root.putSubHandler('sys', Interface.SysInterface())
root.putSubHandler('hub', Message.HubInterface(hub))
root.putSubHandler('ruleset', Ruleset.RulesetInterface(rulesetStorage))
root.putSubHandler('mail', IncomingMail.MailInterface(hub))
root.putSubHandler('stats', Stats.StatsInterface(statsStorage))
internet.TCPServer(3910, server.Site(root)).setServiceParent(application)

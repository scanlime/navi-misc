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

# A list of URI handlers that can be used as targets for rulesets
uriRegistry = Ruleset.URIRegistry([
    IRC.IrcURIHandler(hub, IRC.BotNetwork("CIA-%d")),
    Stats.StatsURIHandler(hub, 'data/stats'),
    ])

# Use a persistent set of rulesets to filter and format messages
storage = Ruleset.RulesetStorage("data/rulesets.xml", hub, uriRegistry)
Ruleset.RulesetController(hub, storage)

# Add XML-RPC interfaces for the components that need them
root = xmlrpc.XMLRPC()
root.putSubHandler('hub', Message.HubInterface(hub))
root.putSubHandler('mail', IncomingMail.MailInterface(hub))
root.putSubHandler('sys', Interface.SysInterface())
internet.TCPServer(3910, server.Site(root)).setServiceParent(application)

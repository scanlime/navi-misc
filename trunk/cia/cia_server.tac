#
# This is a .tac configuration file that sets up a CIA server.
# Start the server by running 'twistd -y' on this file.
#

from twisted.application import service, internet
from twisted.web import server
from LibCIA import Message, XMLRPC, Ruleset, IRC

application = service.Application("cia_server")
hub = Message.Hub()

uriRegistry = Ruleset.URIRegistry([
    IRC.URIHandler(IRC.BotNetwork("CIA-%d")),
    ])

storage = Ruleset.RulesetStorage("data/rulesets.xml", hub, uriRegistry)
Ruleset.RulesetController(hub, storage)

# Add an XML-RPC interface for delivering messages
f = server.Site(XMLRPC.Interface(hub))
internet.TCPServer(3910, f).setServiceParent(application)

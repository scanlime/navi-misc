# -*- mode: python; -*-
#
# This is a .tac configuration file that sets up a full CIA
# server with all the bells and whistles. It requires a MySQL
# database for rulesets and stats, and provides a full web
# interface.
#
# Start the server by running 'twistd -oy' on this file.
#

from twisted.application import service, internet
from twisted.web import vhost
from LibCIA import Database, Message, Ruleset, IRC, Stats, IncomingMail, Cron
from LibCIA import Debug, Security, RpcServer, RpcClient, Web, Cache
from twisted.internet import tcp
from Nouvelle import tag

Database.init()

# Disable stats pruning automatically if our I/O load gets critical
def isPruningEnabled(self):
    # FIXME: pruning disabled completely, for now
    return False
#    ioStatus = dict([t.split("=") for t in open("/proc/io_status").read().split()])
#    return int(ioStatus['io_tokens']) > 300000
Stats.Target.Messages.isPruningEnabled = isPruningEnabled

# Remove the non-main CIA server notice, since this is in fact the main server.
Web.Template.Page.site_mainServerNotice = []

application = service.Application("cia_server")
hub = Message.Hub()

# Connect to IRC bots running in a separate process
remoteBots = IRC.Handler.RemoteBots("bots.socket")

# Set up periodic maintenance tasks
Cron.Scheduler(
    Cron.Event(Cron.hourly, Stats.Target.Maintenance().run, "stats maintenance"),
    Cron.Event(Cron.hourly, Cache.Maintenance().run, "cache maintenance"),
    )

# A list of URI handlers that can be used as targets for rulesets
uriRegistry = Ruleset.URIRegistry(
    IRC.Handler.IrcURIHandler(remoteBots),
    Stats.Handler.StatsURIHandler(),
    RpcClient.XmlrpcURIHandler(),
    )

# Use a persistent set of rulesets to filter and format messages.
# We keep the rulesets in a database table, updating them according
# to requests from the ruleset editor.
rulesetStorage = Ruleset.DatabaseRulesetStorage(hub, uriRegistry)

# Give the default user a 'universe' capability and save its key,
# so it can be used later by the administrative tools. This effectively
# bootstraps our security system by creating a powerful user.
Security.User().saveKey('~/.cia_key', 'universe')

# Create components we'll need in multiple places later
doc   = Web.Doc.Component('doc')
stats = Web.Stats.Component()

# Present an overview page at the top of our web site, using the documentation
# system's default sidebar to give us an easy way to navigate the site.
frontPage = Web.Overview.OverviewPage(doc.resource, stats)

# Our front page gives us an overview of CIA, but unless otherwise specified
# we load other pages from the 'htdocs' directory, as static files.
webRoot = Web.Server.StaticJoiner('htdocs', frontPage)
site = Web.Server.Site(webRoot)

# Add a VHostMonster we can use to safely proxy requests from Apache running on a different port
webRoot.putChild('vhost', vhost.VHostMonsterResource())

# Experimental graphs, currently disabled due to extreme CPU-hoggyness
#webRoot.putChild('graph', Web.Stats.Graph.GraphPage(
#    Stats.Graph.PrefixSelector('project/', color='#FF0000', shape='box'),
#    Stats.Graph.PrefixSelector('author/', color='#0000FF'),
#    ))

# Create a root XML-RPC object, with interfaces attached for each subsystem
rpc = RpcServer.getRootInterface()
rpc.putSubHandler('hub', Message.HubInterface(hub))
rpc.putSubHandler('mail', IncomingMail.MailInterface(hub))
rpc.putSubHandler('ruleset', Ruleset.RulesetInterface(rulesetStorage))
rpc.putSubHandler('stats', Stats.Interface.StatsInterface())
rpc.putSubHandler('security', Security.SecurityInterface())
rpc.putSubHandler('debug', Debug.DebugInterface())
webRoot.putChild('RPC2', rpc)

# The user-navigable areas of our site are all Component instances
site.putComponent('stats', stats)
site.putComponent('doc', doc)
site.putComponent('irc', Web.BotStatus.Component(remoteBots))
site.putComponent('rulesets', Web.RulesetBrowser.Component(rulesetStorage))
site.putComponent('info', Web.Info.Component())

# Now create an HTTP server holding both our XML-RPC and web interfaces
internet.TCPServer(3910, site).setServiceParent(application)

# We don't start our own secure server, apache is running https also
# and proxying that locally to our HTTP server. We do however need to
# tell the keyring module that it's running, and on the default port.
Web.Keyring.setSecureServer()

### The End ###
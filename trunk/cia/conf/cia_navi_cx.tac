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
from Nouvelle import tag, xml

Database.init()

# Trying out Google Adsense a bit.. currently this is in a very out-of-the
# way location. It would be better to keep the ads on the right side of the
# page, so that they're visible but unobtrusive- but our content is awful
# wide for that right now.
adSense = xml('<p style="text-align: center"><script type="text/javascript"><!--\n'
              'google_ad_client = "pub-6191259594996825";'
              'google_ad_width = 120;'
              'google_ad_height = 600;'
              'google_ad_format = "120x600_as";'
              'google_ad_type = "text";'
              'google_ad_channel ="";'
              'google_color_border = "EEF4EB";'
	      'google_color_bg = "EEF4EB";'
	      'google_color_link = "0000CC";'
	      'google_color_url = "008000";'
	      'google_color_text = "000000";\n'
	      '//--></script>'
              '<script type="text/javascript"'
              ' src="http://pagead2.googlesyndication.com/pagead/show_ads.js">'
              '</script></p>')
Web.Template.Page.site_belowLeftColumn.append(adSense)

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
#site.putComponent('info', Web.Info.Component())

# Now create an HTTP server holding both our XML-RPC and web interfaces
internet.TCPServer(3910, site, interface='localhost').setServiceParent(application)

# We don't start our own secure server, apache is running https also
# and proxying that locally to our HTTP server. We do however need to
# tell the keyring module that it's running, and on the default port.
Web.Keyring.setSecureServer()

### The End ###

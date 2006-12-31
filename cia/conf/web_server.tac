# -*- mode: python; -*-
#
# Web-server-only configuration. This does not answer RPC
# requests, and it has no message delivery capabilities.
#
# Start the server from the 'cia' directory by running:
#   twistd -oy conf/web_server.tac -l web_server.log --pidfile=web_server.pid
#
# This configuration is currently experimental, don't use it yet!
#

from twisted.application import service, internet
from LibCIA import Database, Message, Web
from twisted.internet import tcp

Database.init()

application = service.Application("web_server")

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

# The user-navigable areas of our site are all Component instances
site.putComponent('stats', stats)
site.putComponent('doc', doc)
#site.putComponent('irc', Web.BotStatus.Component(remoteBots))
#site.putComponent('rulesets', Web.RulesetBrowser.Component(rulesetStorage))
#site.putComponent('info', Web.Info.Component())

# Now create an HTTP server holding both our XML-RPC and web interfaces
internet.TCPServer(3911, site).setServiceParent(application)

# We don't start our own secure server, apache is running https also
# and proxying that locally to our HTTP server. We do however need to
# tell the keyring module that it's running, and on the default port.
Web.Keyring.setSecureServer()

### The End ###

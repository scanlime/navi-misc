# -*- mode: python; -*-
#
# Web-server-only configuration. This does not answer RPC
# requests, and it has no message delivery capabilities.
# This server is read-only, and any number of instances
# may be run concurrently.
#
# This is the configuration for the official CIA host.
# It can be started via http-cluster.sh.
#

import os
port = int(os.getenv("PORT"))

from twisted.application import service, internet
from LibCIA import Database, Message, Web, RpcServer, Stats
from twisted.internet import tcp

Database.init()

# Remove the non-main CIA server notice, since this is in fact the main server.
Web.Template.Page.site_mainServerNotice = []

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

# We still need to install RPC components that are accessed locally via
# RpcServer.getRootInterface(), even though we don't expose them over
# HTTP in this process.
rpc = RpcServer.getRootInterface()
rpc.putSubHandler('stats', Stats.Interface.StatsInterface())

# The user-navigable areas of our site are all Component instances
site.putComponent('stats', stats)
site.putComponent('doc', doc)

# External components, implemented in DJango
site.putComponent('account', Web.Server.Component("Your Account"))

# These components don't work across the Web/RPC split yet
#site.putComponent('irc', Web.BotStatus.Component(remoteBots))
#site.putComponent('rulesets', Web.RulesetBrowser.Component(rulesetStorage))
#site.putComponent('info', Web.Info.Component())

# Run the HTTP server
internet.TCPServer(port, site, interface='localhost').setServiceParent(application)

### The End ###

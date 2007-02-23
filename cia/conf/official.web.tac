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
from Nouvelle import xml
from twisted.internet import tcp

Database.init()

# Add the Google Analytics integration
Web.Template.Page.site_bottomOfFooter = [xml("""
  <script src="http://www.google-analytics.com/urchin.js" type="text/javascript">
  </script>
  <script type="text/javascript">
  _uacct = "UA-247340-1";
  urchinTracker();
  </script>
""")]

application = service.Application("web_server")

# Our front page gives us an overview of CIA, but unless otherwise specified
# we load other pages from the 'htdocs' directory, as static files.
webRoot = Web.Server.StaticJoiner('htdocs', Web.Overview.OverviewPage())
site = Web.Server.Site(webRoot)

# We still need to install RPC components that are accessed locally via
# RpcServer.getRootInterface(), even though we don't expose them over
# HTTP in this process.
rpc = RpcServer.getRootInterface()
rpc.putSubHandler('stats', Stats.Interface.StatsInterface())

# The user-navigable areas of our site are all Component instances
site.putComponent('stats', Web.Stats.Component())

# External components, implemented in Django
site.putComponent('doc', Web.Server.Component("Documentation"))
site.putComponent('account', Web.Server.Component("Your Account"))

# Run the HTTP server
internet.TCPServer(port, site, interface='localhost').setServiceParent(application)

### The End ###

#!/usr/bin/env python
#
# Just about the simplest possible example of using Nouvelle with twisted.web.
# This runs a web server at http://localhost:8080
#

from twisted.web import server
from twisted.internet import reactor
from Nouvelle import tag, Twisted

class Hello(Twisted.Page):
    isLeaf = 1
    document = tag('html')[
                   tag('head') [
                       tag('title')[ "Hi" ],
                   ],
                   tag('body') [
                       tag('h3')[ "Hello World!" ],
                   ],
               ]

if __name__ == "__main__":
    root = Hello()
    site = server.Site(root)
    reactor.listenTCP(8080, site)
    reactor.run()

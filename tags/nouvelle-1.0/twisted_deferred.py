#!/usr/bin/env python
#
# An example of asynchronous rendering using Deferred.
# This simulates rendering a page that generates dynamic content asynchronously.
# The 'greeting' placeholder generates data via a callback that's executed after 5 seconds.
#
# This runs a web server at http://localhost:8080
#

from twisted.web import server
from twisted.internet import reactor, defer
from Nouvelle import tag, place, Twisted

class Hello(Twisted.Page):
    isLeaf = 1
    document = tag('html')[
                   tag('head') [
                       tag('title')[ "Hi" ],
                   ],
                   tag('body') [
                       tag('h3')[ place('greeting') ],
                   ],
               ]

    def render_greeting(self, context):
        d = defer.Deferred()
        print "Starting to render the greeting..."
        reactor.callLater(5, self.greetingCallback, d)
        return d

    def greetingCallback(self, d):
        # This is called 5 seconds after the page was requested.
        # We call the deferred's callback with our finished greeting-
        # this completes the web page and it can be sent to the client
        print "Finishing"
        d.callback("Hello World")

if __name__ == "__main__":
    root = Hello()
    site = server.Site(root)
    reactor.listenTCP(8080, site)
    reactor.run()

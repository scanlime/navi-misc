#!/usr/bin/env python
#
# A proof-of-concept demo for the RSS 2.0 <cloud> tag
# support in the CIA server. This subscribes to the RSS
# feed holding all commit messages, printing new messages
# to stdout as they arrive.
#
# This requires that at least one port, specified below,
# can accept inbound connections at the same IP address
# this machine appears at to the rest of the internet.
# In short, you'll be fine if you're not using NAT or a
# firewall, but if you are you'll have to pick a port to
# let through and set it below.
#
# --Micah Dowty <micah@navi.cx>
#

import urllib2, xmlrpclib
import sys
from xml.dom import minidom
from SimpleXMLRPCServer import SimpleXMLRPCServer


class config:
    # We need an RSS 2.0 feed to get the <cloud> element. Ask for plain text
    # because stdout probably isn't a web browser. We only want one item,
    # since we should be notified every time a new message comes in.
    url = "http://cia.vc/stats/total/commits/.rss?ver=2&medium=plaintext&limit=1"

    # This is the port we'll listen on, expecting a connection back from the CIA server
    port = 23692


class Feed:
    """Represents an RSS 2.0 feed downloaded from a given URL. This isn't the place for
       a full RSS 2.0 implementation, but it supports extracting items and creating
       subscriptions.
       """
    def __init__(self, url):
        self.url = url

    def grab(self):
        """Download the feed and parse it as XML, saving the DOM tree"""
        self.dom = minidom.parse(urllib2.urlopen(self.url))
        self.rss = self.dom.getElementsByTagName('rss')[0]
        self.channel = self.rss.getElementsByTagName('channel')[0]

    def items(self):
        """Return a list of FeedItem instances"""
        return map(FeedItem, self.channel.getElementsByTagName('item'))

    def subscribe(self, procedureName, clientPort, responderPath):
        """Subscribe to this feed. When the feed is modified, the given
           XML-RPC procedure will be invoked.
           """
        cloud = self.channel.getElementsByTagName('cloud')[0]

        # Assemble the bits from the <cloud> tag back into a URL we can get to the server at
        s = xmlrpclib.ServerProxy("http://%s:%s%s" % (cloud.getAttribute('domain'),
                                                      cloud.getAttribute('port'),
                                                      cloud.getAttribute('path')))
        regProcedure = getattr(s, cloud.getAttribute('registerProcedure'))

        # Call the registration procedure with enough information for the server to call us back
        regProcedure(procedureName, clientPort, responderPath, 'xml-rpc', [self.url])

    def publisher(self):
        """Return a FeedPublisher instance for this feed"""
        return FeedPublisher(self.channel.getElementsByTagName('cloud')[0])


def getText(node):
    """Get the text contained within the given DOM node recursively"""
    content = ""
    for node in node.childNodes:
        if node.nodeType == node.TEXT_NODE:
            content += node.data
        elif node.nodeType == node.ELEMENT_NODE:
            content += getText(node)
    return content


class FeedItem:
    """One item in an RSS 2.0 feed, constructed with the item's DOM tree.
       The string representation of this object is the description,
       stripped of all HTML.
       """
    def __init__(self, dom):
        self.dom = dom

    def __str__(self):
        # The description is actually XHTML, so run it through the XML parser again
        html = getText(self.dom.getElementsByTagName('description')[0])
        dom = minidom.parseString("<html>%s</html>" % html)
        return getText(dom.getElementsByTagName('html')[0]).strip()


class Subscriber(SimpleXMLRPCServer):
    """An XML-RPC server providing a function that will get called by the CIA
       server to get notifications for resources we've subscribed to.
       """
    def __init__(self, port):
        self.port = port
        SimpleXMLRPCServer.__init__(self, ('', port), logRequests=False)
        self.urlToCallbackMap = {}
        self.register_function(self.notify, 'notify')

    def notify(self, url):
        """This function is called by the CIA server when a resource is modified.
           We get the URL of the modified resource, and look up the proper callback.
           """
        self.urlToCallbackMap[url]()
        return True

    def subscribe(self, feed, callback):
        """Subscribe to the provided feed, causing the given callback to be called when it changes"""
        self.urlToCallbackMap[feed.url] = callback
        feed.subscribe('notify', self.port, '/RPC2')


def main(c):
    """Main program, print new items to stdout as they appear, using the supplied configuration"""
    feed = Feed(c.url)
    print "Retrieving feed..."
    feed.grab()

    def feedChanged():
        print
        feed.grab()
        print feed.items()[0]

    subscriber = Subscriber(c.port)
    print "Subscribing..."
    subscriber.subscribe(feed, feedChanged)
    print "Waiting for notifications..."
    subscriber.serve_forever()


if __name__ == '__main__':
    if len(sys.argv) > 1:
        config.url = sys.argv[1]
    if len(sys.argv) > 2:
        config.port = int(sys.argv[2])
    main(config)

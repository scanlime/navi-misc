""" LibCIA.XMLRPC

A simple XMLRPC interface used for delivering messages and other
simple unidirectional activities that should be easy to access from
any language.

A more complex interface supporting the addition of new filters
and callbacks is available though the Perspective Broker interface
to CIA, but that is only convenient to use from Python whereas this
XML-RPC interface is meant to be universal.
"""
#
# CIA open source notification system
# Copyright (C) 2003 Micah Dowty <micahjd@users.sourceforge.net>
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

from twisted.web import xmlrpc
from Message import Message
from IncomingMail import IncomingMailParser
import sys


class SimpleCIAInterface(xmlrpc.XMLRPC):
    """A simple interface to CIA over XML-RPC.
       Must be constructed with a reference to a Message.Hub
       """
    def __init__(self, hub):
        self.hub = hub

    def xmlrpc_deliverMessage(self, xml):
        """Deliver the given message, provided as XML text"""
        try:
            self.hub.deliver(Message(xml))
        except:
            e = sys.exc_info()[1]
            return xmlrpc.Fault(e.__class__.__name__, str(e))
        return True

    def xmlrpc_processEmail(self, message):
        """Given the raw text of an email message, log it and process it if applicable"""
        xmlMessage = IncomingMailParser().parseString(message)
        if xmlMessage:
            self.hub.deliver(xmlMessage)
        return True


if __name__ == "__main__":
    # A simple test that just creates a hub and starts an XML-RPC server
    from twisted.internet import reactor
    from twisted.web import server
    from Message import Hub
    hub = Hub()
    def f(msg):
        print msg
    hub.addClient(f)
    r = SimpleCIAInterface(hub)
    reactor.listenTCP(3910, server.Site(r))
    reactor.run()

### The End ###

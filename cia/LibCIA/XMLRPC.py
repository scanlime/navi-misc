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
from twisted.python.rebuild import rebuild
from IncomingMail import IncomingMailParser
import sys


class SimpleCIAInterface(xmlrpc.XMLRPC):
    """A simple interface to CIA over XML-RPC.
       Must be constructed with a reference to a Message.Hub
       """
    def __init__(self, hub):
        self.hub = hub

    def xmlrpc_deliverMessage(self, xml):
        """Deliver the given message, provided as XML text.
           If the message generates a reply, returns that.
           Otherwise, returns True.
           """
        try:
            result = self.hub.deliver(Message(xml))
        except:
            e = sys.exc_info()[1]
            return xmlrpc.Fault(e.__class__.__name__, str(e))
        result = self.hub.deliver(Message(xml))
        if result is not None:
            return result
        return True

    def xmlrpc_processEmail(self, message):
        """Given the raw text of an email message, log it and process it if applicable"""
        xmlMessage = IncomingMailParser().parseString(message)
        if xmlMessage:
            self.hub.deliver(xmlMessage)
        return True

    def xmlrpc_rebuild(self):
        """Use twisted.python.rebuild to reload all applicable modules"""
        # Rebuild our package to make sure we have the latest module list
        import LibCIA
        rebuild(LibCIA)

        # Always rebuild this module, so in case the code below fails
        # we can edit this function to fix it without restarting :)
        import XMLRPC
        rebuild(XMLRPC)

        # Now rebuild all loaded modules inside the LibCIA package
        for item in LibCIA.__dict__.itervalues():
            if type(item) == type(LibCIA):
                rebuild(item)
        return True

### The End ###

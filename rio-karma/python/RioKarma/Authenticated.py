""" RioKarma.Authenticated

This module implements the authentication sequence for the Pearl protocol.
It must be separate from the Pearl module, since this relies on both
it and the requests themselves, whereas Pearl itself must stay independent
of the Request module:

 +--------------+
 |     Auth     |
 |   +----------+
 |   | Requests |
 +---+----------+
 |     Pearl    |
 +--------------+

"""
#
# Copyright (C) 2005 Micah Dowty <micah@navi.cx>
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

from RioKarma import Pearl, Request
from twisted.internet import protocol, defer


class Protocol(Pearl.Protocol):
    """This builds on the Pearl.Protocol to include authentication,
       initiated when the protocol connects successfully. This reports
       authentication status via the factory's 'authResult' deferred, returning
       a reference to this Protocol on success.
       """
    def connectionMade(self):
        self.sendRequest(Request.GetAuthenticationSalt()).addCallback(
            self._sendAuthDigest).addErrback(self.factory.authResult.errback)

    def _sendAuthDigest(self, salt):
        self.sendRequest(Request.Authenticate(salt, self.factory.password)).addCallback(
            self._reportAuthResult).addErrback(self.factory.authResult.errback)

    def _reportAuthResult(self, rights):
        self.authRights = rights
        self.factory.authResult.callback(self)


class Factory(protocol.ClientFactory):
    """This factory includes a Deferred notifying us of the connection/auth
       status, and provides a way to send our password to AuthenticatedProtocol.
       """
    protocol = Protocol

    def __init__(self, password):
        self.password = password
        self.authResult = defer.Deferred()

    def clientConnectionFailed(self, connector, reason):
        """Pass this on to our Deferred"""
        self.authResult.errback(reason)

### The End ###

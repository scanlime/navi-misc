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

import getpass, sqlite
from RioKarma import Pearl, Request, Paths
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


class Keychain:
    """The Keychain object looks up passwords corresponding to a device's
       unique identifier. If a password isn't found it fails, but subclasses
       can override this to prompt for a password somehow.
       """
    schema = """
    CREATE TABLE auth
    (
        id         VARCHAR(255) PRIMARY KEY,
        password   VARCHAR(255) NOT NULL
    );
    """

    connection = None

    def open(self):
        """Open the cache, creating it if necessary"""
        if self.connection is not None:
            return

        self.connection = sqlite.connect(Paths.getCache("auth"))
        self.cursor = self.connection.cursor()

        # If the database hasn't been initialized yet, we'll get
        # an error about the table not existing. Fix this by
        # loading the schema.
        try:
            self.cursor.execute("SELECT COUNT(*) FROM auth");
        except sqlite.DatabaseError:
            self.empty()

    def close(self):
        if self.connection is not None:
            self.connection.close()
            self.connection = None

    def empty(self):
        """Reset the database to a default empty state"""

        # Find and destroy every table in the database
        self.cursor.execute("SELECT tbl_name FROM sqlite_master WHERE type='table'")
        tables = [row.tbl_name for row in self.cursor.fetchall()]
        for table in tables:
            self.cursor.execute("DROP TABLE %s" % table)

        # Apply the schema
        self.cursor.execute(self.schema)

    def store(self, id, password):
        """Save or overwrite a password in the database"""

        # First try inserting a new row
        try:
            self.cursor.execute("INSERT INTO auth (id, password) VALUES ('%s', '%s')" %
                                (sqlite.encode(id), sqlite.encode(password)))
        except sqlite.IntegrityError:
            # Violated the primary key constraint, update an existing item
            self.cursor.execute("UPDATE auth SET password = '%s' WHERE id = '%s'" %
                                (sqlite.encode(password), sqlite.encode(id)))

        self.connection.commit()

    def lookup(self, id, ignoreStored=False):
        """Lookup a password for the given ID, returning via a Deferred"""
        if not ignoreStored:
            self.cursor.execute("SELECT password FROM auth WHERE id = '%s'" %
                                sqlite.encode(id))
            row = self.cursor.fetchone()
            if row:
                return defer.succeed(sqlite.decode(row[0]))

        result = defer.Deferred()
        defer.maybeDeferred(self.promptForPassword, id).addCallback(
            self._storeNewPassword, id, result).addErrback(result.errback)
        return result

    def _storeNewPassword(self, password, id, result):
        self.store(id, password)
        result.callback(password)

    def promptForPassword(self, id):
        """This is called when there is no stored password. The default
           implementation throws an exception, since we have no UI.
           """
        raise NotImplementedError


class ConsoleKeychain(Keychain):
    """A Keychain subclass that prompts on the console for unknown passwords"""
    def promptForPassword(self, id):
        print "Authenticating device %r" % id
        return getpass.getpass()

### The End ###

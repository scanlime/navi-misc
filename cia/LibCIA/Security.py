""" LibCIA.Security

Implements CIA's simple security model. CIA uses a simple
capabilities-like system, where a particular capability is represented
on the wire as an unguessable random key, and internally by nearly any
python object.

Generally the 'universe' key will be saved somewhere only the server's
owner can access it on startup. The 'universe' key can be used to grant
other keys, which can then be distributed to other people or machines.

Note that this system has many of the same qualities as traditional
capabilities, but is not implemented in the same way. In traditional
capabilities, the unguessable keys map directly to objects that provide
whatever interface that key grants permissions to. This is simple and
effective for some systems, however in CIA the meaning of a key must be
preserved over a long period of time regardless of code changes- simply
making keys map to pickled callable objects would be too fragile.
"""
#
# CIA open source notification system
# Copyright (C) 2003-2004 Micah Dowty <micahjd@users.sourceforge.net>
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

from twisted.internet import defer
from twisted.enterprise.util import quote as quoteSQL
import string, os
import Database, RPC


class SecurityInterface(RPC.Interface):
    """An XML-RPC interface to the global capabilities database"""
    def protected_grant(self, capability, owner=None):
        """Return a new key for the given capability. Note that this means
           that the capability to use this function is effectively equivalent
           to the 'universe' key
           """
        return caps.grant(capability, owner)

    def xmlrpc_test(self, key, *capabilities):
        """Test the given key against one or more capabilities, returning True if it
           matches any of them, False otherwise.
           """
        return caps.test(key, *capabilities)


def createRandomKey(bytes = 24,
                    allowedChars = string.ascii_letters + string.digits):
    """Create a somewhat-secure random string of the given length.
       This implementation probably only works on Linux and similar systems.
       Also note that since we're using /dev/urandom instead of /dev/random,
       the system might be out of entropy. Using /dev/random however could
       block, and would make everything else here more complex.
       The result will be base64-encoded.
       """
    s = ''
    f = open("/dev/urandom")
    for i in xrange(bytes):
        s += allowedChars[ ord(f.read(1)) % len(allowedChars) ]
    f.close()
    return s


class SecurityException(Exception):
    pass


class CapabilityDB:
    """A simple capability database, stored in an SQL table.
       Keys are unguessable random strings, identifiers are repr()'ed
       python objects representing what the key allows. The mapping
       between keys and identifiers doesn't have to be 1 to 1.

       This interface provides the ability to test a key for some particular
       capability, grant a new key for a particular capability, and revoke keys.

       All functions return a Deferred.
       """
    def saveKey(self, capability, file):
        """Save the key for a capability to the given filename.
           Useful for saving important keys on initialization so that
           they can later be used to retrieve other keys.
           This ensures that the freshly created key isn't readable
           by other users. Returns a Deferred that indicates the
           success or failure of the operation.
           """
        result = defer.Deferred()
        d = self.getKey(capability)
        d.addCallback(self._saveKeyCallback, os.path.expanduser(file), result)
        d.addErrback(result.errback)
        return result

    def _saveKeyCallback(self, key, file, result):
        f = open(file, "w")
        os.chmod(file, 0600)
        f.write(key)
        f.close()
        result.callback(None)

    def close(self):
        self.rack.close()

    def getKey(self, capability):
        """Find a key for the given capability with a NULL owner,
           creating one if it doesn't exist.
           """
        result = defer.Deferred()
        d = Database.pool.runQuery("SELECT key_data FROM capabilities WHERE id = %s AND owner IS NULL LIMIT 1" %
                                   quoteSQL(repr(capability), 'text'))
        d.addCallback(self._returnOrCreateKey, capability, result)
        d.addErrback(result.errback)
        return result

    def _returnOrCreateKey(self, keys, capability, result):
        """After looking for an existing key in getKey(), this either returns
           what was found or creates a new key.
           """
        if keys:
            result.callback(keys[0][0])
        else:
            result.callback(self.grant(capability))
        return keys

    def _createTestQuery(self, key, capabilities):
        """Create an SQL query that returns something nonzero if a key matches any of
           a list of capabilities. If the capabilities list is empty, this creates a
           query that always has a nonzero result.
           """
        if capabilities:
            return "SELECT 1 FROM capabilities WHERE key_data = %s AND (%s) LIMIT 1" % (
                quoteSQL(key, 'text'),
                " OR ".join(["id = " + quoteSQL(repr(c), 'text') for c in capabilities]),
                )
        else:
            return "SELECT 1"

    def test(self, key, *capabilities):
        """Test the given key for any of the given capabilities. Returns a Deferred"""
        result = defer.Deferred()
        d = Database.pool.runQuery(self._createTestQuery(key, capabilities))
        d.addCallback(self._testCallback, result)
        d.addErrback(result.errback)
        return result

    def _testCallback(self, matched, result):
        result.callback(bool(matched))

    def require(self, key, *capabilities):
        """Return a deferred that runs its callback if the key matches any of the given
           capabilities. If not, this raises an error explaining the capabilities required.
           """
        result = defer.Deferred()
        d = Database.pool.runQuery(self._createTestQuery(key, capabilities))
        d.addCallback(self._requireCallback, result, capabilities)
        d.addErrback(result.errback)
        return result

    def _requireCallback(self, matched, result, capabilities):
        if matched:
            result.callback(None)
        else:
            raise SecurityException("One of the following capabilities are required: " +
                                    repr(capabilities)[1:-1])

    def grant(self, capability, owner=None):
        """Create a new key for some capability. The key is generated and returned
           immediately, and the process of adding it to the database is started.
           """
        newKey = createRandomKey()
        if owner:
            owner = quoteSQL(owner, 'text')
        else:
            owner = "NULL"

        Database.pool.runOperation("INSERT INTO capabilities (key_data, id, owner) values(%s, %s, %s)" % (
            quoteSQL(newKey, 'text'),
            quoteSQL(repr(capability), 'text'),
            owner,
            ))
        return newKey


caps = CapabilityDB()

### The End ###

""" LibCIA.Security

Implements CIA's simple security model. CIA uses a simple capabilities
system, where a particular capability is represented on the wire as an
unguessable random key, and internally by any hashable python object.

For example, the capability for rebuilding the server may be called
'rebuild' internally. Any object with access to the CapabilityDB can
retrieve the key associated with the 'rebuild' capability or test a
given key against the 'rebuild' capability.

Any object with access to the CapabilityDB can be infinitely priveliged,
as it can retrieve any capability's key. As the CapabilityDB is stored on
disk, this includes any process with read access to the on-disk database.
This means that the local user running CIA is a superuser, but can hand
out particular capabilities to any other entity without handing over any
particular identity.
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
import anydbm, cPickle, base64, os


class SecurityInterface(xmlrpc.XMLRPC):
    """An XML-RPC interface to the capabilities database"""
    def __init__(self, caps):
        self.caps = caps

    def xmlrpc_revoke(self, capability, key):
        """Revoke the given capability, invalidating its current key"""
        self.caps.faultIfMissing(key, 'universe', 'security.revoke')
        self.caps.revoke(capability)
        return True

    def xmlrpc_grant(self, capability, key):
        """Return the key for the given capability. Note that this means
           that the capability to use this function is effectively equivalent
           to the 'universe' key- this is why there are no 'security' or
           'security.grant' capabilities.
           """
        self.caps.faultIfMissing(key, 'universe')
        return self.caps.grant(capability)

    def xmlrpc_list(self):
        """Return a list of all capabilities that have been assigned keys"""
        self.caps.faultIfMissing(key, 'universe', 'security.list')
        return self.caps.list()


def createRandomKey(bytes=128):
    """Create a somewhat-secure random string of the given number of bytes.
       This implementation probably only works on Linux and similar systems.
       Also note that since we're using /dev/urandom instead of /dev/random,
       the system might be out of entropy. Using /dev/random however could
       block, and would make everything else here more complex.
       """
    f = open("/dev/urandom")
    n = f.read(bytes)
    f.close()
    return n


class CapabilityDB(object):
    """A simple capability database- capabilities are mapped from a
       short identifier that can be any hashable python object to an
       actual capability key taking the form of an 'unguessable' random
       number.

       This database provides the ability to test a key for some particular
       capability, retrieve the key for a particular capability, and revoke
       capabilities. Revoking a capability invalidates the key assigned to it.

       Internally, the capability is an anydbm-compatible database where
       keys are pickled representations of a capability, and values are the
       associated random keys stored as 8-bit strings.
       """
    def __init__(self, fileName, flags='c', mode=0600):
        self.db = anydbm.open(fileName, flags, mode)

    def saveKey(self, capability, file):
        """Save the key for a capability to the given filename.
           Useful for saving important keys on initialization so that
           they can later be used to retrieve other keys.
           This ensures that the freshly created key isn't readable
           by other users.
           """
        f = open(file, "w")
        os.chmod(file, 0600)
        f.write(self.grant(capability))
        f.close()

    def close(self):
        self.db.close()

    def _dbKey(self, capability):
        """Returns the database key used for the given capability"""
        return cPickle.dumps(capability)

    def _dbCap(self, dbkey):
        """Returns the capability associated with the given db key"""
        return cPickle.loads(dbkey)

    def test(self, key, capability):
        """Test the given key for some capability"""
        return key and self.grant(capability, create=False) == key

    def faultIfMissing(self, key, *capabilities):
        """Raise a fault if the given key doesn't match any of the given capabilities"""
        for capability in capabilities:
            if self.test(key, capability):
                return
        import xmlrpclib
        raise xmlrpclib.Fault("SecurityException",
                              "One of the following capabilities are required: " +
                              repr(capabilities)[1:-1])

    def grant(self, capability, create=True):
        """Return the key for some capability, optionally creating it
           if it doesn't exist. If it doesn't exist and create is False,
           returns None.
           """
        try:
            return self.db[self._dbKey(capability)]
        except KeyError:
            if create:
                return self.create(capability)

    def create(self, capability):
        """Create a new key for the given capability, returning it"""
        key = base64.encodestring(createRandomKey())
        self.db[self._dbKey(capability)] = key
        return key

    def revoke(self, capability):
        """Delete the current key for the given capability if one exists"""
        try:
            del self.db[self._dbKey(capability)]
        except KeyError:
            pass

    def list(self):
        """Return all capabilities we have in the database"""
        return map(self._dbCap, self.db.keys())

### The End ###

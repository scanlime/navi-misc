""" LibCIA.Security

Implements CIA's simple security model. CIA uses a simple
capabilities-like system, where a particular capability is represented
on the wire as an unguessable random key, and internally by any python
object that can be serialized into a capability.

Generally the 'universe' key will be saved somewhere only the server's
owner can access it on startup. The 'universe' key can be used to grant
other keys, which can then be distributed to other people or machines.

Keys are represented in the database as a chunk of random binary data,
but they are represented everywhere else base64-encoded, to make them
XML-friendly.

Note that this system has many of the same qualities as traditional
capabilities, but is not implemented in the same way. In traditional
capabilities, the unguessable keys map directly to objects that provide
whatever interface that key grants permissions to. This is simple and
effective, and any number of proxy objects can be created to grant
different keys to one set of functionality. However, this is also a
problem- every time a capability was granted, a new object would be
created. With CIA's simple request-based interfaces, there would be
no way to determine when a capability was no longer needed.

The current system simplifies this in some ways by making keys associate
with simple identifiers that are checked for manually in any externally
accessable functions that need protection. Currently there is no way
to assign multiple keys to one capability- this might be implemented by
using shelve instead of anydbm and representing capability keys as lists.
This however presents some of the same challenges as just implementing
a real capabilities system.
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
import binascii, os
import Rack


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

    def xmlrpc_list(self, key):
        """Return a list of all capabilities that have been assigned keys"""
        self.caps.faultIfMissing(key, 'universe', 'security.list')
        return self.caps.list()

    def xmlrpc_test(self, capability, key):
        """Test the given key against a capability, returning True if it
           passes, False if it fails.
           """
        return self.caps.test(key, capability)


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


def decodeKey(key):
    """Convert the external representation of a key to the binary representation"""
    if key:
        return binascii.a2b_base64(key)

def encodeKey(key):
    """Conver the binary representation of a key to the external representation"""
    return binascii.b2a_base64(key)


class CapabilityDB(object):
    """A simple capability database- capabilities are mapped from a
       short identifier to an actual capability key taking the form of
       an 'unguessable' random number. Capability identifiers are any
       python object compatible with Rack.KeyPickler.

       This database provides the ability to test a key for some particular
       capability, retrieve the key for a particular capability, and revoke
       capabilities. Revoking a capability invalidates the key assigned to it.

       We use a rack to map from serialized Capabilities to raw capability keys.

          >>> caps = CapabilityDB('/tmp/capability_test.db')
          >>> bunny_key = caps.grant('fuzzyBunny')
          >>> banana_key = caps.grant(('banana', None))
          >>> caps.test(bunny_key, 'fuzzyBunny')
          True
          >>> caps.test(banana_key, ('banana', None))
          True
          >>> caps.test(bunny_key, ('banana', None))
          False
          >>> caps.test(banana_key, ('banana', 4))
          False
       """
    def __init__(self, fileName, flags='c', mode=0600):
        self.rack = Rack.open(fileName, flags, mode)

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
        self.rack.close()

    def test(self, key, capability):
        """Test the given key for some capability"""
        decoded = decodeKey(key)
        try:
            return self.rack[capability] == decoded
        except KeyError:
            return False

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
            return encodeKey(self.rack[capability])
        except KeyError:
            if create:
                return self.create(capability)

    def create(self, capability):
        """Create a new key for the given capability, returning it"""
        key = createRandomKey()
        self.rack[capability] = key
        return encodeKey(key)

    def revoke(self, capability):
        """Delete the current key for the given capability if one exists"""
        try:
            del self.rack[capability]
        except KeyError:
            pass

    def list(self):
        """Return all capabilities we have in the database"""
        return self.rack.keys()


def _test():
    import doctest, Security
    return doctest.testmod(Security)

if __name__ == "__main__":
    _test()

### The End ###

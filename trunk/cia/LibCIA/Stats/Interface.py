""" LibCIA.Stats.Handler

The stats:// URI handler, the root of the stats XML-RPC interface,
and the maintenance system. All the usual entry points for the stats system.
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

from twisted.python import log
from twisted.internet import defer
from twisted.web import xmlrpc
from LibCIA import RpcServer
from Target import StatsTarget
import Subscription


class StatsInterface(RpcServer.Interface):
    """An XML-RPC interface used to query stats"""
    def __init__(self):
        RpcServer.Interface.__init__(self)
        self.putSubHandler('metadata', MetadataInterface())
        self.putSubHandler('subscribe', Subscription.SubscriptionInterface())

    def xmlrpc_catalog(self, path=''):
        """Return a list of subdirectories within this stats path"""
        result = defer.Deferred()
        d = StatsTarget(path).catalog()
        d.addErrback(result.errback)
        d.addCallback(self._catalog, result)
        return result

    def _catalog(self, items, result):
        """Convert the returned catalog into target names and return them via
           the provided Deferred instance.
           """
        result.callback([target.name for target in items])

    def xmlrpc_getLatestMessages(self, path, limit=None):
        """Return 'limit' latest messages delivered to this stats target,
           or all available recent messages if 'limit' isn't specified.
           """
        return StatsTarget(path).messages.getLatest(limit)

    def xmlrpc_getCounterValues(self, path, name):
        """Returns a dictionary with current values for the given counter.
           Note that times are returned as UNIX-style seconds since
           the epoch in UTC.
           """
        return StatsTarget(path).counters.getCounter(name)

    def protected_clearTarget(self, path):
        """Deletes any data stored at a given stats target or in any of its subtargets"""
        log.msg("Clearing stats path %r" % path)
        return StatsTarget(path).clear()

    def caps_clearTarget(self, rpcPath, statsPath):
        """In addition to the usual capabilities, allow ('stats.path', path)"""
        return self.makeDefaultCaps(rpcPath) + [('stats.path', statsPath)]


class MetadataInterface(RpcServer.Interface):
    """An XML-RPC interface for querying and modifying stats metadata"""
    def wrapTuple(self, t):
        """Wrap the value in a (value, type) tuple in an xmlrpc.Binary
           if the type doesn't start with text/.
           """
        if t and not t[1].startswith("text/"):
            return (xmlrpc.Binary(t[0]), t[1])
        else:
            return t

    def xmlrpc_get(self, path, name, default=False):
        """Get a (value, type) tuple for the metadata key with the given
           name, returning 'default' if it isn't found
           """
        result = defer.Deferred()
        StatsTarget(path).metadata.get(name, default).addCallback(
            self._get, result).addErrback(result.errback)
        return result

    def _get(self, t, result):
        """Backend for get() that ensures the results are serializable"""
        result.callback(self.wrapTuple(t))

    def xmlrpc_dict(self, path):
        """Return a mapping of names to (value, type) tuples for the given path"""
        result = defer.Deferred()
        StatsTarget(path).metadata.dict().addCallback(
            self._dict, result).addErrback(result.errback)
        return result

    def _dict(self, original, result):
        """Backend for dict() that ensures the results are serializable"""
        d = {}
        for name, t in original.iteritems():
            d[name] = self.wrapTuple(t)
        result.callback(d)

    def protected_set(self, path, name, value, mimeType='text/plain'):
        """Set a metadata key's value and MIME type"""
        return StatsTarget(path).metadata.set(name, str(value), mimeType)

    def protected_clear(self, path):
        """Remove all metadata for one target"""
        return StatsTarget(path).metadata.clear()

    def protected_remove(self, path, name):
        """Remove one metadata key for this target, if it exists"""
        return StatsTarget(path).metadata.remove(name)

### The End ###

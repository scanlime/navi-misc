""" LibCIA.Cache

A generic object cache. Arbitrary python objects are mapped to strings,
possibly containing arbitrary binary data. Every item in the cache has
an access time and an optional expiration date.
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

from LibCIA import Database
from twisted.internet import defer


class AbstractStringCache:
    """An abstract cache mapping arbitrary python parameters to
       a string. Subclasses should define the miss() function to
       generate the data being cached in the event of a cache miss.
       """
    def get(self, *args):
        """Retrieve the item associated with some set of arguments.
           If the item doesn't exist in the cache, this calls miss()
           with the same arguments to generate the item, and adds it
           to the cache.

           Returns a Deferred instance.
           """
        id = self.hash(self, *args)
        print "%r hashes to %r" % (args, id)
        result = defer.Deferred()
        Database.pool.runQuery("SELECT value, expiration FROM cache WHERE id = %s" %
                               Database.quote(id, 'varchar')).addCallback(
            self._get, result, id, args).addErrback(result.errback)
        return result

    def _get(self, rows, result, id, args):
        """This gets called after we've checked our database and either received
           a cached copy of the data or nothing. If we have a cached copy and it
           hasn't expired yet, return that. Otherwise, we can start creating the
           data to return.
           """
        if rows and rows[0][1] > time.time():
            # It's a cache hit, yay. Update the access time asynchronously,
            # and return the cached result immediately.
            result.callback(rows[0][0])
            Database.pool.runOperation("UPDATE cache SET atime = %s WHERE id = %s" %
                                       (Database.quote(int(time.time()), 'bigint'),
                                        Database.quote(id, 'varchar')))
        else:
            # Darn, a cache miss. Start generating the data, possibly asynchronously
            defer.maybeDeferred(self.miss, *args).addCallback(
                self.returnAndStoreValue, result, id).addErrback(result.errback)

    def returnAndStoreValue(self, value, result, id):
        """This is called after we've finished generating a value to return, when
           there was a cache miss. We report this value to the caller ASAP, then update
           our cache database asynchronously.
           """
        result.callback(value)
        Database.pool.runInteraction(self.storeValue, value, id)

    def storeValue(self, cursor, value, id):
        """A database interaction to store or update the current cache value for a given id"""
        cursor.execute("DELETE FROM cache WHERE id = %s" %
                       Database.quote(id, 'varchar'))
        # This has to be "INSERT IGNORE" so we don't kerpode if
        # two cache misses happened concurrently.
        cursor.execute("INSERT IGNORE INTO cache (id, value) VALUES (%s, '%s')" %
                       (Database.quote(id, 'varchar'),
                        Database.quoteBlob(str(value))))

    def miss(self, *args):
        """Subclasses must implement this to generate the data we're supposed
           to be caching in the event of a cache miss. The return value
           can be the result itself or a Deferred that will eventually yield the result.
           """
        pass

    def hash(self, *args):
        """Convert our arguments to something that will fit in the
           database's varchar(32) id field. This uses a hash()
           of the arguments plus our class, so each subclass of the
           abstract cache will have a different id space.
           """
        return str(hash((self.__class__, args)))

### The End ###

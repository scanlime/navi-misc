""" LibCIA.Cache

A generic object cache. Arbitrary python objects are mapped to files or strings.

"""
#
# CIA open source notification system
# Copyright (C) 2003-2005 Micah Dowty <micah@navi.cx>
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
import time, md5, os, random, cPickle
from LibCIA import Files

class CachePerformance:
    """Collects performance information about a cache.
       This is not persistent across server sessions.
       """
    def __init__(self, name):
        self.name = name
        self.hits = 0
        self.misses = 0

_cachePerformanceStorage = {}

def getNamedCachePerformance(name):
    """Retrieve a CachePerformance object with the given name,
       creating it if necessary.
       """
    global _cachePerformanceStorage
    try:
        return _cachePerformanceStorage[name]
    except KeyError:
        perf = CachePerformance(name)
        _cachePerformanceStorage[name] = perf
        return perf

def getCachePerformanceList():
    """Return an iterator for all registered CachePerformance instances"""
    global _cachePerformanceStorage
    return _cachePerformanceStorage.itervalues()


class AbstractFileCache:
    """An abstract cache mapping arbitrary python parameters to
       a files. Subclasses should define the miss() function to
       generate the data being cached in the event of a cache miss.
       """
    lifespan = None

    def get(self, *args):
        """Retrieve the item associated with some set of arguments.
           If the item doesn't exist in the cache, this calls miss()
           with the same arguments to generate the item, and adds it
           to the cache.

           Returns a Deferred instance that eventually results in a file object.
           """
        filename = self.getFilename(args)
        perf = getNamedCachePerformance(self.__class__.__name__)

        if os.path.isfile(filename):
            perf.hits += 1

            result = defer.Deferred()
            self._returnHit(filename, result)
            return result
        else:
            perf.misses += 1

            # We need to create the file. Do this atomically by first writing
            # to a temporary file then moving that.
            result = defer.Deferred()
            defer.maybeDeferred(self.miss, *args).addCallback(
                self._returnMiss, filename, result).addErrback(result.errback)
            return result

    def _returnHit(self, filename, result):
        result.callback(open(filename))

    def _returnMiss(self, tempFilename, filename, result):
        os.rename(tempFilename, filename)
        result.callback(open(filename))

    def miss(self, *args):
        """Subclasses must implement this to generate the data we're supposed
           to be caching in the event of a cache miss. Returns a filename where
           the result can be found- this file will then be moved to the correct
           place in the cache.

           Implementations are encouraged, but not required, to get their file
           name from self.getTempFilename().

           The return value can optionally be passed via a Deferred.
           """
        pass

    def getFilename(self, args):
        """Return a filename for a cached object corresponding to the provided args
           and this data type. This combines a hash of args with the name of this class.
           We create the cache directory if necessary.
           """
        hash = md5.md5(repr(args)).hexdigest()
        return Files.getCacheFile(self.__class__.__name__, hash)

    def getTempFilename(self):
        """Return a suggested temporary file name for miss() to use"""
        return Files.getTempFile()


class AbstractStringCache(AbstractFileCache):
    """Based on AbstractFileCache, this cache provides an interface based on strings
       rather than files. This still uses the same file-based caching mechanism
       as AbstractFileCache.
       """
    def _returnHit(self, filename, result):
        result.callback(open(filename, "rb").read())

    def _returnMiss(self, string, filename, result):
        tempFilename = self.getTempFilename()
        open(tempFilename, "wb").write(string)
        os.rename(tempFilename, filename)
        result.callback(string)


class AbstractObjectCache(AbstractFileCache):
    """Based on AbstractFileCache, this cache provides an interface based on
       arbitrary Python objects, serialized using cPickle.
       """
    def _returnHit(self, filename, result):
        result.callback(cPickle.load(open(filename, "rb")))

    def _returnMiss(self, obj, filename, result):
        tempFilename = self.getTempFilename()
        cPickle.dump(obj, open(tempFilename, "wb"), cPickle.HIGHEST_PROTOCOL)
        os.rename(tempFilename, filename)
        result.callback(obj)


class Maintenance:
    """Maintenance operations we run regularly to keep the cache in shape:"""

    def run(self):
        # FIXME: implement this for the new filesystem-based cache
        pass

### The End ###

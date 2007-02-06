""" LibCIA.Stats.Metadata

A system for storing and retrieving metadata associated with a
stats target. This includes an AbstractStringCache subclass
used to cache thumbnails generated with PIL.
"""
#
# CIA open source notification system
# Copyright (C) 2003-2007 Micah Dowty <micah@navi.cx>
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
from LibCIA import Database, Cache
import time
from cStringIO import StringIO
import Image


class Metadata:
    """An abstraction for the metadata that may be stored for any stats target.
       Metadata objects consist of a name, a MIME type, and a value. The value
       can be any binary or text object stored as a string.
       """
    def __init__(self, target):
        self.target = target
        self._cache = None

    def get(self, name, default=None):
        """Return a Deferred that results in the (value, type) tuple for the the
           given metadata key, or 'default' if a result can't be found.
           """
        return Database.pool.runInteraction(self._get, name, default)

    def getValue(self, name, default=None, typePrefix='text/'):
        """Like get(), but only returns the value. Ensures the MIME type
           begins with typePrefix. If it doesn't, a TypeError is raised.
           """
        return Database.pool.runInteraction(self._getValue, name, default, typePrefix)

    def getMTime(self, name, default=None):
        """Returns a Deferred that eventually results in the modification
           time for the specified metadata object, in seconds since the epoch.
           This returns the specified default if the item doesn't exist. Note that
           None might be returned if the object was created before mtime support
           was included in the metadata table.
           """
        # This is not supported by the new stats database, and it isn't
        # needed now that images are stored separately.
        raise NotImplementedError

    def set(self, name, value, mimeType='text/plain'):
        """Set a metadata key, creating it if it doesn't exist"""
        # Writing to metadata from the LibCIA codebase is deprecated
        raise NotImplementedError

    def keys(self):
        """Return (via a Deferred) a list of all valid metadata key names"""
        return Database.pool.runInteraction(self._keys)

    def dict(self):
        """Return (via a Deferred) a mapping from names to (value, type) tuples"""
        return Database.pool.runInteraction(self._dict)

    def clear(self):
        """Delete all metadata for this target. Returns a Deferred"""
        # Writing to metadata from the LibCIA codebase is deprecated
        raise NotImplementedError

    def remove(self, name):
        """Remove one metadata key, with the given name"""
        # Writing to metadata from the LibCIA codebase is deprecated
        raise NotImplementedError

    def has_key(self, name):
        """Returs True (via a Deferred) if the given key name exists for this target"""
        return Database.pool.runInteraction(self._has_key, name)

    def _update_cache(self, cursor):
        """Retrieve all metadata keys for this target, populating self._cache."""

        # XXX: This only works for text keys. Images can't be fully represented
        #      in the old system, so we're ignoring them until the transition is complete.
        keys = ('title', 'subtitle', 'url', 'description', 'links-filter', 'related-filter')

        # Our new column names use underscores instead of dashes
        columns = [key.replace('-', '_') for key in keys]

        cursor.execute("SELECT %s FROM stats_statstarget WHERE path = %s" % (
            ', '.join(columns),
            Database.quote(self.target.path, 'varchar')))

        row = cursor.fetchone()
        self._cache = {}
        if row:
            for i, key in enumerate(keys):
                self._cache[key] = row[i]

    def _has_key(self, cursor, name):
        """Database interaction implemented has_key()"""
        if self._cache is None:
            self._update_cache(cursor)
        return self._cache.get(name) is not None

    def _get(self, cursor, name, default):
        """Database interaction to return the value and type for a particular key"""
        if self._cache is None:
            self._update_cache(cursor)
        v = self._cache.get(name)
        if v is None:
            return default
        else:
            return v, 'text/plain'

    def _getValue(self, cursor, name, default, typePrefix):
        # XXX: typePrefrix is being ignored. This is an artifact of the
        #      transition from old metadata system to new...
        if self._cache is None:
            self._update_cache(cursor)
        v = self._cache.get(name)
        if v is None:
            return default
        else:
            return v

    def _dict(self, cursor):
        """Database interaction to return to implement dict()"""
        if self._cache is None:
            self._update_cache(cursor)
        d = {}
        for k, v in self._cache.items():
           d[k] = (v, 'text/plain')
        return d

    def _keys(self, cursor):
        """Database interaction implementing keys()"""
        if self._cache is None:
            self._update_cache(cursor)
        return self._cache.keys()


class MetadataThumbnailCache(Cache.AbstractFileCache):
    """A cache for thumbnails of image metadata, generated using PIL.
       The cache is keyed by a target, metadata key, maximum
       thumbnail size, and metadata key modification time.
       """
    # The mime type our generated thumbnails should be in
    mimeType = 'image/png'

    def miss(self, target, metadataKey, size, mtime=None):
        # Get the metadata value first
        result = defer.Deferred()
        target.metadata.getValue(metadataKey, typePrefix='image/').addCallback(
            self.makeThumbnail, size, result).addErrback(result.errback)
        return result

    def makeThumbnail(self, imageData, size, result):
        i = Image.open(StringIO(imageData))

        # Our thumbnails look much better if we paste the image into
        # a larger transparent one first with a margin about equal to one
        # pixel in our final thumbnail size. This smoothly blends
        # the edge of the image to transparent rather than chopping
        # off a fraction of a pixel. It looks, from experimentation,
        # like this margin is only necessary on the bottom and right
        # sides of the image.
        margins = (i.size[0] // size[0] + 1,
                   i.size[1] // size[1] + 1)
        bg = Image.new("RGBA",
                       (i.size[0] + margins[0],
                        i.size[1] + margins[1]),
                       (255, 255, 255, 0))
        bg.paste(i, (0,0))

        bg.thumbnail(size, Image.ANTIALIAS)

        tempFilename = self.getTempFilename()
        bg.save(tempFilename, 'PNG')
        result.callback(tempFilename)

### The End ###

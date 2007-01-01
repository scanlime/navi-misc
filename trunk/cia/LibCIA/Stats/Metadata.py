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
        return Database.pool.runInteraction(self._getMTime, name, default)

    def getThumbnail(self, name, size):
        """Returns a thumbnail of the given metadata object
           (which must have an image/* mime type) no larger than the given size.
           The return value is a (fileObject, type) tuple.
           """
        # First we have to look up the modification time, so old
        # cached thumbnails can be invalidated.
        result = defer.Deferred()
        self.getMTime(name).addCallback(
            self._getThumbnailWithMTime, name, size, result).addErrback(result.errback)
        return result

    def _getThumbnailWithMTime(self, mtime, name, size, result):
        # The rest of getThumbnail. We have the modification time now, so we can
        # ask the cache to either retrieve or create a thumbnail image.
        cache = MetadataThumbnailCache()
        cache.get(self.target, name, size, mtime).addCallback(
            self._finishGetThumbnail, cache, result).addErrback(result.errback)

    def _finishGetThumbnail(self, fileObject, cache, result):
        # The last step of getThumbnail. We have the finished thumbnail,
        # return it and the mime type to the result deferred.
        result.callback((fileObject, cache.mimeType))

    def set(self, name, value, mimeType='text/plain'):
        """Set a metadata key, creating it if it doesn't exist"""
        return Database.pool.runInteraction(self._set, name, value, mimeType)

    def keys(self):
        """Return (via a Deferred) a list of all valid metadata key names"""
        return Database.pool.runInteraction(self._keys)

    def dict(self):
        """Return (via a Deferred) a mapping from names to (value, type) tuples"""
        return Database.pool.runInteraction(self._dict)

    def clear(self):
        """Delete all metadata for this target. Returns a Deferred"""
        return Database.pool.runOperation("DELETE FROM stats_metadata WHERE target_path = %s" %
                                          Database.quote(self.target.path, 'varchar'))

    def remove(self, name):
        """Remove one metadata key, with the given name"""
        return Database.pool.runOperation("DELETE FROM stats_metadata WHERE target_path = %s AND name = %s" %
                                          (Database.quote(self.target.path, 'varchar'),
                                           Database.quote(name, 'varchar')))

    def has_key(self, name):
        """Returs True (via a Deferred) if the given key name exists for this target"""
        return Database.pool.runInteraction(self._has_key, name)

    def _has_key(self, cursor, name):
        """Database interaction implemented has_key()"""
        cursor.execute("SELECT COUNT(*) FROM stats_metadata WHERE target_path = %s AND name = %s" %
                       (Database.quote(self.target.path, 'varchar'),
                        Database.quote(name, 'varchar')))
        return bool(cursor.fetchone()[0])

    def _get(self, cursor, name, default):
        """Database interaction to return the value and type for a particular key"""
        cursor.execute("SELECT value, mime_type FROM stats_metadata WHERE target_path = %s AND name = %s" %
                       (Database.quote(self.target.path, 'varchar'),
                        Database.quote(name, 'varchar')))
        return cursor.fetchone() or default

    def _getValue(self, cursor, name, default, typePrefix):
        result = self._get(cursor, name, None)
        if result is None:
            return default
        value, mimeType = result
        if not mimeType.startswith(typePrefix):
            raise TypeError("A metadata key of type %s was found where %s* was expected" %
                            (mimeType, typePrefix))
        return value

    def _getMTime(self, cursor, name, default):
        cursor.execute("SELECT mtime FROM stats_metadata WHERE target_path = %s AND name = %s" %
                       (Database.quote(self.target.path, 'varchar'),
                        Database.quote(name, 'varchar')))
        row = cursor.fetchone()
        if row:
            return row[0]
        else:
            return default

    def _dict(self, cursor):
        """Database interaction to return to implement dict()"""
        cursor.execute("SELECT name, value, mime_type FROM stats_metadata WHERE target_path = %s" %
                       Database.quote(self.target.path, 'varchar'))
        results = {}
        while True:
            row = cursor.fetchone()
            if row is None:
                break
            results[row[0]] = (row[1], row[2])
        return results

    def _set(self, cursor, name, value, mimeType):
        """Database interaction implementing set(). This first runs a dummy
           'insert ignore' to ensure that the row exists in our table, then
           runs an update to change its value.
           """
        # Make sure our row exists. This is wrapped in an autoCreateTargetFor
        # so that if the stats target doesn't exist, it is also automatically created.
        self.target._autoCreateTargetFor(cursor, cursor.execute,
                                         "INSERT IGNORE INTO stats_metadata (target_path, name) VALUES(%s, %s)" %
                                         (Database.quote(self.target.path, 'varchar'),
                                          Database.quote(name, 'varchar')))

        # Now actually set the value
        cursor.execute("UPDATE stats_metadata SET mime_type = %s, mtime = %s, value = '%s' "
                       "WHERE target_path = %s AND name = %s" %
                       (Database.quote(mimeType, 'varchar'),
                        Database.quote(time.time(), 'bigint'),
                        Database.quoteBlob(value),
                        Database.quote(self.target.path, 'varchar'),
                        Database.quote(name, 'varchar')))

    def _keys(self, cursor):
        """Database interaction implementing keys()"""
        cursor.execute("SELECT name FROM stats_metadata WHERE target_path = %s" %
                       (Database.quote(self.target.path, 'varchar')))
        results = []
        while True:
            row = cursor.fetchone()
            if row is None:
                break
            results.append(row[0])
        return results


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

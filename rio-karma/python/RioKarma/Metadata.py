""" RioKarma.Metadata

This module extracts metadata from new media files, using mmpython,
and provides utilities for converting metadata between formats.

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

import md5, os, cPickle
import mmpython
from mmpython.audio import mp3info
import sqlite
from RioKarma import Paths


class RidCalculator:
    """This object calculates the RID of a file- a sparse digest used by Rio Karma.
       For files <= 64K, this is the file's md5sum. For larger files, this is the XOR
       of three md5sums, from 64k blocks in the beginning, middle, and end.
       """
    def fromSection(self, fileObj, start, end, blockSize=0x10000):
        """This needs a file-like object, as well as the offset and length of the portion
           the RID is generated from. Beware that there is a special case for MP3 files.
           """
        # It's a short file, compute only one digest
        if end-start <= blockSize:
            fileObj.seek(start)
            return md5.md5(fileObj.read(end-start)).hexdigest()

        # Three digests for longer files
        fileObj.seek(start)
        a = md5.md5(fileObj.read(blockSize)).digest()

        fileObj.seek(end - blockSize)
        b = md5.md5(fileObj.read(blockSize)).digest()

        fileObj.seek((start + end - blockSize) / 2)
        c = md5.md5(fileObj.read(blockSize)).digest()

        # Combine the three digests
        return ''.join(["%02x" % (ord(a[i]) ^ ord(b[i]) ^ ord(c[i])) for i in range(16)])

    def fromFile(self, filename, length=None, mminfo=None):
        """Calculate the RID from a file, given its name. The file's length and
           mmpython results may be provided if they're known, to avoid duplicating work.
           """
        if mminfo is None:
            mminfo = mmpython.parse(filename)

        f = open(filename, "rb")

        if length is None:
            f.seek(0, 2)
            length = f.tell()
            f.seek(0)

        # Is this an MP3 file? For some silliness we have to skip the header
        # and the last 128 bytes of the file. mmpython can tell us where the
        # header starts, but only in a somewhat ugly way.
        if isinstance(mminfo, mmpython.audio.eyed3info.eyeD3Info):
	    try:
	        offset = mp3info.MPEG(f)._find_header(f)[0]
            except ZeroDivisionError:
	        # This is a bit of a kludge, since mmpython seems to crash
		# here on some MP3s for a currently-unknown reason.
		print "WARNING, mmpython got a div0 error on %r" % filename
		offset = 0

	    if offset < 0:
                # Hmm, it couldn't find the header? Set this to zero
                # so we still get a usable RID, but it probably
                # won't strictly be a correct RID.
                offset = 0

            f.seek(0)
            return self.fromSection(f, offset, length-128)

        # Otherwise, use the whole file
        else:
            return self.fromSection(f, 0, length)


class BaseCache:
    """This is an abstract base class for objects that cache metadata
       dictionaries on disk. The cache is implemented as a sqlite database,
       with a 'dict' table holding administrative key-value data, and a
       'files' table holding both a pickled representation of the metadata
       and separate columns for all searchable keys.
       """
    # This must be defined by subclasses as a small integer that changes
    # when any part of the database schema or our storage format changes.
    schemaVersion = None

    # This is the template for our SQL schema. All searchable keys are
    # filled in automatically, but other items may be added by subclasses.
    schemaTemplate = """
    CREATE TABLE dict
    (
        name   VARCHAR(64) PRIMARY KEY,
        value  TEXT
    );

    CREATE TABLE files
    (
        %(keys)s,
        _pickled TEXT NOT NULL
    );
    """

    # A list of searchable keys, used to build the schema and validate queries
    searchableKeys = None
    keyType = "VARCHAR(255)"

    # The primary key is what ensures a file's uniqueness. Inserting a file
    # with a primary key identical to an existing one will update that
    # file rather than creating a new one.
    primaryKey = None

    def __init__(self, name):
        self.name = name
        self.connection = None

    def open(self):
        """Open the cache, creating it if necessary"""
        if self.connection is not None:
            return

        self.connection = sqlite.connect(Paths.getCache(self.name))
        self.cursor = self.connection.cursor()

        # See what version of the database we got. If it's empty
        # or it's old, we need to reset it.
        try:
            version = self._dictGet('schemaVersion')
        except sqlite.DatabaseError:
            version = None

        if version != str(self.schemaVersion):
            self.empty()

    def close(self):
        if self.connection is not None:
            self.sync()
            self.connection.close()
            self.connection = None

    def _getSchema(self):
        """Create a complete schema from our schema template and searchableKeys"""
        keys = []
        for key in self.searchableKeys:
            type = self.keyType
            if key == self.primaryKey:
                type += " PRIMARY KEY"
            keys.append("%s %s" % (key, type))

        return self.schemaTemplate % dict(keys=', '.join(keys))

    def _encode(self, obj):
        """Encode an object that may not be a plain string"""
        if type(obj) is unicode:
            obj = obj.encode('utf-8')
        elif type(obj) is not str:
            obj = str(obj)
        return "'%s'" % sqlite.encode(obj)

    def _dictGet(self, key):
        """Return a value stored in the persistent dictionary. Returns None if
           the key has no matching value.
           """
        self.cursor.execute("SELECT value FROM dict WHERE name = '%s'" % key)
        row = self.cursor.fetchone()
        if row:
            return sqlite.decode(row[0])

    def _dictSet(self, key, value):
        """Create or update a value stored in the persistent dictionary"""
        encodedValue = self._encode(value)

        # First try inserting a new item
        try:
            self.cursor.execute("INSERT INTO dict (name, value) VALUES ('%s', %s)" %
                                (key, encodedValue))
        except sqlite.IntegrityError:
            # Violated the primary key constraint, update an existing item
            self.cursor.execute("UPDATE dict SET value = %s WHERE name = '%s'" % (
                encodedValue, key))

    def sync(self):
        """Synchronize in-memory parts of the cache with disk"""
        self.connection.commit()

    def empty(self):
        """Reset the database to a default empty state"""

        # Find and destroy every table in the database
        self.cursor.execute("SELECT tbl_name FROM sqlite_master WHERE type='table'")
        tables = [row.tbl_name for row in self.cursor.fetchall()]
        for table in tables:
            self.cursor.execute("DROP TABLE %s" % table)

        # Apply the schema
        self.cursor.execute(self._getSchema())
        self._dictSet('schemaVersion', self.schemaVersion)

    def _insertFile(self, d):
        """Insert a new file into the cache, given a dictionary of its metadata"""

        # Make name/value lists for everything we want to update
        dbItems = {'_pickled': self._encode(cPickle.dumps(d, -1))}
        for column in self.searchableKeys:
            if column in d:
                dbItems[column] = self._encode(d[column])

        # First try inserting a new row
        try:
            names = dbItems.keys()
            self.cursor.execute("INSERT INTO files (%s) VALUES (%s)" %
                                (",".join(names), ",".join([dbItems[k] for k in names])))
        except sqlite.IntegrityError:
            # Violated the primary key constraint, update an existing item
            self.cursor.execute("UPDATE files SET %s WHERE %s = %s" % (
                ", ".join(["%s = %s" % i for i in dbItems.iteritems()]),
                self.primaryKey, self._encode(d[self.primaryKey])))

    def _deleteFile(self, key):
        """Delete a File from the cache, given its primary key"""
        self.cursor.execute("DELETE FROM files WHERE %s = %s" % (
            self.primaryKey, self._encode(key)))

    def _getFile(self, key):
        """Return a metadata dictionary given its primary key"""
        self.cursor.execute("SELECT _pickled FROM files WHERE %s = %s" % (
            self.primaryKey, self._encode(key)))
        row = self.cursor.fetchone()
        if row:
            return cPickle.loads(sqlite.decode(row[0]))

    def _findFiles(self, **kw):
        """Search for files. The provided keywords must be searchable.
           Yields a list of details dictionaries, one for each match.
           Any keyword can be None (matches anything) or it can be a
           string to match. Keywords that aren't provided are assumed
           to be None.
           """
        constraints = []
        for key, value in kw.iteritems():
            if key not in self.searchableKeys:
                raise ValueError("Key name %r is not searchable" % key)
            constraints.append("%s = %s" % (key, self._encode(value)))

        if not constraints:
            constraints.append("1")

        self.cursor.execute("SELECT _pickled FROM files WHERE %s" %
                            " AND ".join(constraints))
        row = None
        while 1:
            row = self.cursor.fetchone()
            if not row:
                break
            yield cPickle.loads(sqlite.decode(row[0]))

    def countFiles(self):
        """Return the number of files cached"""
        self.cursor.execute("SELECT COUNT(_pickled) FROM files")
        return int(self.cursor.fetchone()[0])

    def updateStamp(self, stamp):
        """The stamp for this cache is any arbitrary value that is expected to
           change when the actual data on the device changes. It is used to
           check the cache's validity. This function update's the stamp from
           a value that is known to match the cache's current contents.
           """
        self._dictSet('stamp', stamp)

    def checkStamp(self, stamp):
        """Check whether a provided stamp matches the cache's stored stamp.
           This should be used when you have a stamp that matches the actual
           data on the device, and you want to see if the cache is still valid.
           """
        return self._dictGet('stamp') == str(stamp)


class LocalCache(BaseCache):
    """This is a searchable metadata cache for files on the local disk.
       It can be used to speed up repeated metadata lookups for local files,
       but more interestingly it can be used to provide full metadata searching
       on local music files.
       """
    schemaVersion = 1
    searchableKeys = ('type', 'rid', 'title', 'artist', 'source', 'filename')
    primaryKey = 'filename'

    def lookup(self, filename):
        """Return a details dictionary for the given filename, using the cache if possible"""
        filename = os.path.realpath(filename)

        # Use the mtime as a stamp to see if our cache is still valid
        mtime = os.stat(filename).st_mtime
        cached = self._getFile(filename)

        if cached and int(cached.get('mtime')) == int(mtime):
            # Yay, still valid
            return cached['details']

        # Nope, generate a new dict and cache it
        details = {}
        Converter().detailsFromDisk(filename, details)
        generated = dict(
            type     = details.get('type'),
            rid      = details.get('rid'),
            title    = details.get('title'),
            artist   = details.get('artist'),
            source   = details.get('source'),
            mtime    = mtime,
            filename = filename,
            details  = details,
            )
        self._insertFile(generated)
        return details

    def findFiles(self, **kw):
        """Search for files that match all given search keys. This returns an iterator
           over filenames, skipping any files that aren't currently valid in the cache.
           """
        for cached in self._findFiles(**kw):
            try:
                mtime = os.stat(cached['filename']).st_mtime
            except OSError:
                pass
            else:
                if cached.get('mtime') == mtime:
                    yield cached['filename']

    def scan(self, path):
        """Recursively scan all files within the specified path, creating
           or updating their cache entries.
           """
        for root, dirs, files in os.walk(path):
            for name in files:
                filename = os.path.join(root, name)
                self.lookup(filename)

            # checkpoint this after every directory
            self.sync()

_defaultLocalCache = None

def getLocalCache(create=True):
    """Get the default instance of LocalCache"""
    global _defaultLocalCache
    if (not _defaultLocalCache) and create:
        _defaultLocalCache = LocalCache("local")
        _defaultLocalCache.open()
    return _defaultLocalCache


class Converter:
    """This object manages the connection between different kinds of
       metadata- the data stored within a file on disk, mmpython attributes,
       Rio attributes, and file extensions.
       """
    # Maps mmpython classes to codec names for all formats the player
    # hardware supports.
    codecNames = {
        mmpython.audio.eyed3info.eyeD3Info: 'mp3',
        mmpython.audio.mp3info.MP3Info:     'mp3',
        mmpython.audio.flacinfo.FlacInfo:   'flac',
        mmpython.audio.pcminfo.PCMInfo:     'wave',
        mmpython.video.asfinfo.AsfInfo:     'wma',
        mmpython.audio.ogginfo.OggInfo:     'vorbis',
        }

    # Maps codec names to extensions. Identity mappings are the
    # default, so they are omitted.
    codecExtensions = {
        'wave': 'wav',
        'vorbis': 'ogg',
        }

    def filenameFromDetails(self, details,
                            unicodeEncoding = 'utf-8'):
        """Determine a good filename to use for a file with the given metadata
           in the Rio 'details' format. If it's a data file, this will use the
           original file as stored in 'title'.

           Otherwise, it uses Navi's naming convention: Artist_Name/album_name/##_track_name.extension
           """
        if details.get('type') == 'taxi':
            return details['title']

        artist = details.get('artist', 'None').replace(os.sep, "").replace(" ", "_")
        album = details.get('source', 'None').replace(os.sep, "").replace(" ", "_").lower()
        track = details.get('tracknr', 0)
        title = details.get('title', 'None').replace(os.sep, "").replace(" ", "_").lower()

        name = "%s/%s/%02d_%s" % (artist, album, track, title)

        codec = details.get('codec')
        extension = self.codecExtensions.get(codec, codec)
        if extension:
            name += '.' + extension
        return unicode(name).encode(unicodeEncoding, 'replace')

    def detailsFromDisk(self, filename, details):
        """Automagically load media metadata out of the provided filename,
           adding entries to details. This works on any file type
           mmpython recognizes, and other files should be tagged
           appropriately for Rio Taxi.
           """
        info = mmpython.parse(filename)
        st = os.stat(filename)

        # Generic details for any file. Note that we start out assuming
        # all files are unreadable, and label everything for Rio Taxi.
        # Later we'll mark supported formats as music.
        details['length'] = st.st_size
        details['type'] = 'taxi'
        details['rid'] = RidCalculator().fromFile(filename, st.st_size, info)

        # We get the bulk of our metadata via mmpython if possible
        if info:
            self.detailsFromMM(info, details)

        if details['type'] == 'taxi':
            # All taxi files get their filename as their title, regardless of what mmpython said
            details['title'] = os.path.basename(filename)

            # Taxi files also always get a codec of 'taxi'
            details['codec'] = 'taxi'

        # Music files that still don't get a title get their filename minus the extension
        if not details.get('title'):
            details['title'] = os.path.splitext(os.path.basename(filename))[0]

    def detailsFromMM(self, info, details):
        """Update Rio-style 'details' metadata from MMPython info"""
        # Mime types aren't implemented consistently in mmpython, but
        # we can look at the type of the returned object to decide
        # whether this is a format that the Rio probably supports.
        # This dictionary maps mmpython clases to Rio codec names.
        for cls, codec in self.codecNames.iteritems():
            if isinstance(info, cls):
                details['type'] = 'tune'
                details['codec'] = codec
                break

        # Map simple keys that don't require and hackery
        for fromKey, toKey in (
            ('artist', 'artist'),
            ('title', 'title'),
            ('album', 'source'),
            ('date', 'year'),
            ('samplerate', 'samplerate'),
            ):
            v = info[fromKey]
            if v is not None:
                details[toKey] = v

        # The rio uses a two-letter prefix on bit rates- the first letter
        # is 'f' or 'v', presumably for fixed or variable. The second is
        # 'm' for mono or 's' for stereo. There doesn't seem to be a good
        # way to get VBR info out of mmpython, so currently this always
        # reports a fixed bit rate. We also have to kludge a bit because
        # some metdata sources give us bits/second while some give us
        # kilobits/second. And of course, there are multiple ways of
        # reporting stereo...
        kbps = info['bitrate']
        if type(kbps) in (int, float) and kbps > 0:
            stereo = bool( (info['channels'] and info['channels'] >= 2) or
                           (info['mode'] and info['mode'].find('stereo') >= 0) )
            if kbps > 8000:
                kbps = kbps // 1000
            details['bitrate'] = ('fm', 'fs')[stereo] + str(kbps)

        # If mmpython gives us a length it seems to always be in seconds,
        # whereas the Rio expects milliseconds.
        length = info['length']
        if length:
            details['duration'] = int(length * 1000)

        # mmpython often gives track numbers as a fraction- current/total.
        # The Rio only wants the current track, and we might as well also
        # strip off leading zeros and such.
        trackNo = info['trackno']
        if trackNo:
            details['tracknr'] = int(trackNo.split("/", 1)[0])

### The End ###

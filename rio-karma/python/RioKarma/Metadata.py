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

import md5, os
import mmpython


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
            return md5.md5(fileObj.read(length)).hexdigest()

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
            offset = mminfo._find_header(f)[0]
            f.seek(0)
            return self.fromSection(f, offset, length-128)

        # Otherwise, use the whole file
        else:
            return self.fromSection(f, 0, length)


class Converter:
    """This object manages the connection between different kinds of
       metadata- the data stored within a file on disk, mmpython attributes,
       Rio attributes, and file extensions.
       """
    # Maps mmpython classes to codec names for all formats the player
    # hardware supports.
    codecNames = {
        mmpython.audio.eyed3info.eyeD3Info: 'mp3',
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
                            replaceSpaces   = True,
                            lowercase       = True,
                            unicodeEncoding = 'utf-8'):
        """Determine a good filename to use for a file with the given metadata
           in the Rio 'details' format. If it's a data file, this will use the
           original file as stored in 'title'. Otherwise, it cleans up the filename
           (always removing slashes, optionally replacing spaces with underscores)
           and adds an extension.
           """
        if details.get('type') == 'taxi':
            return details['title']

        name = details['title']
        name = name.replace(os.sep, "")
        if replaceSpaces:
            name = name.replace(" ", "_")
        if lowercase:
            name = name.lower()

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
        if kbps and kbps > 0:
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

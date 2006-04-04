""" RioKarma.Filesystem

This module implements a local model of the Rio's filesystem, including
objects for an in-memory model and a persistent cache implemented with
an sqlite database. This filesystem model must be used to do anything
useful with the rio: when uploading files we need to be able to allocate
file IDs, and before downloading a file we need its ID. The model
synchronizes with the device, then it can be queried for existing files
or IDs to use for new files.

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

import os, time, sys, cPickle
from twisted.internet import defer, reactor
from RioKarma import Request, Metadata, Progress


class AllocationTree:
    """This is a data structure for keeping track of unused areas of an
       abstract address space, marking areas as used, and looking for unused
       areas. It is a binary tree, with maximum depth equal to the number
       of bits in the address space. Every node (represented by a list) can
       either lead to another node or to a boolean indicating whether that
       space is used.

       If the tree were fully populated, it would be no better than a lookup
       table holding a flag for every address, scanned linearly every time we
       wanted to find an available one. To avoid this, the tree is optimized-
       a node with leaves for both children may only exist if the leaves have
       different values. Initially the tree's root node is a False leaf,
       indicating that the entire address space is empty. When we allocate
       space, leaves split into nodes as necessary. When traversing the tree
       looking for available space, this lets us jump over large blocks of
       unavailable space very quickly. This is very similar to the Karnaugh
       maps used to optimize simple digital logic circuits.
       """
    def __init__(self, bits):
        self.mask = 1 << (bits - 1)
        self.data = False

    def isAllocated(self, address):
        """Lookup the boolean value of a particular address"""
        data = self.data
        mask = self.mask

        while type(data) is list:
            data = data[bool(address & mask)]
            mask >>= 1

        return data

    def findFirst(self):
        """Find the first free location"""
        return self._findFirst(self.data, self.mask, 0)

    def _findFirst(self, data, mask, address):
        if type(data) is list:
            # A node- if the zero child isn't True, we can always follow
            # it and find a free address.
            if data[0] == True:
                return self._findFirst(data[1], mask >> 1, address | mask)
            else:
                return self._findFirst(data[0], mask >> 1, address)
        elif data:
            # Everything's allocated
            return None
        else:
            # Everything's free, return the first available
            # address by refraining from setting any additional
            # bits to the right of 'mask'
            return address

    def allocate(self, address):
        """Mark the given address as used"""
        mask = self.mask
        node = self.data
        getNode = lambda: self.data
        setNode = lambda x: setattr(self, 'data', x)
        stack = [(getNode, setNode)]

        while 1:
            if type(node) is not list:
                if node:
                    # Everything down this path is already allocated and we're done.
                    return
                else:
                    # Are we at the bottom of the tree yet?
                    if mask == 0:
                        # Yes. This leaf describes our address and only our address.
                        # We can safely mark it as True.
                        node = True
                        setNode(node)
                        break
                    else:
                        # Nope. We should split this leaf into a node with two
                        # False children. This isn't a legal condition for our
                        # tree, but it will be fixed up below.
                        node = [False, False]
                        setNode(node)

            # Traverse down a level
            i = bool(address & mask)
            getNode = lambda node=node, i=i: node.__getitem__(i)
            setNode = lambda x, node=node, i=i: node.__setitem__(i, x)
            node = node[i]
            mask >>= 1
            stack.append((getNode, setNode))

        # Now wander back up the tree, looking for nodes we can collapse into leaves
        stack.reverse()
        for getNode, setNode in stack:
            node = getNode()
            if node == [False, False]:
                setNode(False)
            elif node == [True, True]:
                setNode(True)


class FileIdAllocator:
    """File IDs, from experimentation, seem to be multiples of 0x10 between 0x110 and
       0xFFFF0. This uses a 16-bit-deep AllocationTree, with bit shifting to hide the
       requirement that IDs be multiples of 0x10. Additionally, we pre-mark IDs below
       0x110 as unusable.
       """
    def __init__(self):
        self.tree = AllocationTree(16)
        for i in xrange(0x11):
            self.tree.allocate(i)

    def isAllocated(self, address):
        return self.tree.isAllocated(address >> 4)

    def allocate(self, address):
        return self.tree.allocate(address >> 4)

    def findFirst(self):
        return self.tree.findFirst() << 4


class DeviceCache(Metadata.BaseCache):
    """This is a searchable local cache of one device's database. It is used
       to find remote files using metadata searhces, plus it keeps track
       of which file IDs are in use via a FileIdAllocator.
       """
    schemaVersion = 2
    searchableKeys = ('fid', 'type', 'rid', 'title', 'artist',
                      'source', 'codec', 'fid_generation', 'ctime')
    primaryKey = 'fid'
    fileIdAllocator = None

    def open(self):
        Metadata.BaseCache.open(self)

        # After opening, load the file ID allocator
        pickled = self._dictGet('fileIdAllocator')
        if pickled:
            self.fileIdAllocator = cPickle.loads(pickled)
        else:
            self.fileIdAllocator = FileIdAllocator()

    def sync(self):
        # Update the file ID allocator
        self._dictSet('fileIdAllocator', cPickle.dumps(self.fileIdAllocator, -1))
        Metadata.BaseCache.sync(self)

    def empty(self):
        # Create a new blank file ID allocator
        self.fileIdAllocator = FileIdAllocator()
        Metadata.BaseCache.empty(self)

    def insertFile(self, f):
        """Insert a new File object into the cache. If an existing file with this ID
           exists, it is updated rather than overwritten.
           """
        self.fileIdAllocator.allocate(f.id)
        self._insertFile(f.details)

    def deleteFile(self, f):
        """Delete a File object from the cache"""
        # FIXME: This can't remove the ID from the allocation tree yet
        self._deleteFile(f.details)

    def getFile(self, fid):
        """Return a File object, given a file ID"""
        details = self._getFile(fid)
        if details:
            return File(details=details)

    def findFiles(self, **kw):
        """Search for files that match all the given search keys.
           Returns an iterator that yields File objects.
           """
        for details in self._findFiles(**kw):
            yield File(details=details)


class FileManager:
    """The FileManager coordinates the activity of Cache and File objects
       with a real Rio device.
       """
    def __init__(self, protocol):
        self.protocol = protocol
        self.locksHeld = ()
        self.cache = None

        # Make sure that we get a chance to sync our cache and release
        # any I/O locks before we exit- otherwise the Rio might get stuck
        # on the data transfer screen, and our db might be out of sync.
        self._shutdownTrigger = reactor.addSystemEventTrigger('before', 'shutdown', self.close)

    def close(self):
        """Cleanly shut down the file manager and the connection. This will block
           waiting for a write lock to be released, if one was held. If you'd rather
           not block, release the lock before calling this.
           """
        if self.protocol:
            if 'write' in self.locksHeld:
                sys.stderr.write("\n\nReleasing write lock, please wait...\n")
                self.unlock()

                # Note that we're waiting on locksHeld, rather than the result
                # of this call to unlock(). This is in case someone else already
                # started an unlock that we're not aware of.
                while self.locksHeld:
                    reactor.iterate(0.1)

            # In case, while waiting for a lock, someone else beat us to it...
            if self.protocol:
                self.protocol.sendRequest(Request.Hangup())
                self.protocol = None

        # Close the cache *after* releasing locks, since releasing a
        # write lock will want to update the cache's stamp.
        if self.cache:
            self.cache.close()
            self.cache = None

        # Sync the default LocalCache, but only if it already exists
        if Metadata.getLocalCache(False):
            Metadata.getLocalCache().sync()

        # We only needed this once.. don't want this object to leak
        if self._shutdownTrigger:
            reactor.removeSystemEventTrigger(self._shutdownTrigger)
            self._shutdownTrigger = None

    def readLock(self):
        """Acquire a read-only lock on this filesystem- necessary for most
           operations, but also automatic on syncrhonization. The device
           is still usable as normal when a read lock is active.
           """
        if not self.locksHeld:
            self.locksHeld = ('read',)
        return self.protocol.sendRequest(Request.RequestIOLock('read'))

    def writeLock(self):
        """Acquire a read-write lock on this filesystem. This is necessary
           to modify anything, but it puts the device into an unusable
           state. Release this lock as soon as possible after you're done.
           """
        self.locksHeld = ('read', 'write')
        return self.protocol.sendRequest(Request.RequestIOLock('write'))

    def unlock(self):
        """Release any locks currently held"""
        result = Progress.Deferred()
        self.protocol.sendRequest(Request.ReleaseIOLock()).addStatusback(
            result.statusback).addCallback(
            self._updateStamp, result).addErrback(
            result.errback)
        return result

    def _updateStamp(self, retval, result):
        """We just finished asking the device to release locks. Make note
           that we no longer hold the write lock. If we were holding it,
           update the cache's stamp to reflect the changes we just made.
           """
        self.getStamp().addCallback(
            self._finishUnlock, result).addErrback(result.errback)

    def _finishUnlock(self, stamp, result):
        if 'write' in self.locksHeld:
            self.cache.updateStamp(stamp)
            self.cache.sync()
        self.locksHeld = ()
        result.callback(None)

    def synchronize(self):
        """Update our local database if necessary. Returns a Deferred
           signalling completion. This has the side-effect of acquiring
           a read lock.
           """
        # We need the latest stamp and our device info to make
        # sure the cache is open and see if it's up to date.
        d = Progress.Deferred()
        defer.gatherResults([
            self.getStamp(),
            self.protocol.sendRequest(Request.GetDeviceSettings()),
            ]).addCallback(
            self._startSynchronize, d).addErrback(
            d.errback)
        return d

    def _startSynchronize(self, (deviceStamp, deviceSettings), d):
        # If the cache is valid, skip all this mess
        self._openCache(deviceSettings['serial'])
        if self.cache.checkStamp(deviceStamp):
            d.callback(None)
            return

        # We don't really care about the readLock's result, and due to
        # the queue this is guaranteed to be sent before our actual sync.
        self.readLock()

        # Empty and rebuild all database files
        self.cache.empty()
        self.protocol.sendRequest(Request.GetAllFileDetails(
            self._synchronizeFile, d, [0])).addCallback(
            self._finishSynchronize, d, deviceStamp).addErrback(d.errback)

    def _synchronizeFile(self, details, d, storageTotal):
        # Add one file to the cache
        self.cache.insertFile(File(details=details))

        # Poke our Progress.Deferred with the latest status.
        # We're measuring our progress based on how much of the
        # device's utilized storage we can account for, since
        # there doesn't seem to be a way to get a total file count.
        storageTotal[0] += details.get('length', 0)
        d.statusback(storageTotal[0],
                     self.storageDetails['totalSpace'] -
                     self.storageDetails['freeSpace'],
                     name='Synchronizing')

    def _finishSynchronize(self, retval, d, deviceStamp):
        # Before signalling completion, update the cache stamp and sync it
        self.cache.updateStamp(deviceStamp)
        self.cache.sync()
        d.callback(None)

    def getStamp(self):
        """This returns an arbitrary value that is expected to change any
           time the actual content of the device changes. It is used to verify
           our cache's integrity. This returns via a Deferred, since it needs
           to communicate with the device.

           GetDeviceSettings returns a 'device_generation' field that seems
           to do exactly this- however it changes when a file's playcount
           is modified, so it's fairly annoying to use in practice. We currently
           use the amount of free space on the device.
           """
        result = defer.Deferred()
        self.protocol.sendRequest(Request.GetStorageDetails()).addCallback(
            self._getStamp, result).addErrback(result.errback)
        return result

    def _getStamp(self, storageDetails, result):
        self.storageDetails = storageDetails
        result.callback(storageDetails['freeSpace'])

    def _openCache(self, serial):
        """Ensure that our cache is open. If it isn't open yet, this opens
           one for our device given its unique serial number.
           """
        if not self.cache:
            self.cache = DeviceCache("device-%s" % serial)
            self.cache.open()

    def updateFileDetails(self, f):
        """Update our cache and the device itself with the latest details dictionary
           from this file object. Returns a Deferred signalling completion.
           """
        self.cache.insertFile(f)
        return self.protocol.sendRequest(Request.UpdateFileDetails(f.id, f.details))

    def createFile(self, title=None):
        """Allocate a new file ID, and return a new File object representing it.
           Beware that a file isn't actually allocated until its details are uploaded
           for the first time, so multiple new files shouldn't be created at once.
           """
        f = File(id=self.cache.fileIdAllocator.findFirst())
        if title is not None:
            f.details['title'] = unicode(title)
        return f

    def deleteFile(self, f):
        """Delete a file from the cache, and request that it be deleted from the
           device. Returns a Deferred signalling completion.
           """
        self.cache.deleteFile(f)
        return self.protocol.sendRequest(Request.DeleteFile(f.id))

    def loadFromDisk(self, remoteFile, localFilename, blockSize=None):
        """Load a file's metadata and content from a file on disk.
           Returns a Deferred signalling completion.
           """
        remoteFile.loadMetadataFrom(localFilename)
        result = Progress.Deferred()

        transfer = ContentTransfer(Request.WriteFileChunk, self.protocol,
                                   remoteFile, open(localFilename, "rb"), blockSize)

        transfer.start().addStatusback(
            result.statusback).addCallback(
            self._updateDetailsAfterLoad, remoteFile, result).addErrback(
            result.errback)
        return result

    def _updateDetailsAfterLoad(self, retval, remoteFile, result):
        """The Rio gets fairly angry if we update details on a file before
           it has any valid content. This is called after the transfer
           finishes when we're doing a loadFromDisk.
           """
        self.updateFileDetails(remoteFile).chainDeferred(result)

    def saveToDisk(self, remoteFile, localFilename, blockSize=None):
        """Save this file's content to disk. Returns a deferred signalling completion.
           This will create parent directories automatically, as necessary.
           """
        result = Progress.Deferred()

        parentDir = os.path.split(localFilename)[0]
        if parentDir and not os.path.isdir(parentDir):
            os.makedirs(parentDir)

        dest = open(localFilename, "wb")
        transfer = ContentTransfer(Request.ReadFileChunk, self.protocol,
                                   remoteFile, dest, blockSize)

        transfer.start().addStatusback(
            result.statusback).addCallback(
            self._finishSaveToDisk, dest, result.callback).addErrback(
            self._finishSaveToDisk, dest, result.errback)
        return result

    def _finishSaveToDisk(self, retval, dest, chainTo):
        """After the transfer for saveToDisk finishes, we explicitly
           close the file to be sure that its filesystem metadata is
           up to date by the time this returns.
           """
        dest.close()
        chainTo(retval)

    def getStorageDetails(self):
        return self.protocol.sendRequest(Request.GetStorageDetails())

    def getPlaylistFiles(self, playlist):
        """Given a File object representing a playlist, return a list of File
           objects for its children.
           """
        results = []
        if 'playlist' in playlist.details:
            for details in playlist.details['playlist']:
                results.extend(self.cache.findFiles(**details))
        return results

    def setPlaylistFiles(self, playlist, children):
        """Set the contents of a playlist from a list of File instances, given a
           File instance for the playlist itself. Playlists may be created and
           deleted with createFile/deleteFile.

           This requires a write lock, and returns a Deferred indicating completion.

           Note that for a playlist to actually appear in the menu, you have to place
           it within another playlist. This will essentially create a floating playlist.
           """
        playlist.details['type'] = 'playlist'

        # We store the children's metadata, but filtered to include
        # only fid and fid_generation. Non-searchable keys will cause
        # problems later, and those two are the only ones the device
        # will actually store.
        playlist.details['playlist'] = [
            dict(fid=f.details.get('fid', 0),
                 fid_generation=f.details.get('fid_generation', 0))
            for f in children]
        return self.updateFileDetails(playlist)

    def getRootPlaylist(self):
        """Playlists can contain other playlists- they're much like directories.
           There is a hardcoded root playlist, called 'All Music' in English,
           that the playlist menu first loads. Returns a File object.
           """
        # The name isn't always "All Music", but the FID seems to be hardcoded
        root = self.cache.getFile(0x100)
        if not root:
            # Shouldn't happen, but just in case... create a new root playlist
            root = File(0x100)
            root.details['title'] = u'All Music'
            root.details['type'] = 'playlist'
        return root


class File:
    """A File represents one media or data file corresponding to an entry in
       the device's database and our copy of that database. File objects
       always have an ID and a 'details' dictionary containing that file's
       metadata. Either of these may be provided when the file is created.
       Files can't do much on their own, since they have no knowledge
       of the device or the cache- the FileManager coordinates the movement
       and life of a File.
       """
    def __init__(self, id=None, details=None):

        if details is None:
            # New file, start out with barebones metadata
            if id is None:
                raise ValueError("File instances must be created with an 'id' and/or 'details'")
            now = int(time.time())
            details = {
                'ctime': now,
                'fid_generation': now,
                'fid': id,
                }
        else:
            id = details['fid']

        self.id = id
        self.details = details
        self._metadataLoadedFrom = None

    def loadMetadataFrom(self, filename):
        """Populate this object's metadata from a real file on disk. This is cached,
           such that if it is convenient to call this in multiple places to ensure
           that the file has valid metadata, this won't cause a performance penalty.
           """
        if self._metadataLoadedFrom != filename:
            self.details.update(Metadata.getLocalCache().lookup(filename))
            self._metadataLoadedFrom = filename

    def suggestFilename(self, **options):
        return Metadata.Converter().filenameFromDetails(self.details, **options)

    def __repr__(self):
        if self.details.get('type') == 'tune':
            return "<File 0x%05X: song %r on album %r by %r>" % (
                self.id,
                self.details.get('title').encode('ascii', 'replace'),
                self.details.get('source').encode('ascii', 'replace'),
                self.details.get('artist').encode('ascii', 'replace'),
                )
        else:
            return "<File 0x%05X: %r>" % (
                self.id,
                self.details.get('title'),
                )

    def __str__(self):
        artist = self.details.get('artist')
        if artist:
            name = "%s (%s)" % (self.details.get('title'), artist)
        else:
            name = self.details.get('title')
        return name.encode('ascii', 'replace')


class ContentTransfer:
    """This is an object that transfers content, in either direction, between the
       device and a file-like object. The transfer is buffered by Protocol, but
       also throttled so we don't dump the entire file into the request queue
       right away.
       """
    def __init__(self, request, protocol, remoteFile, localFile, blockSize=None):
        if blockSize is None:
            blockSize = 8192

        self.request = request
        self.protocol = protocol
        self.remoteFile = remoteFile
        self.localFile = localFile
        self.blockSize = blockSize

    def start(self):
        """Begins the transfer, and returns a Deferred signalling its completion"""
        self.offset = 0
        self.remaining = self.remoteFile.details['length']
        self.result = Progress.Deferred()
        self._next()
        return self.result

    def _error(self, err):
        """Handle an error in the file transfer. This prevents duplicate
           errors from reaching self.result, since if any blocks fail,
           the ones after that one surely will too.
           """
        if not self.result.called:
            self.result.errback(err)

    def _next(self):
        """Queue up another transfer block"""

        # If our result has already been issued, something went
        # wrong- bail out now to avoid a flood of 'already called'
        # errors.
        if self.result.called:
            return

        if self.remaining <= 0:
            # We're already done, let our caller know
            self.result.callback(None)
            return

        chunkLen = min(self.remaining, self.blockSize)
        d = self.protocol.sendRequest(self.request(
            self.remoteFile.id, self.offset, chunkLen, self.localFile))
        self.remaining -= chunkLen
        self.offset += chunkLen

        # Send a status update
        self.result.statusback(self.offset, self.offset + self.remaining,
                               'bytes', str(self.remoteFile))

        if self.remaining <= 0:
            # This is the last one, chain to our deferred and
            # stop setting up transfers.
            d.addCallback(self.result.callback)
            d.addErrback(self._error)
        else:
            # Queue up the next block once Protocol thinks we should
            d.addErrback(self._error)
            self.protocol.throttle(self._next)

### The End ###

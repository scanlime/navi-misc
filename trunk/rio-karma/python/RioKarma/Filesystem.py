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

import os, time, sys
import shelve, cPickle
from twisted.internet import defer
from RioKarma import Request, Metadata


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

    def next(self):
        i = self.findFirst()
        if i is not None:
            self.allocate(i)
        return i


class Cache:
    """This is a container for the several individual databases that make up
       our cache of the Rio's file database. It is responsible for opening,
       closing, synchronizing, and resetting all caches in unison.
       """
    def __init__(self, path):
        self.path = os.path.expanduser(path)

    def _openShelve(self, name, mode):
        """This is a convenience function for opening shelve databases using
           a common naming convention and parent directory.
           """
        return shelve.open(os.path.join(self.path, name + '.db'), mode)

    def _openPickle(self, name, mode, factory, *args, **kwargs):
        """This is a convenience function for opening pickled objects in the same
           fashion as an anydbm database.
           """
        filename = os.path.join(self.path, name + '.p')
        if mode == 'n' or not os.path.isfile(filename):
            return factory(*args, **kwargs)
        else:
            return cPickle.load(open(filename, "rb"))

    def _savePickle(self, obj, name):
        """A convenience function for saving pickled objects using
           our common naming convention
           """
        filename = os.path.join(self.path, name + '.p')
        cPickle.dump(obj, open(filename, "wb"), cPickle.HIGHEST_PROTOCOL)

    def open(self, mode='c'):
        """Open all cache components, creating them if necessary"""
        if not os.path.isdir(self.path):
            os.makedirs(self.path)

        self.fileDetails     = self._openShelve('file-details', mode)
        self.fileIdAllocator = self._openPickle('file-id-allocator', mode, FileIdAllocator)

    def close(self):
        self.sync()
        self.quickClose()

    def sync(self):
        """Synchronize in-memory parts of the cache with disk"""
        self.fileDetails.sync()
        self._savePickle(self.fileIdAllocator, 'file-id-allocator')

    def quickClose(self):
        """Close without regard to data integrity! This will close our
           databases, but does not flush in-memory caches to disk.
           This should only be used when we're throwing the cache
           away soon anyway.
           """
        self.fileDetails.close()

    def empty(self):
        """Discard the contents of the cache"""
        self.quickClose()
        self.open('n')


class Filesystem:
    """The Filesystem coordinates the activity of several File objects
       through one Protocol, and provides ways of discovering Files.
       It owns a local cache of the rio's database, which it can synchronize.

       The Filesystem is responsible for organizing metadata, but not for
       collecting it, or for dealing with file contents.
       """
    def __init__(self, protocol, cacheDir="~/.riokarma-py/cache"):
        self.protocol = protocol
        self.cache = Cache(cacheDir)
        self.cache.open()

    def readLock(self):
        """Acquire a read-only lock on this filesystem- necessary for most
           operations, but also automatic on syncrhonization. The device
           is still usable as normal when a read lock is active.
           """
        return self.protocol.sendRequest(Request.RequestIOLock('read'))

    def writeLock(self):
        """Acquire a read-write lock on this filesystem. This is necessary
           to modify anything, but it puts the device into an unusable
           state. Release this lock as soon as possible after you're done.
           """
        return self.protocol.sendRequest(Request.RequestIOLock('write'))

    def unlock(self):
        """Release any locks currently held"""
        return self.protocol.sendRequest(Request.ReleaseIOLock())

    def synchronize(self):
        """Update our local database if necessary. Returns a Deferred
           signalling completion. This has the side-effect of acquiring
           a read lock.
           """
        d = defer.Deferred()
        self.readLock().addCallback(self._startSynchronize, d).addErrback(d.errback)
        return d

    def _startSynchronize(self, retval, d):
        if self.isCacheDirty():
            # Empty and rebuild all database files
            self.cache.empty()
            self.protocol.sendRequest(Request.GetAllFileDetails(
                self._cacheFileDetails)).addCallback(
                self._finishSynchronize, d).addErrback(d.errback)
        else:
            d.callback(None)

    def _finishSynchronize(self, retval, d):
        # Before signalling completion, sync the cache
        self.cache.sync()
        d.callback(None)

    def isCacheDirty(self):
        """This function determines whether our local cache of the database
           is still valid. If not, we should retrieve a new copy.
           """
        # FIXME: this should compare datestamps, or free space on the disk, to
        #        be able to invalidate the cache when the rio is externally modified.
        if len(self.cache.fileDetails) == 0:
            return True
        else:
            return False

    def _cacheFileDetails(self, details):
        """Add a file details dictionary to our database cache. This is used to populate
           the database when synchronizing from the device, and it's used internally
           by updateFileDetails().
           """
        self.cache.fileIdAllocator.allocate(int(details['fid']))
        self.cache.fileDetails[str(details['fid'])] = details

        # FIXME: Report progress near here, at least for synchronization
        print "FID: 0x%05X" % details['fid']

    def updateFileDetails(self, f):
        """Update our cache and the device itself with the latest details dictionary
           from this file object. Returns a Deferred signalling completion.
           """
        self._cacheFileDetails(f.details)
        self.cache.sync()
        return self.protocol.sendRequest(Request.UpdateFileDetails(f.fileID, f.details))


class File:
    """A File represents one media or data file corresponding to an entry in
       the device's database and our copy of that database. File objects
       have lifetimes disjoint from that of the actual files on disk- if a
       file ID is provided, existing metadata is looked up. If an ID isn't
       provided, we generate a new ID and metadata will be uploaded.

       Files are responsible for holding metadata, extracting that metadata
       from real files on disk, and for transferring a file's content to and
       from disk. Since the real work of metadata extraction is done by
       mmpython, this mostly concerns translating mmpython's metadata into
       the Rio's metadata format.
       """
    def __init__(self, filesystem, fileID=None):
        self.filesystem = filesystem

        if fileID is None:
            # New file, start out with barebones metadata
            now = int(time.time())
            self.fileID = self.filesystem.cache.fileIdAllocator.next()
            self.details = {
                'ctime': now,
                'fid_generation': now,
                'fid': self.fileID,
                }
        else:
            # Load this file from the cache
            self.fileID = int(fileID)
            self.details = self.filesystem.cache.fileDetails[str(fileID)]

    def loadFromDisk(self, filename):
        """Load this file's metadata and content from a file on disk.
           Returns a Deferred signalling completion.
           """
        Metadata.Converter().detailsFromDisk(filename, self.details)
        result = defer.Deferred()
        self.loadContentFrom(open(filename, "rb")).addCallback(
            self._updateDetailsAfterLoad, result).addErrback(result.errback)
        return result

    def _updateDetailsAfterLoad(self, retval, result):
        self.filesystem.updateFileDetails(self).chainDeferred(result)

    def saveToDisk(self, filename):
        """Save this file's content to disk. Returns a deferred signalling completion."""
        # We make sure to explicitly close it, so that
        # when this returns you're sure the file is complete.
        result = defer.Deferred()
        dest = open(filename, "wb")
        self.saveContentTo(dest).addCallback(
            self._finishSaveToDisk, dest, result.callback).addErrback(
            self._finishSaveToDisk, dest, result.errback)
        return result

    def _finishSaveToDisk(self, retval, dest, chainTo):
        dest.close()
        chainTo(retval)

    def saveContentTo(self, fileObj, blockSize=8192):
        """Save this file's content to a file-like object.
           Returns a deferred signalling completion.
           """
        return ContentTransfer(Request.ReadFileChunk,
                               self, fileObj, blockSize).result

    def loadContentFrom(self, fileObj, blockSize=8192):
        """Load this file's content from a file-like object.
           Returns a Deferred signalling completion.
           """
        return ContentTransfer(Request.WriteFileChunk,
                               self, fileObj, blockSize).result


class ContentTransfer:
    """This is an object that transfers content, in either direction, between the
       device and a file-like object. The transfer is buffered by Protocol, but
       also throttled so we don't dump the entire file into the request queue
       right away.
       """
    def __init__(self, request, rioFile, fileObj, blockSize):
        self.rioFile = rioFile
        self.request = request
        self.fileObj = fileObj
        self.blockSize = blockSize
        self.offset = 0
        self.remaining = self.rioFile.details['length']
        self.result = defer.Deferred()

        self.next()

    def next(self):
        """Queue up another transfer block"""

        if self.remaining <= 0:
            return

        chunkLen = min(self.remaining, self.blockSize)
        d = self.rioFile.filesystem.protocol.sendRequest(self.request(
            self.rioFile.fileID, self.offset, chunkLen, self.fileObj))
        self.remaining -= chunkLen
        self.offset += chunkLen

        if self.remaining <= 0:
            # This is the last one, chain to our deferred and
            # stop setting up transfers.
            d.addCallback(self.result.callback)
            return

        # FIXME: real progress updates should be triggered here
        d.addCallback(lambda x: sys.stderr.write("."))
        d.addErrback(self.result.errback)

        # Queue up the next block once Protocol thinks we should
        self.rioFile.filesystem.protocol.throttle(self.next)

### The End ###

#!/usr/bin/python
#
# This is an experimental userspace NFS server, using Pinefs, that
# merges several disks into one virtual disk. The result is something like
# RAID, but each individual disk still has a self-contained filesystem.
# This is similar to Unionfs, but works on a higher level and includes
# code to spread new files around the available disks automatically.
#
# Copyright (C) 2005 Micah Dowty
#

import os, sys, stat, statvfs, threading, time
import pinefs.srv
import pinefs.rpc
import pinefs.memfs
import pinefs.fsbase
from pinefs import rfc1094

import psyco
psyco.full()


class DiskSet:
    """This class represents a group of other mounts that a spread
       filesystem can be comprised of. The order the disks are specified
       in controls the order in which we search for existing files. New files
       are created on whichever disk has the most free space.
       """
    def __init__(self, mounts):
        self.mounts = mounts

    def findIter(self, path, criteria=os.path.exists):
        """Iterate over the absolute paths for all copies of 'path' on the DiskSet"""
        if path.startswith("/"):
            path = path[1:]
        for mount in self.mounts:
            p = os.path.abspath(os.path.join(mount, path))
            if p.startswith(mount) and criteria(p):
                yield p

    def find(self, path):
        """Find the first copy of 'path' on the DiskSet"""
        for p in self.findIter(path):
            return p

    def listdir(self, dir):
        """List the contents of a directory, using data from all mounts"""
        contents = {}
        for p in self.findIter(dir, os.path.isdir):
            for f in os.listdir(p):
                contents[f] = 1
        return contents.iterkeys()

    def create(self, path):
        """Find a home for a new file given by 'path'. This creates parent directories
           as necessary and returns the new absolute path where the file should go.
           This reduces the mounts to a set of unique devices, and uses self._mergeStatVfs
           to combine the results.
           """
        raise NotImplementedError

    def statvfs(self):
        """Implements os.statvfs() for this disk set"""
        devices = {}
        total = None
        for mount in self.mounts:

            # Remember the devices for each mount in our diskset, in order
            # to detect and ignore mounts that reside on the same device.
            device = os.stat(mount)[stat.ST_DEV]
            if device in devices:
                continue
            else:
                devices[device] = 1

            vfs = os.statvfs(mount)
            if total:
                total = self._mergeStatVfs(total, vfs)
            else:
                total = vfs
        return total

    def _mergeStatVfs(self, a, b):
        """Merge two statvfs() results"""
        c = list(a)

        # No flags
        c[statvfs.F_FLAG] = 0

        # Block sizes and namemax are the minimum of the two
        for i in (
            statvfs.F_BSIZE,
            statvfs.F_FRSIZE,
            statvfs.F_NAMEMAX,
            ):
            c[i] = min(a[i], b[i])

        # Sum the stats on file nodes
        for i in (
            statvfs.F_FILES,
            statvfs.F_FFREE,
            statvfs.F_FAVAIL,
            ):
            c[i] = a[i] + b[i]

        # All stats measured in blocks are converted to bytes and summed
        for i in (
            statvfs.F_BLOCKS,
            statvfs.F_BFREE,
            statvfs.F_BAVAIL,
            ):
            c[i] = (a[i] * a[statvfs.F_BSIZE] + b[i] * b[statvfs.F_BSIZE]) / c[statvfs.F_BSIZE]

        return tuple(c)


class FileTypeException(Exception):
    """SpreadFile and SpreadDirectory raise this if they're created on the wrong type of file"""
    pass


class SpreadFileBase(pinefs.fsbase.FileObj):
    """A Pinefs file object representing one file or directory on the
       DiskSet. This is an abstract base class, subclasses implement
       files and directories.
       """
    def __init__(self, fs, path, type, size=0):
        self.path = path
        self.fs = fs
        self.type = type
        self.dir = dir
        self.size = size
        self.blocks = (size-1)/self.blocksize+1

        # Register our file ID
        self.fileid = self.fs.handleFactory.next()
        self.fileHandle = self.fs.handleFactory.fmt % self.fileid
        self.fs.handleMap[self.fileHandle] = self

        pinefs.fsbase.FileObj.__init__(self)

    def isValid(self):
        return True

    def read(self, offset, count):
        raise NotImplementedError

    def write(self, offset, data):
        raise NotImplementedError

    def get_dir(self):
        raise NotImplementedError

    def truncate(self):
        raise NotImplementedError

    def mk_link(self, name, fromHandle):
        raise NotImplementedError


class SpreadFile(SpreadFileBase):
    """A Pinefs file object for one file on the diskset"""
    def __init__(self, fs, path=''):

        # Look up this file's real path on our diskset and open it
        self.absPath = fs.diskSet.find(path)
        if not (self.absPath and os.path.isfile(self.absPath)):
            raise FileTypeException
        self.fileStat = os.stat(self.absPath)
        self.openedFile = None

        self.mode = self.fileStat.st_mode
        self.uid = self.fileStat.st_uid
        self.gid = self.fileStat.st_gid

        SpreadFileBase.__init__(self, fs, path, rfc1094.NFREG,
                                size=self.fileStat.st_size)

    def isValid(self):
        return os.path.isfile(self.absPath)

    def read(self, offset, count):
        if not self.openedFile:
            self.open()

        self.openedFile.seek(offset)
        return self.openedFile.read(count)

    def open(self):
        """We have a limited number of total open files.
           If we're about to exceed that, close someone else's file.
           """
        while len(self.fs.openFiles) >= self.fs.maxOpenFiles:
            self.fs.openFiles[0].openedFile.close()
            self.fs.openFiles[0].openedFile = None
            del self.fs.openFiles[0]

        self.openedFile = open(self.absPath, "rb")
        self.fs.openFiles.append(self)


class SpreadDirectory(SpreadFileBase):
    """A Pinefs file object for a directory on the DiskSet"""
    dirCacheLifetime = 5.0

    def __init__(self, fs, path=''):
        self.childCache = {}
        self.dirCache = None
        self.dirCacheExpiration = None
        SpreadFileBase.__init__(self, fs, path, rfc1094.NFDIR)

    def getChild(self, name):
        """Get either a SpreadFile or SpreadDirectory for a child of this object"""

        child = self.childCache.get(name)
        if child and not child.isValid():
            # Invalidate a child that's disappeared
            del self.fs.handleMap[child.fileHandle]
            child = None

        if not child:
            # Create a new file object
            path = os.path.join(self.path, name)
            try:
                child = SpreadFile(self.fs, path)
            except FileTypeException:
                child = SpreadDirectory(self.fs, path)

        self.childCache[name] = child
        return child

    def get_dir(self):
        """Return a directory listing, as a mapping from child names to their file handles"""

        # This is a very common operation for the NFS server
        # to ask us to perform. Cache the results, but since we
        # don't yet have a way of seeing when the underlying
        # filesystem changes we limit the wallclock time in which
        # the cache is valid.
        now = time.time()
        if self.dirCache is not None and now > self.dirCacheExpiration:
            self.dirCache = None

        if self.dirCache is None:
            dir = {}
            for name in self.fs.diskSet.listdir(self.path):
                dir[name] = self.getChild(name).fileHandle
            self.dirCache = dir
            self.dirCacheExpiration = now + self.dirCacheLifetime

        return self.dirCache


class SpreadFilesystem:
    """This is a Pinefs filesystem object implementing our 'spread' filesystem.
       Data is spread across several physical disks. When opening a file, we
       search for it on any of the disks. When creating a new file, it is put
       on whichever disk has the most available space.
       """
    maxOpenFiles = 100

    def __init__(self, diskSet):
        self.diskSet = diskSet
        self.handleFactory = pinefs.fsbase.Ctr()
        self.handleMap = {}
        self.openFiles = []
        self.root = SpreadDirectory(self)

    def mount(self, path):
        """Mount this file system starting at the given path.
           Returns a file object representing the root.
           Currently we only support mounting the filesystem's root.
           """
        if path == '/':
            return self.root.fileHandle

    def get_fil(self, handle):
        """Get a file object, given its handle.
           Returns None if the file handle is stale.
           """
        return self.handleMap.get(handle)

    def create_fil(self, parentHandle, name, **kw):
        raise NotImplementedError

    def rename(self, oldParent, oldName, newParent, newName):
        raise NotImplementedError

    def remove(self, parentHandle, name):
        raise NotImplementedError


class SpreadMountServer(pinefs.srv.MntSrv):
    prog = 100015
    def check_host_ok (self, host, cred, verf):
        return True


class SpreadNfsServer(pinefs.srv.NfsSrv):
    """A subclass of Pinefs's NFS server, with some extras for SpreadFilesystem"""
    prog = 100013

    def check_host_ok (self, host, cred, verf):
        # FIXME: Before read-write support, we should at least restrict
        #        this to hosts on the local submet.
        return True

    def NFSPROC_STATFS(self, fh):
        """Use the DiskSet's statvfs() implementation"""
        vfstat = self.fs.diskSet.statvfs()
        stat = rfc1094.statfsres ()
        stat.status = rfc1094.NFS_OK
        stat._data = stat.info (
            tsize  = 1024 * 16,  # Optimum transfer size
            bsize  = vfstat[statvfs.F_BSIZE],
            blocks = vfstat[statvfs.F_BLOCKS],
            bfree  = vfstat[statvfs.F_BFREE],
            bavail = vfstat[statvfs.F_BAVAIL],
            )
        return stat


if __name__ == "__main__":

    # Get a list of mounts from the command line
    mounts = sys.argv[1:]
    if not mounts:
        print "usage: %s mounts..." % sys.argv[0]
        sys.exit(1)

    fs = SpreadFilesystem(DiskSet(mounts))
    lock = threading.Lock()
    serverClass = pinefs.rpc.UDPServer

    # Create the mount daemon, in a separate thread
    mountServer = SpreadMountServer(fs)
    mountRpc = mountServer.create_transport_server(5565, serverClass, lock=lock)
    mountServer.register(mountRpc)
    mountThread = threading.Thread(target=mountRpc.loop)
    mountThread.start()

    # Create the NFS server itself, in the main thread
    nfsServer = SpreadNfsServer(fs)
    nfsRpc = nfsServer.create_transport_server(2059, serverClass, lock=lock)
    nfsServer.register(nfsRpc)

    # We might listen on a nonstandard program number to avoid interfering
    # with an existing NFS server, but we need to also know about the
    # standard number in order to accept calls to that program after connection.
    nfsRpc.progs[100003] = nfsRpc.progs[nfsServer.prog]

    try:
        nfsRpc.loop()
    finally:
        mountRpc.stop()
        nfsRpc.stop()
        mountRpc.unregister()
        nfsRpc.unregister()

### The End ###

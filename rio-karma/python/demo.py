#!/usr/bin/env python

from twisted.internet import reactor
from twisted.python import log
import RioKarma
import sys


class RioApp:
    def run(self):
        # Oh no, the mighty h4><0rz will break into my rio!
        RioKarma.connect("10.0.0.80", password="orange").addCallback(
            self.connected).addErrback(log.err)
        reactor.run()

    def connected(self, protocol):
        print "Connected successfully"
        self.fs = RioKarma.Filesystem(protocol)
        self.fs.synchronize().addCallback(self.synchronized).addErrback(log.err)

    def synchronized(self, retval):
        print "Synchronized database, %d entries" % len(self.fs.cache.fileDetails)


class Downloader(RioApp):
    def synchronized(self, retval):
        RioApp.synchronized(self, retval)
        self.fileIter = self.fs.cache.fileDetails.iterkeys()
        self.nextFile()

    def nextFile(self, retval=None):
        f = RioKarma.File(self.fs, self.fileIter.next())
        destName = RioKarma.MetadataConverter().filenameFromDetails(f.details)

        print "\n"
        print "          FID: 0x%05X" % f.details.get('fid')
        print "        Title: %s"     % f.details.get('title').encode('ascii', 'replace')
        print "        Album: %s"     % f.details.get('source').encode('ascii', 'replace')
        print "       Artist: %s"     % f.details.get('artist').encode('ascii', 'replace')
        print "       Length: %s"     % f.details.get('length')
        print "  Downloading: %s"     % destName

        f.saveToDisk(destName).addCallback(self.nextFile).addErrback(log.err)


class Uploader(RioApp):
    def synchronized(self, retval):
        RioApp.synchronized(self, retval)
        self.fileIter = iter(sys.argv[1:])

        self.fs.writeLock()
        self.nextFile()

    def nextFile(self, retval=None):
        try:
            filename = self.fileIter.next()
        except StopIteration:
            self.fs.unlock()
            print "Done"
            reactor.stop()
            return

        f = RioKarma.File(self.fs)
        print "\n\nUploading: %s" % filename
        f.loadFromDisk(filename).addCallback(self.nextFile).addErrback(log.err)


if __name__ == "__main__":
    if len(sys.argv) > 1:
        Uploader().run()
    else:
        Downloader().run()

### The End ###



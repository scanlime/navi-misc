#!/usr/bin/env python

from twisted.internet import reactor
from twisted.python import log
from cStringIO import StringIO
import sys, time

import RioKarma


class RioApp:
    def run(self):
        # Oh no, the mighty h4><0rz will break into my rio!
        RioKarma.connect("10.0.0.80", password="orange").addCallback(
            self.connected).addErrback(log.err)
        reactor.run()

    def connected(self, fileManager):
        self.fileManager = fileManager
        print "Connected. %d files in database." % self.fileManager.cache.countFiles()
        self.main()

    def finished(self, retval=None):
        print "\nDone"
        reactor.stop()


class Downloader(RioApp):
    def main(self):
        self.fileIter = iter(self.fileManager.cache.findFiles(type='taxi'))
        self.fileManager.readLock()
        self.nextFile()

    def nextFile(self, retval=None):
        try:
            f = self.fileIter.next()
        except StopIteration:
            self.finished()
            return

        filename = f.suggestFilename()

        print "\n"
        print f.details
        print " -> %s" % filename

        self.fileManager.saveToDisk(f, filename).addCallback(self.nextFile).addErrback(self.error)

    def error(self, e):
        log.err(e)
        self.nextFile()


class Uploader(RioApp):
    def main(self):
        self.fileIter = iter(sys.argv[1:])
        self.fileManager.writeLock()
        self.nextFile()

    def nextFile(self, retval=None):
        try:
            filename = self.fileIter.next()
        except StopIteration:
            print "Releasing lock..."
            self.fileManager.unlock().addCallback(self.finished).addErrback(log.err)
            return

        print "\n\nUploading: %s" % filename

        f = self.fileManager.createFile()
        f.loadMetadataFrom(filename)

        if self.fileManager.cache.findFiles(rid=f.details['rid']):
            print "Already exists on device"
            return self.nextFile()

        self.fileManager.loadFromDisk( f, filename ).addCallback(
            self.nextFile).addCallback(
            self.showSpeed, f, time.time()).addErrback(
            log.err)

    def showSpeed(self, foo, f, startTime):
        now = time.time()
        print f.details
        print "\n%.02f KB/s" % (f.details['length'] / (now - startTime) / 1000.0)


def hexDump(src, dest, bytesPerLine=16, wordSize=2):
    """A pretty standard hex dumper routine.
       Dumps the stream 'src' to the stream 'dest'
       """
    addr = 0
    while 1:
        srcLine = src.read(bytesPerLine)
        if not srcLine:
            break

        # Address
        dest.write("%04X: " % addr)
        addr += len(srcLine)

        # Hex values
        for i in xrange(bytesPerLine):
            if i < len(srcLine):
                dest.write("%02X" % ord(srcLine[i]))
            else:
                dest.write("  ")
            if not (i+1) % wordSize:
                dest.write(" ")
        dest.write(" ")

        # ASCII representation
        for byte in srcLine:
            if ord(byte) >= 32 and ord(byte) < 128:
                dest.write(byte)
            else:
                dest.write(".")
        for i in xrange(bytesPerLine - len(srcLine)):
            dest.write(" ")
        dest.write("\n")


class PlaylistDownloader(RioApp):
    def main(self):
        # This operates entirely out of the cache, no need for locking and such

        for playlist in self.fileManager.listPlaylists():
            print
            print playlist.encode('ascii', 'replace')

            for file in self.fileManager.getPlaylistFiles(playlist):
                print "\t%r" % file


if __name__ == "__main__":
    if len(sys.argv) > 1:
        Uploader().run()
    else:
        #Downloader().run()
        PlaylistDownloader().run()

### The End ###



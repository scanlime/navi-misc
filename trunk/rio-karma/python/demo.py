#!/usr/bin/env python

from twisted.internet import reactor
from twisted.python import log
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
        print "Done"
        reactor.stop()


class Downloader(RioApp):
    def main(self):
        self.fileIter = iter(self.fileManager.cache.findFiles(artist="Zero 7"))
        self.nextFile()

    def nextFile(self, retval=None):
        try:
            f = self.fileIter.next()
        except StopIteration:
            self.finished()
            return

        filename = f.suggestFilename()

        print "\n"
        print f
        print " -> %s" % filename

        self.fileManager.saveToDisk(f, filename).addCallback(self.nextFile).addErrback(log.err)


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
            self.nextFile()

        self.fileManager.loadFromDisk( f, filename ).addCallback(
            self.nextFile).addCallback(
            self.showSpeed, f, time.time()).addErrback(
            log.err)

    def showSpeed(self, foo, f, startTime):
        now = time.time()
        print f.details
        print "\n%.02f KB/s" % (f.details['length'] / (now - startTime) / 1000.0)


if __name__ == "__main__":
    if len(sys.argv) > 1:
        Uploader().run()
    else:
        Downloader().run()

### The End ###



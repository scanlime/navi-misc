#!/usr/bin/env python

from twisted.internet import reactor, defer
from twisted.python import log
from twisted.flow import flow
from cStringIO import StringIO
import sys, time

import RioKarma

#defer.Deferred.debug = True

class RioApp:
    def run(self):
        # Oh no, the mighty h4><0rz will break into my rio!
        RioKarma.connect("10.0.0.80", password="orange").addCallback(
            self.connected).addErrback(self.failed)
        reactor.run()

    def connected(self, fileManager):
        self.fileManager = fileManager
        print "Connected. %d files in database, %.02f/%.02f GB free" % (
            self.fileManager.cache.countFiles(),
            self.fileManager.storageDetails['freeSpace'] / (1024.0 * 1024 * 1024),
            self.fileManager.storageDetails['totalSpace'] / (1024.0 * 1024 * 1024),
            )

        flow.Deferred(self.main()).addCallback(self.finished).addErrback(self.failed)
        self.main()

    def finished(self, retval=None):
        print "\nDone"
        reactor.stop()

    def failed(self, err):
        log.err(err)
        reactor.stop()


class Downloader(RioApp):
    def main(self):
        print "Obtaining read lock..."
        yield self.fileManager.readLock()

        for f in self.fileManager.cache.findFiles(type='taxi'):
            filename = f.suggestFilename()

            print "\n"
            print f.details
            print " -> %s" % filename

            yield self.fileManager.saveToDisk(f, filename)


class Uploader(RioApp):
    def main(self):
        print "Obtaining write lock..."
        yield self.fileManager.writeLock()

        for filename in sys.argv[1:]:
            print "\n\nUploading: %s" % filename

            f = self.fileManager.createFile()
            f.loadMetadataFrom(filename)

            if list(self.fileManager.cache.findFiles(rid=f.details['rid'])):
                print "Already exists on device (rid %r)" % f.details['rid']
            else:
                yield self.fileManager.loadFromDisk( f, filename )


class PlaylistDownloader(RioApp):
    def main(self):
        # This operates entirely out of the cache, no need for locking and such

        self.showPlaylist(self.fileManager.getRootPlaylist(), {})

        #from RioKarma import Metadata
        #c = Metadata.getLocalCache()
        #c.scan("/navi/media/audio/groups")
        #c.close()

        yield defer.succeed(None)

    def showPlaylist(self, f, memo, indentIncrement='    ', indent=''):
        if memo.get(f.id):
            print "%s%s *Already Visited*" % (indent, f)
            return

        memo[f.id] = 1
        print "%s%s:" % (indent, f)
        indent += indentIncrement

        for child in self.fileManager.getPlaylistFiles(f):

            if child.details['type'] == 'playlist':
                self.showPlaylist(child, memo, indentIncrement, indent)
            else:
                print "%s%s" % (indent, child)


class PlaylistUploader(RioApp):
    def main(self):
        print "Obtaining write lock..."
        yield self.fileManager.writeLock()

        root = self.fileManager.getRootPlaylist()

        wobble = self.fileManager.createFile("Wobble")
        yield self.fileManager.setPlaylistFiles(wobble, self.fileManager.cache.findFiles(artist="Zero 7"))

        foo = self.fileManager.createFile("Foo")
        yield self.fileManager.setPlaylistFiles(foo, [wobble])

        boing = self.fileManager.createFile("[Boing]")
        yield self.fileManager.setPlaylistFiles(boing, self.fileManager.cache.findFiles(artist="Kirsty MacColl"))

        bar = self.fileManager.createFile("Taxi Files")
        yield self.fileManager.setPlaylistFiles(bar, self.fileManager.cache.findFiles(type='taxi'))

        yield self.fileManager.setPlaylistFiles(root, [foo, boing, bar])


if __name__ == "__main__":
    if len(sys.argv) > 1:
        Uploader().run()
    else:
        #Downloader().run()
        #PlaylistDownloader().run()
        PlaylistUploader().run()

### The End ###



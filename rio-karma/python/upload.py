#!/usr/bin/env python
#
# This tool recursively crawls a directory structure
# and uploads any files it finds that don't already
# exist on the Rio Karma.
#
#   upload.py [files...]
#

from twisted.internet import reactor, defer
from twisted.python import log
from twisted.flow import flow
import sys, os

import RioKarma
from RioKarma import Progress, Metadata

class RioUploader:
    def __init__(self, roots):
        self.roots = roots

    def run(self):
        self.reporter = Progress.ConsoleReporter()
        RioKarma.autoConnect().addStatusback(
            self.reporter.statusback).addCallback(
            self.connected).addErrback(
            self.failed)
        reactor.run()

    def connected(self, fileManager):
        self.fileManager = fileManager

        print "Scanning files..."
        files = []
        for root in self.roots:
            self.findMediaFiles(root, files, {})
        print "Found %d files to upload" % len(files)

        flow.Deferred(self.upload(files)).addCallback(self.finished).addErrback(self.failed)

    def finished(self, retval=None):
        print "\nDone"
        reactor.stop()

    def failed(self, err):
        log.err(err)
        reactor.stop()

    def upload(self, files):
        print "Obtaining write lock..."
        yield self.fileManager.writeLock()
        print

        for filename in files:
            print filename

            f = self.fileManager.createFile()
            f.loadMetadataFrom(filename)
            yield self.fileManager.loadFromDisk( f, filename ).addStatusback(
                self.reporter.statusback)

    def findMediaFiles(self, top, results, memo):
        """Starting at the given root directory, follow the directory
           tree including symlinks to find all media files that aren't
           already on the device.
           """
        top = os.path.realpath(top)
        if top in memo:
            return
        memo[top] = 1

        for root, dirs, files in os.walk(top):

            for name in files:
                # Is the user trying to kill this? Twisted
                # overrides the usual sigint handler.
                reactor.iterate()
                if not reactor.running:
                    return

                path = os.path.realpath(os.path.join(root, name))
                if path not in memo:
                    memo[path] = 1

                    # This is the first time we've seen the file. Look at its metadata
                    metadata = Metadata.getLocalCache().lookup(path)

                    if metadata.get('type') != 'tune':
                        # This isn't a music file
                        status = "Not Music"

                    elif list(self.fileManager.cache.findFiles(rid=metadata['rid'])):
                        # Already exists on the device
                        status = "Exists"

                    else:
                        # Yay, we need this file
                        results.append(path)
                        status = "Adding"

                    print "[%-10s] %s" % (status, path)

            # Sync the local cache after every directory, to avoid
            # losing a lot of work if this is aborted
            Metadata.getLocalCache().sync()

            for name in dirs:
                # We have to explicitly follow symlinks
                path = os.path.join(root, name)
                if os.path.islink(path):
                    self.findMediaFiles(path, results, memo)


if __name__ == "__main__":
    RioUploader(sys.argv[1:]).run()

### The End ###



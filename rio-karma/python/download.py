#!/usr/bin/env python
#
# This is a simple tool for listing or downloading existing
# songs on the Rio. Usage:
#
#   download.py (list|get) [key=value, key=value...]
#
# The provided metadata keys and their values are used
# as constraints when searching for database entries.
#
# For example, to list all taxi files:
#   download.py list type=taxi
#
# Or to grab all David Bowie music:
#   download.py get artist="David Bowie"
#

from twisted.internet import reactor, defer
from twisted.python import log
from twisted.flow import flow
import sys, os

import RioKarma
from RioKarma import Progress, Metadata

class RioDownloader:
    def __init__(self, action, constraints):
        self.action = action
        self.constraints = constraints

    def run(self):
        self.reporter = Progress.ConsoleReporter()
        RioKarma.autoConnect().addStatusback(
            self.reporter.statusback).addCallback(
            self.connected).addErrback(
            self.failed)
        reactor.run()

    def connected(self, fileManager):
        self.fileManager = fileManager
        flow.Deferred(self.main()).addCallback(self.finished).addErrback(self.failed)

    def finished(self, retval=None):
        print "\nDone"
        reactor.stop()

    def failed(self, err):
        log.err(err)
        reactor.stop()

    def main(self):
        print "Obtaining read lock..."
        yield self.fileManager.readLock()
        print

        for f in self.fileManager.cache.findFiles(**self.constraints):
            filename = f.suggestFilename()

            if self.action == 'get':
                if os.path.isfile(filename):
                    localRid = Metadata.getLocalCache().lookup(filename)['rid']
                    remoteRid = f.details['rid']
                    if localRid == remoteRid:
                        print "%s exists" % f
                        continue
                else:
                    print "%s -> %s" % (f, filename)
                    yield self.fileManager.saveToDisk(f, filename).addStatusback(self.reporter.statusback)

                    # Sync the local cache after every file, to avoid
                    # losing a lot of work if this is aborted
                    Metadata.getLocalCache().sync()

            elif self.action == 'list':
                print f

if __name__ == "__main__":
    action = sys.argv[1]

    # By default, only download audio files (not taxi or playlist files)
    constraints = dict(type='tune')

    for arg in sys.argv[2:]:
        key, value = arg.split("=", 1)
        constraints[key] = value
    RioDownloader(action, constraints).run()

### The End ###



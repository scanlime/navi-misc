#!/usr/bin/env python
#
# This just connects to the Rio Karma and displays basic device info
#

from twisted.internet import reactor
from twisted.python import log
import RioKarma
from RioKarma import Progress

class RioInfoDownloader:
    def run(self):
        self.reporter = Progress.ConsoleReporter()
        RioKarma.autoConnect().addStatusback(
            self.reporter.statusback).addCallback(
            self.connected).addErrback(
            self.failed)
        reactor.run()

    def connected(self, fileManager):
        print "%d files in database" % fileManager.cache.countFiles()
        print "%.02f/%.02f GB free" % (
            fileManager.storageDetails['freeSpace'] / (1024.0 * 1024 * 1024),
            fileManager.storageDetails['totalSpace'] / (1024.0 * 1024 * 1024),
            )

        reactor.stop()

    def failed(self, err):
        log.err(err)
        reactor.stop()

if __name__ == "__main__":
    RioInfoDownloader().run()

### The End ###



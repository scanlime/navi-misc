#!/usr/bin/env python
#
# This tool connects to the Rio Karma and displays the contents of all playlists
#

from twisted.internet import reactor
from twisted.python import log
import RioKarma
from RioKarma import Progress

class RioPlaylistDownloader:
    def run(self):
        self.reporter = Progress.ConsoleReporter()
        RioKarma.autoConnect().addStatusback(
            self.reporter.statusback).addCallback(
            self.connected).addErrback(
            self.failed)
        reactor.run()

    def connected(self, fileManager):
        self.fileManager = fileManager
        self.showPlaylist(self.fileManager.getRootPlaylist(), {})
        reactor.stop()

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

    def failed(self, err):
        log.err(err)
        reactor.stop()

if __name__ == "__main__":
    RioPlaylistDownloader().run()

### The End ###



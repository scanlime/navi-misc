#!/usr/bin/env python

from twisted.internet import protocol, reactor
from twisted.python import log
import RioKarma
import sys


def connected(protocol):
    global fs
    print "Connected successfully"
    fs = RioKarma.Filesystem(protocol)
    fs.synchronize().addCallback(synchronized).addErrback(log.err)

def show(result):
    print "Result: %r" % (result,)

def synchronized(retval):
    print "Synchronized database, %d entries" % len(fs.cache.fileDetails)

    def nextFile(i):
        f = RioKarma.File(fs, i.next())
        destName = RioKarma.MetadataConverter().filenameFromDetails(f.details)

        print
        print
        print "          FID: 0x%05X" % f.details.get('fid')
        print "        Title: %s" % f.details.get('title')
        print "        Album: %s" % f.details.get('source')
        print "       Artist: %s" % f.details.get('artist')
        print "       Length: %s" % f.details.get('length')
        print "  Downloading: %s" % destName

        f.saveToDisk(destName).addCallback(finishFile, i).addErrback(log.err)

    def finishFile(retval, i):
        nextFile(i)

    # Upload files
    for filename in sys.argv[1:]:
        f = RioKarma.File(fs)
        RioKarma.MetadataConverter().detailsFromDisk(filename, f.details)
        print f.details

    # Download files
    nextFile(fs.cache.fileDetails.iterkeys())



# Oh no, the mighty h4><0rz will break into my rio!
RioKarma.connect("10.0.0.80", password="orange").addCallback(
    connected).addErrback(log.err)

reactor.run()

### The End ###



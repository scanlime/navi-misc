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
        length = int(f.details.get('length'))
        destName = f.pickFilename()
        dest = open(destName, "wb")

        print
        print
        print "         Title: %s" % f.details.get('title')
        print "         Album: %s" % f.details.get('source')
        print "        Artist: %s" % f.details.get('artist')
        print "Downloading to: %s" % destName
        print "        Length: %s" % length

        nextChunk(0, f, 0, length, dest, i)

    def nextChunk(readSize, f, offset, length, dest, i):
        sys.stderr.write(".")

        offset += readSize
        length -= readSize
        blockSize = min(length, 64*1024)

        if length > 0:
            fs.protocol.sendRequest(RioKarma.Request_ReadFileChunk(f.fileID, offset, blockSize, dest)).addCallback(
                nextChunk, f, offset, length, dest, i).addErrback(log.err)
        else:
            nextFile(i)

    #for filename in sys.argv[1:]:
        #f = RioKarma.File(fs)
        #f.loadMetadataFrom(filename)
        #print f.details

    nextFile(fs.cache.fileDetails.iterkeys())



# Oh no, the mighty h4><0rz will break into my rio!
RioKarma.connect("10.0.0.80", password="orange").addCallback(
    connected).addErrback(log.err)

reactor.run()

### The End ###



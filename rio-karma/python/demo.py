#!/usr/bin/env python

from twisted.internet import protocol, reactor
from twisted.python import log
import RioKarma


def connected(protocol):
    global fs
    print "Connected successfully"
    fs = RioKarma.Filesystem(protocol)
    fs.synchronize().addCallback(synchronized).addErrback(log.err)

def show(result):
    print "Result: %r" % (result,)

def synchronized(retval):
    print "Synchronized database, %d entries" % len(fs.cache.fileDetails)
    i = fs.cache.fileIdAllocator.next()
    print "new id: %d" % i
    print "details: %r" % fs.cache.fileDetails.get(str(i))


# Oh no, the mighty h4><0rz will break into my rio!
RioKarma.connect("10.0.0.80", password="orange").addCallback(
    connected).addErrback(log.err)

reactor.run()

### The End ###



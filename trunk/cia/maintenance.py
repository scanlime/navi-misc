#!/usr/bin/env python
#
# This is a standalone tool for doing stats pruning, that keeps an
# eye on the UML I/O rate limiter to avoid Bad Stuff.
#

from LibCIA import Stats, Database
import time

Database.init()
cursor = Database.pool.connect().cursor()
m = Stats.Target.Maintenance()

def waitForIO(limit=350000):
    # Make sure we don't drain the I/O bucket too much
    while 1:
       ioStatus = dict([t.split("=") for t in open("/proc/io_status").read().split()])
       if int(ioStatus['io_tokens']) > limit:
           break
       print "Waiting for I/O tokens to refill..."
       time.sleep(10)

# Prime the targetQueue
waitForIO()
print "Pruning the first target"
m.pruneTargets(cursor, 1)

while m.targetQueue:
    waitForIO()
    print m.targetQueue[-1]
    m.pruneTargets(cursor, 1)


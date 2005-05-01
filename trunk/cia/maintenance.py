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

# Prime the targetQueue
m.pruneTargets(cursor, 1)

while m.targetQueue:
    # Make sure we don't drain the I/O bucket too much
    ioStatus = dict([t.split("=") for t in open("/proc/io_status").read().split()])
    while int(ioStatus['io_tokens']) < 100000:
        print "Waiting for I/O tokens to refill..."
        time.sleep(10)
    
    # Prune the next target
    print m.targetQueue[-1]
    m.pruneTargets(cursor, 1)


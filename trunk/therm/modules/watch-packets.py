#!/usr/bin/env python
#
# This is a simple example for the therm_db module. It
# polls the mysql server frequently for new packets, printing
# all data received for the source given on the command line.
#

import therm_db
import time, sys

db = therm_db.defaultDatabase

try:
    name = sys.argv[1]
    source = db.getSource(name)
except (KeyError, IndexError):
    print "Available sources:"
    for source in db.iterSources():
        print "\t%r" % source.name
    sys.exit(1)

packet = source.getLatestPacket()
while 1:
    for packet in source.iterPacketsAfter(packet['id']):
        print " ".join(["%s=%-14s" % i for i in packet.iteritems()])
    time.sleep(0.2)

### The End ###

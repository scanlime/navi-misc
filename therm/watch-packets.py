#!/usr/bin/env python
#
# This is a simple example for the pytherm package. It
# polls the mysql server frequently for new packets, printing
# all data received for the source given on the command line.
#

import pytherm.database
import time, sys

db = pytherm.database.open()

try:
    name = sys.argv[1]
    source = db.getSource(name)
except (KeyError, IndexError):
    print "Available sources:"
    for source in db.iterSources():
        print "\t%r" % source.name
    sys.exit(1)

for packet in source.pollNewPackets():
    print " ".join(["%s=%r" % i for i in packet.iteritems()])

### The End ###

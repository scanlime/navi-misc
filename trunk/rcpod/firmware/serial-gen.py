#!/usr/bin/python
#
# Generate a unique serial number, as a UCS-16 string
# embedded in PIC assembly 'retlw' instructions. This
# is suitable for inclusion in the USB descriptor table.
#

import random, sys

def outputString(s):
    """Output a table containing the given ASCII string"""
    for c in s:
        print "\tretlw\t'%s'" % c
        print "\tretlw\t0x00"

def randString(length, alphabet):
    """Generate a string of the given length using
       random characters from the given alphabet
       """
    return ''.join([random.choice(alphabet) for i in xrange(length)])

serial = randString(16, "0123456789ABCDEF")
sys.stderr.write("Generated serial number: %s\n" % serial)
outputString(serial)

### The End ###

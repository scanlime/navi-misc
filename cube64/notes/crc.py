#!/usr/bin/env python
#
# Implementation of a CRC algorithm compatible with the one used
# for bus reads and writes on N64 controllers. This algorithm was
# created by examining the output of the N64's CRC for several test
# vectors using the process outlined below. No details of Nintendo's
# implementation are available to me.
#
# 1. I first tried using a standard CRC-8 algorithm with every possible
#    polynomial, but none matched the controller's output. This apparently
#    isn't a conventional CRC algorithm.
#
# 2. I first noticed that, for a packet with two bits set, the CRC is
#    the Exclusive NOR of the CRCs for packets with each of the two bits
#    set individually.
#
# 3. This XNOR pattern extends to any number of bits set. Extrapolating
#    from this, it's possible to build a table with inverted CRCs for
#    each possible bit set, and use that to calculate a CRC for any message.
#    The CRC is initialized to 0xFF, then XOR'ed by the table entry for each
#    bit set in the packet.
#

import cPickle, sys


def bin(i, width=8):
    return ''.join([ "01"[i & (1<<(width-1-bit)) != 0]
                     for bit in xrange(width) ])


def crc(packet, table):
    """Table-driven reimplementation of Nintendo's CRC"""
    crc = 0xFF
    for byte in xrange(32):
        for bit in xrange(8):
            if packet[byte] & 1<<(7-bit):
                crc ^= table[byte*8 + bit]
    return crc


def extractTable(vectors):
    """Extract a 256-byte table from the test vectors, that can be used to
       compute any CRC. This table is the inverted CRC generated for every
       possible message with exactly one bit set.
       """
    table = []
    for byte in xrange(32):
        for bit in xrange(8):
            packet = [0] * 32
            packet[byte] = 1<<(7-bit)
            table.append(vectors[tuple(packet)] ^ 0xFF)
    return table


def verifyAlgorithm(vectors, f, *args, **kwargs):
    """Run the given algorithm with test vectors plus the given extra parameters,
       showing the results in a compact form.
       """
    print "Testing algorithm"
    passed = 0
    vectorlist = vectors.keys()
    vectorlist.sort()
    for vector in vectorlist:
        result = f(vector, *args, **kwargs)
        expected = vectors[vector]
        if result == expected:
            passed += 1
        else:
            print "Failed: %s => expected %02X, got %02X" % (
                " ".join(["%02X" % b for b in vector]), expected, result)
    print "%d/%d tests passed" % (passed, len(vectors))


if __name__ == "__main__":
    testVectors = cPickle.load(open("crc_test_vectors.p", "rb"))
    table = extractTable(testVectors)
    verifyAlgorithm(testVectors, crc, table)

### The End ###

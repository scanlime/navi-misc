#!/usr/bin/env python
#
# Patches and hooks for the binary translation of TUT.EXE.
# Micah Dowty <micah@navi.cx>
#

import sbt86

b = sbt86.DOSBinary('TUT.EXE')

# Stub out video_set_cursor_position.
#
# It calls push/pop_ax_bx_cx_dx, which both do strange
# things with the stack. This function doesn't appear to be
# necessary anyway (and we don't implement the BIOS routine
# it calls), so just stub it out.

b.patch('098F:03F2', 'ret')

# One of the video_draw_playfield wrappers calls through a function
# pointer. This pointer only ever has one value, so replace with an
# unconditional jump.

b.patch('098F:1CB2', 'jmp 0x21b3')

# Stub out self-modifying code. This patches the largetext rendering
# function with new font addresses. It isn't needed, since this game
# has only one font and it's already the one present at
# disassembly-time.

b.patch('098F:0D72', 'ret')

# Stub out an equivalent bit of self-modifying code, in the normal
# text rather than largetext rendering code.

b.patch('098F:0D19', 'ret')

# This is a function pointer that appears to be some level-specific or
# game-specific logic. As far as I can tell, it's always set to the
# same value by the main loop, and never touched thereafter.

b.patch('098F:0F89', 'call 0x4E03', 4)

# Replace the game's blitter. The normal blit loop is really large,
# and unnecessary for us. Replace it with a call to
# consoleBlitToScreen(), and read directly from the game's backbuffer.

b.patchAndHook('098F:1730', 'ret', """
    uint16_t backbufferSegment = MEM16(reg.ds, 0x3AD5);
    consoleBlitToScreen(mem + SEG(backbufferSegment,0));
""")

# At 11B5, there's a function pointer which is related to finding
# the object under the cursor, for grabbing it. There are two
# possible targets, 1131 and 1141. Target 1131 is just an "stc"
# instruction.

b.patchAndHook('098F:11B5', 'jmp 0x1141', """
    if (reg.bx == 0x1131) {
        SET_CF;
        return reg;
    }
""")

# At 1271, there's a calculated jump which is used to perform a
# left/right shift by 0 to 7. At this point, SI is the shift value
# (times 2, because it's a word-wide table index) to apply to AL.  0-7
# are right shifts, 9-15 (-7 to 0) are left shifts.  After shifting,
# the code in the jump table branches back to 1273.

b.patchAndHook('098F:1271', 'jmp 0x1273', """
    if (reg.si < 16)
        reg.al >>= reg.si >> 1;
    else
        reg.al <<= 16 - (reg.si >> 1);
""")

# XXX: Text rendering self-modifying code
b.patch('098F:0B7B', 'ret')

b.writeCodeToFile("bt_tutorial.c")

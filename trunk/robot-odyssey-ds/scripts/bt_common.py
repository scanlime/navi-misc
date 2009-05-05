#!/usr/bin/env python
#
# Common binary translation patches and hooks for all Robot Odyssey binaries.
# Micah Dowty <micah@navi.cx>
#

def patch(b):
    """Patches that should be applied to all binaries that have the Robot
       Odyssey game engine. (TUT, GAME, LAB)
       """

    # Don't bother zeroing video memory, we already clear all of RAM.

    b.patch(b.findCode(':9c 33c0 33ff 9d 7206 b90040 f3aa'), 'ret')

    # Remove the copy protection. (This would never work on an
    # emulated floppy controller anyway...) Not all binaries have the
    # copy protection itself (TUT.EXE is unprotected) but they all
    # have a call site just after the DOS error handler is installed.

    b.patch(b.findCode('b425 b024 cd21 1f1e :9a________ 1ff7d0'), 'nop', 5)

    # Stub out video_set_cursor_position.
    #
    # It calls push/pop_ax_bx_cx_dx, which both do strange
    # things with the stack. This function doesn't appear to be
    # necessary anyway (and we don't implement the BIOS routine
    # it calls), so just stub it out.

    b.patch(b.findCode(':e80f00b700b402cd10e81400c3'),
            'ret')

    # One of the video_draw_playfield wrappers calls through a function
    # pointer. This pointer only ever has one value, so replace with an
    # unconditional jump.

    b.patchDynamicBranch(b.findCode('8a1e____8a3e____:ffe3 a0____3c9a7203'), [
            b.findCode(':e8c0ffb91e008b2e____8bf9'),
            ])

    # Stub out self-modifying code. There are two instances of a function
    # which patches the text renderer (one for small text, one for large
    # text) with new addresses depending on the current font. It isn't
    # needed, since this game has only one font and it's already the one
    # present at disassembly-time.

    for addr in b.findCodeMultiple(':b000 8a26____ d1e0 f81226____'
                                   '8b1e____ f813d8 2e891e____ f883db60',
                                   2):
        b.patch(addr, 'ret')

    # This is a function pointer that appears to be some level-specific or
    # game-specific logic. As far as I can tell, it's always set to the
    # same value by the main loop, and never touched thereafter.

    b.patchDynamicBranch(b.findCode('75cc8bcf 880e____ :ff16____ a0____ 3cff'), [
            b.findCode(':8a16____ 80faff 74ef a0____ 32f6'),
            ])

    # Replace the game's blitter. The normal blit loop is really large,
    # and unnecessary for us. Replace it with a call to
    # consoleBlitToScreen(), and read directly from the game's backbuffer.

    b.patchAndHook(b.findCode(':803e____01 7503 e95a05 c43e____'
                              'bb2800 a1____ 8cda 8ed8 be0020 33 c0'),
                   'ret', '''
        hw->drawScreen(proc, proc->memSeg(proc->peek16(r.ds, 0x3AD5)));
    ''')

    # Object intersection tests make use of a callback function that tests
    # whether the object is allowed to be picked up. There are at least
    # three different implementations of this function: a stub, a simple
    # one which tests BX, and a complex one that handles robot grabbing.

    b.patchDynamicBranch(b.findCode('a2____ 0ac0 f8 c3 8a1e____ 8a3e____'
                                    ':ffe3 b500 8a____'), [
            # Required in all binaries
            b.findCode(':8a85____ 3cff 7402 f9c3 f8c3'),               # Normal callback
            b.findCode(':80fafc f5 72__ 80fa__ f5 72__'),              # Robot grabbing
            b.findCode(':f9c3 a1____ a2____ 8826____ e8'),             # Soldering (stub)
            ]
            # In GAME.EXE only?
            + b.findCodeMultiple(':80fa00 740c 80faf0 f5 7308 80faf6') # Non-player bots?
            )

    # At 1271 in TUT.EXE, there's a calculated jump which is used to
    # perform a left/right shift by 0 to 7. At this point, SI is the shift
    # value (times 2, because it's a word-wide table index) to apply to
    # AL.  0-7 are right shifts, 9-15 (-7 to 0) are left shifts.  After
    # shifting, the code in the jump table branches back to 1273 (the next
    # instruction).

    b.patchAndHook(b.findCode('b500 8a0e____ 8bf9 8b1e____ 8a01 247f'
                              '741c 8a1e____ 8a3e____ :ffe3 b500 8a0e____'),
                   'nop', length=2, cCode='''
        if (r.si < 16)
            r.al >>= r.si >> 1;
        else
            r.al <<= 16 - (r.si >> 1);
    ''')

    # The text rendering code is a bit crazy.. The inner loop draws one
    # character, 8x8 pixels, by performing a 16-bit read-modify-write at 8
    # different locations on the framebuffer. Every time we move down to
    # the next line of text, these locations must be patched.
    #
    # We need to mark each of these locations as having dynamic
    # literal values.

    b.patchDynamicLiteral(b.findCode('7203 e9c000 8a9d____ b700d1e3'
                                     '8b87____ 86e0 :260b84fefe 268984fefe'),
                          178)

    # Each of the per-level main loops has code to set the currently
    # viewable room (playfield_room_number) based on the player's location.
    # We may want to override that, so insert a halt() hook there.

    for addr in b.findCodeMultiple('a0ac05 :a25848'):
        b.hook(addr, 'proc->halt(SBTHALT_LOAD_ROOM_ID);')

    # Locate the robot data table, by finding the data itself.
    # The robot data table has two parts: A main table, and a grabber
    # table. We can't treat them all as one chunk, because the grabber
    # table length depends on the number of robots (which could be 3 or 4).

    b.publishAddress('SBTADDR_ROBOT_DATA_MAIN',
                     b.findData(':f0 f0 f1 f1 51 52 53 54 55 56 57 58'
                                ' 00 00 00 00 07 07 07 07 00 00 00 00').offset)
    b.publishAddress('SBTADDR_ROBOT_DATA_GRABBER',
                     b.findData('01 01 01 04 01 02 03'
                                ':00010000 00010000 00010000').offset)

    # Locate the circuit data, by finding a reference to it in the
    # code which draws all wires.

    b.publishAddress('SBTADDR_CIRCUIT_DATA',
                     b.peek16(b.findCode(
                        '80f9ff 744f 32ed 8bf1 8a84:____ 0ac0 7437 8a84____')))

    # Locate the world data. The world data is originally loaded by following
    # a pointer which is statically initialized with the address of the data block.
    # Find that code, and read the pointer it references.

    worldPtr = b.peek16(b.findCode('8bd8 b43f 8b0e____ 8b16:____ cd21 b43e cd21'))
    b.publishAddress('SBTADDR_WORLD_DATA', b.peek16(worldPtr))


def patchChips(b):
    """Patches for binaries that support chips. (LAB and GAME, but not TUT).
       """

    # There are several places where the chip simulator uses
    # self-modifying code which requires dynamic literals. The chip
    # simulator patches itself with destination addresses while
    # writing the result of simulating one gate.

    b.patchDynamicLiteral(b.findCode('e9c000 :8a9d____ b700 d1e3 8b87____'
                                     '86e0 260b84fefe 268984fefe'),
                          192)
    b.patchDynamicLiteral(b.findCode(':b200 32f6 8bfa 8b1e____ 8a01 3c07'),
                          896)
    b.patchDynamicLiteral(b.findCode('8a01 :a2____ 8ad0 32f6 8bfa 8a85____'),
                          3)

    # The chip simulator uses the stack in an interesting way... Any
    # time there is a gate simulation result which can't be written
    # immediately (it affects other gates, not just pins) it's added
    # to the stack. At the end of each nested chip, these results are
    # popped off the stack and written to memory. The results are
    # enqueued by a subroutine, so this subroutine needs to affect the
    # caller's stack, without its own return value getting in the
    # way. It does this by temporarily removing the return value from
    # the stack. We need to patch this code so that our strongly typed
    # stack is okay with this usage.

    b.hook(b.findCode(':58 a3____ 32f68bfa 8b1e'),
           'gStack->preSaveRet();')
    b.hook(b.findCode('8ac2 e8____ b200 a1____ 50 :c3'),
           'gStack->postRestoreRet();')

    b.hook(b.findCode(':58 a3____ a0____ 0ac0 7417'),
           'gStack->preSaveRet();')
    b.hook(b.findCode('75e9 a1____ 50 :c3'),
           'gStack->postRestoreRet();')

    # These seem to only be in LAB.EXE? Maybe chip burning related?

    for addr in b.findCodeMultiple(':8885efef'):
        b.patchDynamicLiteral(addr, 4)
    for addr in b.findCodeMultiple(':8884fefe'):
        b.patchDynamicLiteral(addr, 4)
    for addr in b.findCodeMultiple(':8a84fefe'):
        b.patchDynamicLiteral(addr, 4)


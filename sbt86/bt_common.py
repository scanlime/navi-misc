#!/usr/bin/env python
#
# Common binary translation patches and hooks for all Robot Odyssey binaries.
# Micah Dowty <micah@navi.cx>
#

def patch(b):

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

    b.patch(b.findCode('8a1e____8a3e____:ffe3 a0____3c9a7203'),
            'jmp %s' % b.findCode(':e8c0ffb91e008b2e____8bf9').offset)

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

    b.patch(b.findCode('75cc8bcf 880e____ :ff16____ a0____ 3cff'),
            'call %s' % b.findCode(':8a16____ 80faff 74ef a0____ 32f6').offset,
            4)

    # Replace the game's blitter. The normal blit loop is really large,
    # and unnecessary for us. Replace it with a call to
    # consoleBlitToScreen(), and read directly from the game's backbuffer.

    b.patchAndHook(b.findCode(':803e____01 7503 e95a05 c43e____'
                              'bb2800 a1____ 8cda 8ed8 be0020 33 c0'),
                   'ret', '''
        uint16_t backbufferSegment = M16(reg.ptr.ds[0x3AD5]);
        consoleBlitToScreen(mem + SEG(backbufferSegment,0));
        SDL_Delay(50);
    ''')

    # At 11B5 in TUT.EXE, there's a function pointer which is related to
    # finding the object under the cursor, for grabbing it. There are two
    # possible targets. One is just an 'stc' (1131 in TUT), the other is
    # 't1141' below. (1141 in TUT)

    t1141 = b.findCode(':8a85____ 3cff 7402 f9c3 f8c3').offset
    b.patchAndHook(b.findCode('a2____ 0ac0 f8 c3 8a1e____ 8a3e____'
                              ':ffe3 b500 8a____'),
                   'jmp %s' % t1141, '''
        if (reg.bx != %s) {
            SET_CF;
            return reg;
        }
    ''' % t1141)

    # At 1271 in TUT.EXE, there's a calculated jump which is used to
    # perform a left/right shift by 0 to 7. At this point, SI is the shift
    # value (times 2, because it's a word-wide table index) to apply to
    # AL.  0-7 are right shifts, 9-15 (-7 to 0) are left shifts.  After
    # shifting, the code in the jump table branches back to 1273 (the next
    # instruction).

    b.patchAndHook(b.findCode('b500 8a0e____ 8bf9 8b1e____ 8a01 247f'
                              '741c 8a1e____ 8a3e____ :ffe3 b500 8a0e____'),
                   'nop', length=2, cCode='''
        if (reg.si < 16)
            reg.al >>= reg.si >> 1;
        else
            reg.al <<= 16 - (reg.si >> 1);
    ''')

    # The text rendering code is a bit crazy.. The inner loop draws one
    # character, 8x8 pixels, by performing a 16-bit read-modify-write at 7
    # different locations on the framebuffer. Every time we move down to
    # the next line of text, these locations must be patched.
    #
    # To eliminate the self-modifying code, we patch the patcher function
    # to write its offsets into an array rather than into the code itself,
    # then we patch the code to read this array.

    b.decl('''
        union {
            struct {
                uint8_t low;
                uint8_t high;
            };
            uint16_t word;
        } textOffsets[8];
    ''')

    b.patchAndHook(b.findCode('b500b1008bf1 8a85____ :2e8884____ 2e8884____'
                              'fec0 8b1e____ 8a01 2e8884____ 2e8884____'
                              '478bc6f81518008bf0'),
                   'nop', length=10, cCode='''
        textOffsets[reg.si / 0x18].low = reg.al;
    ''')

    b.patchAndHook(b.findCode('b500b1008bf1 8a85____ 2e8884____ 2e8884____'
                              'fec0 8b1e____ 8a01 :2e8884____ 2e8884____'
                              '478bc6f81518008bf0'),
                   'nop', length=10, cCode='''
        textOffsets[reg.si / 0x18].high = reg.al;
    ''')

    addr = b.findCode('7203 e9c000 8a9d____ b700d1e3'
                      '8b87____ 86e0 :260b84fefe 268984fefe')
    for i in range(8):
        b.patchAndHook(addr, 'nop', length=10, cCode='''
            M16(reg.ptr.es[reg.si + textOffsets[%d].word]) |= reg.ax;
        ''' % i)
        addr = addr.add(0x18)

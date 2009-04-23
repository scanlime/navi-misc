#!/usr/bin/env python
#
# Common binary translation patches and hooks for all Robot Odyssey binaries.
# Micah Dowty <micah@navi.cx>
#

def worldTrace(b):
    """Debugging code: Add a memory trace which catches all writes to
       the in-memory world file. For all known offsets, some
       pretty-printing is applied to the values.
       """

    # Data for the known parts of the world format

    WORLD_BASE = 0x02AC
    WORLD_SIZE = 0x3500

    # NOTE: Especially chatty items are commented out, to make the
    # rest of the changes easier to see by default.

    worldData = [
        (0x02AC, 'w_object_next_in_room', 0x100),
        (0x03AC, 'w_object_sprite_id', 0x100),
#        (0x04AC, 'w_object_color', 0x100),
        (0x05AC, 'w_object_room', 0x100),
#        (0x06AC, 'w_object_x', 0x100),
#        (0x07AC, 'w_object_y', 0x100),
        (0x08AC, 'w_object_moved_by_object', 0x100),
        (0x09AC, 'w_object_moved_by_offset_x', 0x100),
        (0x0AAC, 'w_object_moved_by_offset_y', 0x100),
        (0x0BAC, 'w_object_unk', 0x100),
#        (0x0CAC, 'spr_cursor', 0x10),
        (0x0CBC, 'spr_pin_inout_left', 0x10),
        (0x0CCC, 'spr_pin_inout_right', 0x10),
        (0x0CDC, 'spr_solidblack_1', 0x10),
        (0x0CEC, 'spr_solidblack_2', 0x10),
        (0x0CFC, 'spr_solidblack_3', 0x10),
        (0x0D0C, 'spr_pin_input_down', 0x10),
        (0x0D1C, 'spr_andgate_up', 0x10),
        (0x0D2C, 'spr_andgate_down', 0x10),
        (0x0D3C, 'spr_orgate_up', 0x10),
        (0x0D4C, 'spr_orgate_down', 0x10),
        (0x0D5C, 'spr_xorgate_up', 0x10),
        (0x0D6C, 'spr_xorgate_down', 0x10),
        (0x0D7C, 'spr_notgate_up', 0x10),
        (0x0D8C, 'spr_notgate_down', 0x10),
        (0x0D9C, 'spr_pin_input2_down', 0x10),
        (0x0DAC, 'spr_pin_input2_up', 0x10),
        (0x0DBC, 'spr_pin_input3_down', 0x10),
        (0x0DCC, 'spr_pin_input3_up', 0x10),
        (0x0DDC, 'spr_flipflop_right', 0x10),
        (0x0DEC, 'spr_flipflop_left', 0x10),
        (0x0DFC, 'spr_pin_input_left', 0x10),
        (0x0E0C, 'spr_pin_input_right', 0x10),
        (0x0E1C, 'spr_pin_output_left', 0x10),
        (0x0E2C, 'spr_pin_output_right', 0x10),
        (0x0E3C, 'spr_node', 0x10),
        (0x0E4C, 'spr_blank', 0x10),
        (0x0E5C, 'spr_toolbox', 0x10),
        (0x0E6C, 'spr_solder_iron', 0x10),
        (0x0E7C, 'spr_solder_tip', 0x10),
        (0x0E8C, 'spr_battery_top', 0x10),
        (0x0E9C, 'spr_paintbrush', 0x10),
        (0x0EAC, 'spr_crystal_charger', 0x10),
        (0x0EBC, 'spr_chip_1', 0x10),
        (0x0ECC, 'spr_chip_2', 0x10),
        (0x0EDC, 'spr_chip_3', 0x10),
        (0x0EEC, 'spr_chip_4', 0x10),
        (0x0EFC, 'spr_chip_5', 0x10),
        (0x0F0C, 'spr_chip_6', 0x10),
        (0x0F1C, 'spr_chip_7', 0x10),
        (0x0F2C, 'spr_chip_8', 0x10),
        (0x0F3C, 'spr_chip_blank', 0x10),
        (0x0F4C, 'spr_thruster_right', 0x10),
        (0x0F5C, 'spr_thruster_left', 0x10),
        (0x0F6C, 'spr_thruster_anim1w', 0x10),
        (0x0F7C, 'spr_thruster_anim2w', 0x10),
        (0x0F8C, 'spr_thruster_anim3w', 0x10),
        (0x0F9C, 'spr_thruster_anim4w', 0x10),
        (0x0FAC, 'spr_thruster_switch_closed', 0x10),
        (0x0FBC, 'spr_thruster_switch_open', 0x10),
        (0x0FCC, 'spr_sentry_body', 0x10),
        (0x0FDC, 'spr_keyhole', 0x10),
        (0x0FEC, 'spr_pin_output_up', 0x10),
        (0x0FFC, 'spr_pin_output_down', 0x10),
        (0x100C, 'spr_remote_control', 0x10),
        (0x101C, 'spr_button', 0x10),
        (0x102C, 'spr_key', 0x10),
        (0x103C, 'spr_crystal', 0x10),
        (0x104C, 'spr_grabber_up', 0x10),
        (0x105C, 'spr_grabber_right', 0x10),
        (0x106C, 'spr_grabber_left', 0x10),
        (0x107C, 'spr_grabber_down', 0x10),
        (0x108C, 'spr_unused_1', 0x10),
        (0x109C, 'spr_unused_2', 0x10),
        (0x10AC, 'w_room_obj_list_head', 0x40),
        (0x10EC, 'w_room_bgcolor_table', 0x40),
        (0x112C, 'w_room_fgcolor_table', 0x40),
        (0x116C, 'w_room_link_up', 0x40),
        (0x11AC, 'w_room_link_down', 0x40),
        (0x11EC, 'w_room_link_right', 0x40),
        (0x122C, 'w_room_link_left', 0x40),
        (0x126C, 'w_room_unk_2', 0x40),
        (0x12AC, 'w_room_data', 0x780),
        (0x1A2C, 'w_unk_1', 0x100),
        (0x1B2C, 'w_objtable2_room', 0x80),
        (0x1BAC, 'w_objtable2_x', 0x80),
        (0x1C2C, 'w_objtable2_y', 0x80),
        (0x1CAC, 'byte_11CAC', 0x80),
        (0x1D2C, 'byte_11D2C', 0x80),
        (0x1DAC, 'byte_11DAC', 0x80),
        (0x1E2C, 'byte_11E2C', 0x80),
        (0x1EAC, 'byte_11EAC', 0x18E6),
        (0x3792, 'unk', 0x1A),
        ]

    # Convert the above data into a lookup table that converts
    # a byte offset into information about that byte.

    worldDataLUT = ['{NULL, 0x%x},' % i for i in xrange(WORLD_SIZE)]
    for addr, name, size in worldData:
        for i in xrange(size):
            worldDataLUT[addr - WORLD_BASE + i] = '{"%s", 0x%x},' % (name, i)

    b.decl('''
        #include <stdio.h>
        #define WORLD_BASE 0x%04x
        #define WORLD_SIZE 0x%04x
        static int worldWriteCount;
        static uint8_t worldDataCache[WORLD_SIZE];
        static const struct {
            const char *name;
            int index;
        } worldDataLUT[] = {\n%s\n};
    ''' % (WORLD_BASE, WORLD_SIZE, '\n'.join(worldDataLUT)))

    b.trace('w', '''
        return segment == 0x%x &&
               offset >= WORLD_BASE &&
               offset < (WORLD_BASE + WORLD_SIZE);
    ''' % b.relocSegment, '''
        int w = offset - WORLD_BASE;
        uint8_t byte = mem[SEG(segment,offset)];
        if (byte != worldDataCache[offset] && worldDataLUT[w].name) {
            printf("WORLD: @[%04x:%04x] write %s[%02x] = %02x (%d)\\n",
                   cs, ip, worldDataLUT[w].name, worldDataLUT[w].index,
                   byte, byte);
            worldDataCache[offset] = byte;
            worldWriteCount++;
        }
    ''')

    # Add a frame marker to the output. This is the same blit loop we patch below.
    b.hook(b.findCode(':803e____01 7503 e95a05 c43e____'
                      'bb2800 a1____ 8cda 8ed8 be0020 33 c0'),
           '''
        if (worldWriteCount) {
            worldWriteCount = 0;
            printf("\\n\\n==============================================\\n\\n\\n");
        }
    ''')


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

    b.patchDynamic(b.findCode('8a1e____8a3e____:ffe3 a0____3c9a7203'), [
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

    b.patchDynamic(b.findCode('75cc8bcf 880e____ :ff16____ a0____ 3cff'), [
            b.findCode(':8a16____ 80faff 74ef a0____ 32f6'),
            ])

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

    # Object intersection tests make use of a callback function that tests
    # whether the object is allowed to be picked up. There are at least
    # three different implementations of this function: a stub, a simple
    # one which tests BX, and a complex one that handles robot grabbing.

    b.patchDynamic(b.findCode('a2____ 0ac0 f8 c3 8a1e____ 8a3e____'
                              ':ffe3 b500 8a____'), [
            b.findCode(':8a85____ 3cff 7402 f9c3 f8c3'),             # Normal callback
            b.findCode(':80fafc f5 72__ 80faf6 f5 72__ 80fa09 f5'),  # Robot grabbing
            b.findCode(':f9c3 a1____ a2____ 8826____ e8'),           # Soldering (stub)
            ])

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

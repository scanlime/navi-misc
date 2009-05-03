#!/usr/bin/env python
#
# Tracing tools, for use during binary translation.
# Micah Dowty <micah@navi.cx>
#

def findSelfModifyingCode(b):
    """A debug tool which traces access to the code segment,
       to find unpatched self-modifying code or reads to
       data which may have been located in the code segment.

       Warnings are only printed for addresses that weren't patched
       using patchDynamicLiteral. This tool is designed to supplement
       the static warnings given out by sbt86 itself.
       """

    b.decl("#include <stdio.h>")

    # Declare a table of flags which indicate, for every CS offset,
    # whether the address has been already marked as a dynamic literal.
    b.decl("static uint8_t dynLiteralsMap[] = { %s };" % ''.join([
                "%d," % (i in b.image.dynLiterals)
                for i in xrange(0x10000)
                ]))

    b.trace('r', '''
        return segment == 0x%x && !dynLiteralsMap[offset];
    ''' % b.entryPoint.segment, '''
        printf("XXX: Unpatched READ from code segment offset 0x%04x @[%04x:%04x]\\n",
               offset, cs, ip);
    ''')
    b.trace('w', '''
            return segment == 0x%x && !dynLiteralsMap[offset];
    ''' % b.entryPoint.segment, '''
        printf("XXX: Unpatched WRITE to code segment offset 0x%04x @[%04x:%04x]\\n",
               offset, cs, ip);
    ''')


def worldTrace(b):
    """Debugging code: Add a memory trace which catches all writes to
       the in-memory world file. For all known offsets, some
       pretty-printing is applied to the values.
       """
    structTrace(b, structName='world', traceMode='w',
                structBase=0x02AC, structSize=0x3500,
                itemTable=[

            # Data for the known parts of the world format
            #
            # NOTE: Especially chatty items can be commented out, to
            # make the rest of the changes easier to see.

            (0x02AC, 'w_object_next_in_room', 0x100),
            (0x03AC, 'w_object_sprite_id', 0x100),
            (0x04AC, 'w_object_color', 0x100),
            (0x05AC, 'w_object_room', 0x100),
            (0x06AC, 'w_object_x', 0x100),
            (0x07AC, 'w_object_y', 0x100),
            (0x08AC, 'w_object_moved_by_object', 0x100),
            (0x09AC, 'w_object_moved_by_offset_x', 0x100),
            (0x0AAC, 'w_object_moved_by_offset_y', 0x100),
            (0x0BAC, 'w_object_unk', 0x100),
            (0x0CAC, 'spr_cursor', 0x10),
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
            ])


def structTrace(b, structName, traceMode, structBase, structSize, itemTable):
    """Generic debugging code to trace access to a structure in memory.
       'itemTable' is a list of (addr, name, size) tuples for named
       ranges within the structure. 'traceMode' is the tracing mode (r, w, rw).
       """
    # Generate a lookup table which identifies each byte in the structure
    lut = ['{NULL, 0x%x},' % i for i in xrange(structSize)]
    for addr, name, size in itemTable:
        for i in xrange(size):
            lut[addr - structBase + i] = '{"%s", 0x%x},' % (name, i)

    b.decl('''
        #include <stdio.h>
        static int %s_count;
        static uint8_t %s_dataCache[%s];
        static const struct {
            const char *name;
            int index;
        } %s_lut[] = {\n%s\n};
    ''' % (structName, structName, structSize, structName, '\n'.join(lut)))

    b.trace(traceMode, '''
        return segment == 0x%x && offset >= 0x%x && offset < 0x%x;
    ''' % (b.relocSegment, structBase, structBase + structSize), '''
        int w = offset - 0x%x;
        uint8_t byte = mem[SEG(segment,offset)];
        if (byte != %s_dataCache[offset] && %s_lut[w].name) {
            printf("TRACE[%s]: @[%%04x:%%04x] %%s[%%02x] = %%02x (%%d)\\n",
                   cs, ip, %s_lut[w].name, %s_lut[w].index,
                   byte, byte);
            %s_dataCache[offset] = byte;
            %s_count++;
        }
    ''' % (structBase, structName, structName, structName, structName, structName,
           structName, structName))

    # Add a frame marker to the output. This is the same blit loop we patch below.
    b.hook(b.findCode(':803e____01 7503 e95a05 c43e____'
                      'bb2800 a1____ 8cda 8ed8 be0020 33 c0'),
           '''
        if (%s_count) {
            %s_count = 0;
            printf("\\n\\n== %s ========================================\\n\\n\\n");
        }
    ''' % (structName, structName, structName))

/* -*- Mode: C; c-basic-offset: 4 -*-
 *
 * Hardware emulation and environment support code for sbt86, an
 * experimental 8086 -> C static binary translator.
 *
 * Copyright (c) 2009 Micah Dowty <micah@navi.cx>
 *
 *    Permission is hereby granted, free of charge, to any person
 *    obtaining a copy of this software and associated documentation
 *    files (the "Software"), to deal in the Software without
 *    restriction, including without limitation the rights to use,
 *    copy, modify, merge, publish, distribute, sublicense, and/or sell
 *    copies of the Software, and to permit persons to whom the
 *    Software is furnished to do so, subject to the following
 *    conditions:
 *
 *    The above copyright notice and this permission notice shall be
 *    included in all copies or substantial portions of the Software.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *    OTHER DEALINGS IN THE SOFTWARE.
 */

extern "C" {
#include "sbt86.h"
}

#include <nds.h>
#include <gbfs.h>
#include <stdio.h>

#include "videoConvert.h"
#include "soundEngine.h"

extern const GBFS_FILE data_gbfs;

/*
 * XXX: Use less memory. For now, this is just set to the maximum
 * amount that real-mode code can address.
 */
uint8_t mem[0xFFFF0 + 0xFFFF];
jmp_buf dosExitJump;

static struct {
    uint8_t       port61;
} audio;

static uint16_t keyboardPoll(void);


/*****************************************************************
 * Utilitity functions that are referenced by generated code
 */

void
decompressRLE(uint8_t *dest, uint8_t *src, uint32_t srcLength)
{
    /*
     * Decompress our very simple RLE format. Runs of 2 or more zeroes
     * are replaced with 2 zeroes plus a 16-bit count of the omitted
     * zeroes. We assume the output buffer has already been
     * zero-filled, so the zero runs are simply skipped.
     */

    int zeroes = 0;

    while (srcLength) {
        uint8_t byte = *(src++);
        *(dest++) = byte;
        srcLength--;

        if (byte) {
            zeroes = 0;
        } else {
            zeroes++;

            if (zeroes == 2) {
                zeroes = 0;
                dest += src[0] + (src[1] << 8);
                src += 2;
                srcLength -= 2;
            }
        }
    }
}

void
failedDynamicBranch(uint16_t cs, uint16_t ip, uint32_t value)
{
    iprintf("ERROR: Dynamic branch at %04X:%04X to unsupported value 0x%04x\n",
            cs, ip, value);
    exit(1);
}


/*****************************************************************
 * Emulated DOS, BIOS, and PC hardware.
 */

Regs
interrupt10(Regs reg)
{
    switch (reg.ah) {

    case 0x00: {              /* Set video mode */
        /* Ignore. We're always in CGA mode. */
        break;
    }

    default: {
        iprintf("BIOS10: Unsupported! ax=0x%04x\n", reg.ax);
        break;
    }
    }
    return reg;
}

Regs
interrupt16(Regs reg)
{
    switch (reg.ah) {

    case 0x00: {              /* Get keystroke */
        reg.ax = keyboardPoll();
        break;
    }

    case 0x01: {              /* Check for keystroke */
        uint16_t key = keyboardPoll();
        if (key) {
            CLR_ZF;
            reg.ax = key;
        } else {
            SET_ZF;
        }
        break;
    }

    default: {
        iprintf("BIOS16: Unsupported! ax=0x%04x\n", reg.ax);
        break;
    }
    }
    return reg;
}

Regs
interrupt21(Regs reg)
{
    static int numFiles = 0;
    static struct {
        const uint8_t *data;
        uint32_t len;
    } files[16];

    switch (reg.ah) {

    case 0x06: {              /* Direct console input/output (Only input supported) */
        if (reg.dl == 0xFF) {
            uint16_t key = keyboardPoll();
            if (key) {
                reg.al = (uint8_t) key;
                CLR_ZF;
            } else {
                reg.al = 0;
                SET_ZF;
            }
        }
        break;
    }

    case 0x25: {              /* Set interrupt vector */
        /* Ignored. Robot Odyssey uses this to set the INT 24h error handler. */
        break;
    }

    case 0x2C: {              /* Get system time */
        /*
         * XXX: This is supposed to return wallclock time, but Robot Odyssey's
         *      menu just uses this for calculated delays- so the offset from
         *      real time is not important.
         */
        uint32_t ticks = 0; // XXX NDS
        uint32_t seconds = ticks / 1000;
        uint32_t minutes = seconds / 60;
        uint32_t hours = minutes / 60;
        reg.ch = hours % 24;
        reg.cl = minutes % 60;
        reg.dh = seconds % 60;
        reg.dl = (ticks / 10) % 100;
        break;
    }


    case 0x3D: {              /* Open File */
        int fd = numFiles;
        const char *name = (char*)(mem + SEG(reg.ds, reg.dx));

        iprintf("DOS: Open file %04x:%04x='%s' -> #%d\n",
                reg.ds, reg.dx, name, fd);

        numFiles++;
        files[fd].data = (const uint8_t*) gbfs_get_obj(&data_gbfs, name, &files[fd].len);
        reg.ax = fd;
        CLR_CF;

        if (!files[fd].data) {
            iprintf("Error opening file");
            exit(1);
        }
        break;
    }

    case 0x3E: {              /* Close File */
        iprintf("DOS: Close file #%d\n", reg.bx);
        CLR_CF;
        break;
    }

    case 0x3F: {              /* Read File */
        uint16_t fd = reg.bx;
        uint16_t len = reg.cx;
        void *dest = mem + SEG(reg.ds, reg.dx);

        if (len > files[fd].len) {
            len = files[fd].len;
        }
        memcpy(dest, files[fd].data, len);
        files[fd].data += len;
        files[fd].len -= len;
        reg.ax = len;

        iprintf("DOS: Read %d bytes from file #%d -> %d bytes at %04x:%04x\n",
                reg.cx, fd, reg.ax, reg.ds, reg.dx);
        CLR_CF;
        break;
    }

    case 0x4C: {              /* Exit with return code */
        longjmp(dosExitJump, 0x100 | reg.al);
        break;
    }

    default:
        iprintf("DOS: Unsupported! ax=0x%04x\n", reg.ax);
        break;
    }
    return reg;
}

uint8_t
in(uint16_t port, uint32_t timestamp)
{
    switch (port) {

    case 0x61:    /* PC speaker gate */
        return audio.port61;

    default:
        iprintf("IO: Unimplemented IN %0x04x\n", port);
        return 0;
    }
}

void
out(uint16_t port, uint8_t value, uint32_t timestamp)
{
    switch (port) {

    case 0x43:    /* PIT mode bits */
        /*
         * Ignored. We don't emulate the PIT, we just assume the
         * speaker is always being toggled manually.
         */
        break;

    case 0x61:    /* PC speaker gate */
        if ((value ^ audio.port61) & 2) {
            /*
             * PC speaker state toggled. Store a timestamp.
             */

            SoundEngine::writeSpeakerTimestamp(timestamp);
        }

        audio.port61 = value;
        break;

    default:
        iprintf("IO: Unimplemented OUT 0x%04x, %02x\n", port, value);
    }
}


/*****************************************************************
 * Main Program
 */

extern "C" {
    int tutorial_main(const char *cmdLine);
    int lab_main(const char *cmdLine);
}

int
main(int argc, char **argv)
{
    int retval;

    consoleDemoInit();

    /*
     * We'll be using VRAM banks A and B for a double-buffered framebuffer.
     * Assign them directly to the LCD controller, bypassing the graphics engine.
     */
    vramSetBankA(VRAM_A_LCD);
    vramSetBankB(VRAM_B_LCD);

    iprintf("Robot Odyssey DS\n"
            "(Work In Progress)\n"
            "Micah Dowty <micah@navi.cx>\n"
            "---------------------------\n");

    //retval = lab_main("30");
    retval = tutorial_main("21");

    iprintf("DOS Exit (return code %d)\n", retval);
    return retval;
}


/*****************************************************************
 * Console
 */

static uint16_t
keyboardPoll(void)
{
    unsigned int i;
    static const struct {
        uint16_t key;
        uint16_t code;
    } keyTable[] = {

        { KEY_UP    | KEY_R,  0x4800 | '8' },
        { KEY_DOWN  | KEY_R,  0x5000 | '2' },
        { KEY_LEFT  | KEY_R,  0x4B00 | '4' },
        { KEY_RIGHT | KEY_R,  0x4D00 | '6' },

        { KEY_UP,    0x4800 },
        { KEY_DOWN,  0x5000 },
        { KEY_LEFT,  0x4B00 },
        { KEY_RIGHT, 0x4D00 },

        { KEY_B, ' ' },
        { KEY_A, 'S' },
        { KEY_X, 'C' },
        { KEY_Y, 'T' },
        { KEY_L, 'R' },
    };

    scanKeys();

    for (i = 0; i < (sizeof keyTable / sizeof keyTable[0]); i++) {
        if ((keysHeld() & keyTable[i].key) == keyTable[i].key) {
            return keyTable[i].code;
        }
    }

    return 0;
}

void
consoleBlitToScreen(uint8_t *fb)
{
    static bool toggle;
    int i;

    toggle = !toggle;

    VideoConvert::scaleCGAto256(fb, toggle ? VRAM_A : VRAM_B);

    if (!(keysHeld() & KEY_SELECT)) {
        i = 5;
        while (i--) {
            swiWaitForVBlank();
        }
    }

    videoSetMode(toggle ? MODE_FB0 : MODE_FB1);
}

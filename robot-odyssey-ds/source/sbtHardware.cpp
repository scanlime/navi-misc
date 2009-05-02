/* -*- Mode: C; c-basic-offset: 4 -*-
 *
 * Implementation of basic PC hardware emulation support for SBT86.
 * This file defines the base class implementation for SBTHardware.
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

#include <stdio.h>
#include <string.h>
#include <nds.h>
#include <gbfs.h>
#include "sbt86.h"
#include "videoConvert.h"

extern const GBFS_FILE data_gbfs;


SBTRegs SBTHardware::interrupt10(SBTProcess *proc, SBTRegs reg)
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

SBTRegs SBTHardware::interrupt16(SBTProcess *proc, SBTRegs reg)
{
    proc->halt(1000 + reg.ah);

    switch (reg.ah) {

    case 0x00: {              /* Get keystroke */
        reg.ax = keyboardPoll();
        break;
    }

    case 0x01: {              /* Check for keystroke */
        uint16_t key = keyboardPoll();
        if (key) {
            reg.clearZF();
            reg.ax = key;
        } else {
            reg.setZF();
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

SBTRegs SBTHardware::interrupt21(SBTProcess *proc, SBTRegs reg)
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
                reg.clearZF();
            } else {
                reg.al = 0;
                reg.setZF();
            }
        }
        break;
    }

    case 0x25: {              /* Set interrupt vector */
        /* Ignored. Robot Odyssey uses this to set the INT 24h error handler. */
        break;
    }

    case 0x2C: {              /* Get system time */

        //time_t unixTime = time(NULL);
        static time_t unixTime;
        struct tm* timeStruct = gmtime((const time_t *)&unixTime);
        unixTime++;

        reg.ch = timeStruct->tm_hour;
        reg.cl = timeStruct->tm_min;
        reg.dh = timeStruct->tm_sec;
        reg.dl = 0;

        //iprintf("Clock %04x%04x\n", reg.cx, reg.dx);
        break;
    }

    case 0x3D: {              /* Open File */
        int fd = numFiles;
        const char *name = (char*)(proc->memSeg(reg.ds) + reg.dx);

        iprintf("DOS: Open file %04x:%04x='%s' -> #%d\n",
                reg.ds, reg.dx, name, fd);

        numFiles++;
        files[fd].data = (const uint8_t*) gbfs_get_obj(&data_gbfs, name, &files[fd].len);
        reg.ax = fd;
        reg.clearCF();

        if (!files[fd].data) {
            iprintf("Error opening file\n");
        }
        break;
    }

    case 0x3E: {              /* Close File */
        iprintf("DOS: Close file #%d\n", reg.bx);
        reg.clearCF();
        break;
    }

    case 0x3F: {              /* Read File */
        uint16_t fd = reg.bx;
        uint16_t len = reg.cx;
        void *dest = proc->memSeg(reg.ds) + reg.dx;

        if (len > files[fd].len) {
            len = files[fd].len;
        }
        memcpy(dest, files[fd].data, len);
        files[fd].data += len;
        files[fd].len -= len;
        reg.ax = len;

        iprintf("DOS: Read %d bytes from file #%d -> %d bytes at %04x:%04x\n",
                reg.cx, fd, reg.ax, reg.ds, reg.dx);
        reg.clearCF();
        break;
    }

    case 0x4C: {              /* Exit with return code */
        /* XXX */
        proc->halt(0x100 | reg.al);
        break;
    }

    default:
        iprintf("DOS: Unsupported! ax=0x%04x\n", reg.ax);
        break;
    }
    return reg;
}

uint8_t SBTHardware::in(uint16_t port, uint32_t timestamp)
{
    switch (port) {

    case 0x61:    /* PC speaker gate */
        return port61;

    default:
        iprintf("IO: Unimplemented IN %0x04x\n", port);
        return 0;
    }
}

void SBTHardware::out(uint16_t port, uint8_t value, uint32_t timestamp)
{
    switch (port) {

    case 0x43:    /* PIT mode bits */
        /*
         * Ignored. We don't emulate the PIT, we just assume the
         * speaker is always being toggled manually.
         */
        break;

    case 0x61:    /* PC speaker gate */
        if ((value ^ port61) & 2) {
            /*
             * PC speaker state toggled. Store a timestamp.
             */

            /* XXX */
            //SoundEngine::writeSpeakerTimestamp(timestamp);
        }

        port61 = value;
        break;

    default:
        iprintf("IO: Unimplemented OUT 0x%04x, %02x\n", port, value);
    }
}

void SBTHardware::drawScreen(uint8_t *framebuffer)
{
    static bool toggle;

    toggle = !toggle;

    VideoConvert::scaleCGAto256(framebuffer, toggle ? VRAM_A : VRAM_B);

    if (!(keysHeld() & KEY_SELECT)) {
        int i = 5;
        while (i--) {
            swiWaitForVBlank();
        }
    }

    videoSetMode(toggle ? MODE_FB0 : MODE_FB1);
}

uint16_t SBTHardware::keyboardPoll(void)
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

/* -*- Mode: C; c-basic-offset: 4 -*-
 *
 * Common hardware emulation support. This module is responsible for
 * basic DOS, BIOS, and PC hardware emulation. It provides read-only
 * filesystem access via GBFS, but all video, keyboard, and sound are
 * no-ops. Subclasses must provide specific implementations of all
 * user-visible I/O.
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

#include <string.h>
#include <nds.h>
#include <gbfs.h>
#include "panic.h"
#include "sbt86.h"
#include "hwCommon.h"


void DOSFilesystem::reset()
{
    memset(openFiles, 0, sizeof openFiles);
}

uint16_t DOSFilesystem::open(const char *name)
{
    extern const GBFS_FILE data_gbfs;
    uint16_t fd = allocateFD();
    OpenFile *file = &openFiles[fd];

    file->open = true;
    file->data = (const uint8_t*) gbfs_get_obj(&data_gbfs, name, &file->length);

    if (!file->data) {
        PANIC(SBT86_RT_ERROR, ("Can't open file '%s'\n", name));
    }

    return fd;
}

void DOSFilesystem::close(uint16_t fd)
{
    OpenFile *file = &openFiles[fd];

    sassert(fd < MAX_OPEN_FILES, "Closing an invalid file descriptor");
    sassert(file->open, "Closing a file which is not open");

    openFiles[fd].open = false;
}

uint16_t DOSFilesystem::read(uint16_t fd, void *buffer, uint16_t length)
{
    OpenFile *file = &openFiles[fd];

    sassert(fd < MAX_OPEN_FILES, "Reading an invalid file descriptor");
    sassert(file->open, "Reading a file which is not open");

    if (length > file->length) {
        length = file->length;
    }

    memcpy(buffer, file->data, length);
    file->data += length;
    file->length -= length;

    return length;
}

uint16_t DOSFilesystem::allocateFD()
{
    uint16_t fd = 0;

    while (openFiles[fd].open) {
        fd++;
        if (fd >= MAX_OPEN_FILES) {
            PANIC(SBT86_RT_ERROR, ("Too many open files\n"));
        }
    }

    return fd;
}

void HwCommon::reset()
{
    dosFS.reset();
    port61 = 0;
}

uint8_t HwCommon::in(uint16_t port, uint32_t timestamp)
{
    switch (port) {

    case 0x61:    /* PC speaker gate */
        return port61;

    default:
        PANIC(SBT86_RT_ERROR, ("Unimplemented IN 0x%04x\n", port));
        return 0;
    }
}

void HwCommon::out(uint16_t port, uint8_t value, uint32_t timestamp)
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
            writeSpeakerTimestamp(timestamp);
        }

        port61 = value;
        break;

    default:
        PANIC(SBT86_RT_ERROR, ("Unimplemented OUT 0x%04x, 0x%02x\n", port, value));
    }
}

SBTRegs HwCommon::interrupt10(SBTProcess *proc, SBTRegs reg)
{
    switch (reg.ah) {

    case 0x00:    /* Set video mode */
        /* Ignore. We're always in CGA mode. */
        break;

    default:
        PANIC(SBT86_RT_ERROR, ("Unimplemented BIOS Int10\n"
                               "ax=0x%04x\n", reg.ax));
    }
    return reg;
}

SBTRegs HwCommon::interrupt16(SBTProcess *proc, SBTRegs reg)
{
    switch (reg.ah) {

    case 0x00:                /* Get keystroke */
        pollKeys(proc);
        reg.ax = keycode;
        keycode = 0;
        break;

    case 0x01:                /* Check for keystroke */
        pollKeys(proc);
        if (keycode) {
            reg.clearZF();
            reg.ax = keycode;
        } else {
            reg.setZF();
        }
        break;

    default:
        PANIC(SBT86_RT_ERROR, ("Unimplemented BIOS Int16\n"
                               "ax=0x%04x\n", reg.ax));
    }
    return reg;
}

SBTRegs HwCommon::interrupt21(SBTProcess *proc, SBTRegs reg)
{
    switch (reg.ah) {

    case 0x06:                /* Direct console input/output (Only input supported) */
        if (reg.dl == 0xFF) {
            pollKeys(proc);
            if (keycode) {
                reg.al = (uint8_t) keycode;
                reg.clearZF();
                keycode = 0;
            } else {
                reg.al = 0;
                reg.setZF();
            }
        }
        break;

    case 0x25:                /* Set interrupt vector */
        /* Ignored. Robot Odyssey uses this to set the INT 24h error handler. */
        break;

    case 0x2C:                /* Get system time */
        /*
         * XXX: Currently a stub. This is used by MENU.EXE
         */
        {
            static time_t unixTime;
            struct tm* timeStruct = gmtime((const time_t *)&unixTime);
            unixTime++;

            reg.ch = timeStruct->tm_hour;
            reg.cl = timeStruct->tm_min;
            reg.dh = timeStruct->tm_sec;
            reg.dl = 0;
        }
        break;

    case 0x3D:                /* Open File */
        reg.ax = dosFS.open((char*)(proc->memSeg(reg.ds) + reg.dx));
        reg.clearCF();
        break;

    case 0x3E:                /* Close File */
        dosFS.close(reg.bx);
        break;

    case 0x3F:                /* Read File */
        reg.ax = dosFS.read(reg.bx, proc->memSeg(reg.ds) + reg.dx, reg.cx);
        reg.clearCF();
        break;

    case 0x4C:                /* Exit with return code */
        proc->halt(SBTHALT_DOS_EXIT | reg.al);
        break;

    default:
        PANIC(SBT86_RT_ERROR, ("Unimplemented DOS Int21\n"
                               "ax=0x%04x\n", reg.ax));
    }
    return reg;
}

void HwCommon::drawScreen(SBTProcess *proc, uint8_t *framebuffer) {
    proc->halt(SBTHALT_FRAME_DRAWN);
}

void HwCommon::writeSpeakerTimestamp(uint32_t timestamp) {
    /* Stub. Ignore audio output. */
}

void HwCommon::pressKey(uint8_t ascii, uint8_t scancode) {
    keycode = (scancode << 8) | ascii;
}

void HwCommon::pollKeys(SBTProcess *proc) {
    proc->halt(SBTHALT_KEYBOARD_POLL);
}

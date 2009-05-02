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

#ifndef _SBTHARDWARECOMMON_H_
#define _SBTHARDWARECOMMON_H_

#include "sbt86.h"

/*
 * SBTHaltCode --
 *
 *    Enumeration with halt() values we use with SBTProcess.
 */

enum SBTHaltCode {
    SBTHALT_MASK_CODE    = 0xFFFF0000,
    SBTHALT_MASK_ARG     = 0x0000FFFF,

    SBTHALT_DOS_EXIT     = (1 << 16),    // OR'ed with DOS exit code
    SBTHALT_FRAME_DRAWN  = (2 << 16),    // We just rendered a frame
};


/*
 * DOSFilesystem --
 *
 *    A utility class for implementing DOS filesystem emulation.
 */

class DOSFilesystem
{
 public:
    DOSFilesystem() { reset(); }

    void reset();

    uint16_t open(const char *name);
    void close(uint16_t fd);
    uint16_t read(uint16_t fd, void *buffer, uint16_t length);

 private:
    struct OpenFile {
        bool           open;
        const uint8_t *data;
        uint32_t       length;
    };

    uint16_t allocateFD();

    static const unsigned MAX_OPEN_FILES = 16;
    OpenFile openFiles[MAX_OPEN_FILES];
};


/*
 * SBTHardwareCommon --
 *
 *    Concrete subclass of SBTHardware that provides minimal
 *    implementations of all emulated interrupts and hardware.
 *    This class doesn't know how to do anything user-visible,
 *    but it can handle DOS interrupts and load files.
 */

class SBTHardwareCommon : public SBTHardware
{
 public:
    SBTHardwareCommon() { reset(); }

    virtual void reset();

    /*
     * SBT86 Entry points
     */

    virtual uint8_t in(uint16_t port, uint32_t timestamp);
    virtual void out(uint16_t port, uint8_t value, uint32_t timestamp);

    virtual SBTRegs interrupt10(SBTProcess *proc, SBTRegs reg);
    virtual SBTRegs interrupt16(SBTProcess *proc, SBTRegs reg);
    virtual SBTRegs interrupt21(SBTProcess *proc, SBTRegs reg);

    virtual void drawScreen(SBTProcess *proc, uint8_t *framebuffer);

    /*
     * Entry points useful to the main program
     */

    virtual void pressKey(uint8_t ascii, uint8_t scancode = 0);

 protected:
    DOSFilesystem dosFS;
    uint8_t port61;
    uint16_t keycode;

    virtual void writeSpeakerTimestamp(uint32_t timestamp);
    virtual void pollKeys();
};


#endif // _SBTHARDWARECOMMON_H_

/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * ez4loader.h - Shared definitions for the Wifi EZ4 Loader
 *
 * Copyright (C) 2006 Micah Dowty <micah@navi.cx>
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef _EZ4LOADER_H_
#define _EZ4LOADER_H_

/*
 * FIFO messages
 */
#define IPC_MSG_WIFI_SYNC  0xCAFE0001
#define IPC_MSG_WIFI_INIT  0xCAFE0002

/*
 * Bootstrapping constants. All the critical addresses
 * for (re)booting the DS actually live inside a copy of
 * the NDS header which the BIOS makes near the top of
 * main RAM.
 *
 * We use the original entry point addresses in this header,
 * just as they were intended- but an unused portion of the
 * header (actually part of the game title) is home to
 * a single instruction of executable code which forms
 * a loop that the ARM9 runs in while the ARM7 bootstraps
 * the system.
 */
volatile struct {
    unsigned char gameTitle[8];
    uint32 arm9_loop;                // Actually still part of the title...
                                     //   This is set to BOOT_ARM9_LOOP_INSTRUCTION.
    unsigned char gameCode[4];
    unsigned char makerCode[2];
    unsigned char unitCode;
    unsigned char deviceCode;
    unsigned char cardSize;
    unsigned char cardInfo[10];
    unsigned char flags;
    uint32 arm9_romOffset;
    void* arm9_execAddr;
    void* arm9_copyToAddr;
    uint32 arm9_size;
    uint32 arm7_romOffset;
    void* arm7_execAddr;
    void* arm7_copyToAddr;
    uint32 arm7_size;
    /*
     * There's more to this header, but we don't care about the rest.
     * See http://www.bottledlight.com/ds/index.php/FileFormats/NDSFormat
     */
} *BOOT_NDS_HEADER = (void*) 0x027FFE00;

/*
 * When this is placed at BOOT_NDS_HEADER->arm9_loop (address 0x27FFE08)
 * it becomes a "ldr pc, 0x027FFE24" instruction. This points to
 * BOOT_NDS_HEADER->arm9_execAddr. When arm9_execAddr==&arm9_loop, the
 * ARM9 processor is stuck in this one-instruction infinite loop.
 * As soon as arm9_execAddr is assigned, the ARM9 core will branch
 * to the new address.
 */
#define BOOT_ARM9_LOOP_INSTRUCTION  0xE59FF014

#endif /* _EZ4LOADER_H_ */

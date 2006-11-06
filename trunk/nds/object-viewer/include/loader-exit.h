/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * loader-exit.h - Macros for the ARM7 and ARM9 to facilitate
 *                 exiting to the bootloader when a key combination
 *                 is pressed.
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

#ifndef _LOADER_EXIT_H_
#define _LOADER_EXIT_H_

#include <nds.h>

#ifdef ARM9
#include <nds/arm9/cache.h>
#endif

/*
 * Message passed from ARM9 to ARM7 to trigger a reboot.
 * This is totally arbitrary.
 */
#define IPC_MSG_LOADER_EXIT  0xFA57B007

/*
 * Address for a PassMe-style ARM9 loop
 */
#define LOADER_ARM9_LOOP_ADDRESS  ((vuint32*) &NDSHeader.gameTitle[8])

/*
 * When this is placed at BOOT_ARM9_LOOP_ADDRESS (address 0x27FFE08)
 * it becomes a "ldr pc, 0x027FFE24" instruction. This points to
 * the arm9 execute address in NDSHeader.
 */
#define LOADER_ARM9_LOOP_INSTRUCTION  0xE59FF014

/*
 * The loader has copied and relocated itself
 * to high GBAROM space, and stored its entry
 * point at this fixed address.
 *
 * It is expecting the ARM9 to be in a PassMe-style
 * loop, when the ARM7 is jumped to this entry point.
 *
 * To verify the presence of the loader, we ensure
 * that it's stored both an entry address and the
 * complement of that address.
 *
 * NB: This parameter block is placed in the reserved
 *     area of the NDSHeader, past where ndsmall.bin
 *     stops copying.
 */
struct {
    uint32 entryPoint;
    uint32 entryPointComplement;
} *LoaderParameterBlock = (void*) 0x027FFF84;

/*
 * Call Loader_Exit9() from your ARM9 code when you
 * decide to exit. It will return if there is no loader
 * parameter block. Call Loader_Exit7() in your ARM7's
 * FIFO IRQ handler, when you receive an IPC_MSG_LOADER_EXIT.
 */

#ifdef ARM9
static inline void Loader_Exit9()
{
    if (LoaderParameterBlock->entryPoint !=
	~LoaderParameterBlock->entryPointComplement) {
	return;
    }

    /* Disable IRQs */
    REG_IME = 0;
    REG_IF = 0;

    /* Set up the PassMe loop and the new entry points */
    *LOADER_ARM9_LOOP_ADDRESS = LOADER_ARM9_LOOP_INSTRUCTION;
    NDSHeader.arm9executeAddress = (uint32) LOADER_ARM9_LOOP_ADDRESS;
    NDSHeader.arm7executeAddress = LoaderParameterBlock->entryPoint;

    DC_FlushAll();
    IC_InvalidateAll();

    /* Signal the user code to call Loader_Exit7 */
    REG_IPC_FIFO_TX = IPC_MSG_LOADER_EXIT;

    swiSoftReset();
}
#endif /* ARM9 */

#ifdef ARM7
static inline void Loader_Exit7()
{
    /* Disable IRQs */
    REG_IME = 0;
    REG_IF = 0;

    swiSoftReset();
}
#endif /* ARM7 */

#endif /* _LOADER_EXIT_H_ */

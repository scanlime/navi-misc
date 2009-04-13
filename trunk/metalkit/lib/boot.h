/* -*- Mode: C; c-basic-offset: 3 -*-
 *
 * boot.h - Definitions used by both the bootloader and
 *          the rest of the library. This file must be valid
 *          C and assembly.
 *
 * This file is part of Metalkit, a simple collection of modules for
 * writing software that runs on the bare metal. Get the latest code
 * at http://svn.navi.cx/misc/trunk/metalkit/
 *
 * Copyright (c) 2008-2009 Micah Dowty
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef __BOOT_H__
#define __BOOT_H__

#define BOOT_NULL_SEG       0x00
#define BOOT_CODE_SEG       0x08
#define BOOT_DATA_SEG       0x10
#define BOOT_CODE16_SEG     0x18
#define BOOT_DATA16_SEG     0x20
#define BOOT_LDT_SEG        0x28

#define BOOT_LDT_ENTRIES    1024
#define BOOT_LDT_SIZE       (BOOT_LDT_ENTRIES * 8)

/* Unused real-mode-accessable scratch memory. */
#define BOOT_REALMODE_SCRATCH   0x7C00

/*
 * The bootloader defines an LDT table which can be modified
 * by C code, for loading segments dynamically.
 */
#ifndef ASM
extern unsigned char LDT[BOOT_LDT_SIZE];
#endif

#endif /* __BOOT_H__ */

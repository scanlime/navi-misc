/* -*- Mode: C; c-basic-offset: 3 -*-
 *
 * bios.h - Make real-mode BIOS calls from protected mode.
 *          For simplicity and small size, this implementation
 *          switches back to real-mode rather than using virtual 8086
 *          mode. A v86 mode implementation may be more robust.
 *
 * This file is part of Metalkit, a simple collection of modules for
 * writing software that runs on the bare metal. Get the latest code
 * at http://svn.navi.cx/misc/trunk/metalkit/
 *
 * Copyright (c) 2008 Micah Dowty
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

#ifndef __BIOS_H__
#define __BIOS_H__

#include "types.h"
#include "boot.h"

typedef struct Regs16 {
   /*
    * Subset of segment registers
    */

   uint16  ds;
   uint16  es;

   /*
    * General purpose 16-bit registers, in the order expected by pusha/popa.
    */

   uint16  di;
   uint16  si;
   uint16  bp;
   uint16  sp;

   union {
      uint16  bx;
      struct {
         uint8 bl;
         uint8 bh;
      };
   };
   union {
      uint16  dx;
      struct {
         uint8 dl;
         uint8 dh;
      };
   };
   union {
      uint16  cx;
      struct {
         uint8 cl;
         uint8 ch;
      };
   };
   union {
      uint16  ax;
      struct {
         uint8 al;
         uint8 ah;
      };
   };
} PACKED Regs16;

/*
 * This is the communication area between the real-mode BIOS
 * and protected mode. Parts of it are used internally by this
 * module, but the 'userdata' area is available to the caller.
 */

struct BIOSShared {
   uint8 trampoline[512];
   uint8 stack[4096];
   uint8 stackTop[0];
   uint32 esp;
   struct {
      uint16 limit;
      uint32 base;
   } idtr16, idtr32;
   uint8 userdata[1024];
} PACKED;

#define BIOS_SHARED  ((struct BIOSShared*) BOOT_REALMODE_SCRATCH)

/*
 * Macros for converting between 32-bit and 16-bit near/far pointers.
 */

typedef uint32 far_ptr_t;

#define PTR_32_TO_NEAR(p, seg)   ((uint16)((uint32)(p) - ((seg) << 4)))
#define PTR_NEAR_TO_32(seg, off) ((void*)((((uint32)(seg)) << 4) + ((uint32)(off))))
#define PTR_FAR_TO_32(p)         PTR_NEAR_TO_32(p >> 16, p & 0xFFFF)

/*
 * Public entry point.
 */

void BIOS_Call(uint8 vector, Regs16 *regs);

#endif /* __BIOS_H__ */

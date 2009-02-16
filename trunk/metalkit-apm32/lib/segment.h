/* -*- Mode: C; c-basic-offset: 3 -*-
 *
 * segment.h - Utilities for processor segmentation. This supports
 *             editing segment descriptors in the CPU's LDT. (The GDT
 *             is set up once by the bootloader, and not modified.)
 *
 * This file is part of Metalkit, a simple collection of modules for
 * writing software that runs on the bare metal. Get the latest code
 * at http://svn.navi.cx/misc/trunk/metalkit/
 *
 * Copyright (c) 2009 Micah Dowty
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

#ifndef __SEGMENT_H__
#define __SEGMENT_H__

#include "types.h"
#include "boot.h"

typedef struct SegmentDesc {
   uint64 limit0 : 16;
   uint64 base0 : 16;
   uint64 base16 : 8;
   uint64 accessed : 1;
   uint64 rw : 1;
   uint64 ec : 1;
   uint64 code : 1;
   uint64 nonSystem : 1;
   uint64 dpl : 2;
   uint64 present : 1;
   uint64 limit16 : 4;
   uint64 avl : 1;
   uint64 large : 1;
   uint64 bits32 : 1;
   uint64 pageGranularity : 1;
   uint64 base24 : 8;
} PACKED SegmentDesc;


/*
 * Segment_LDTAlloc --
 *
 *    Allocate an unused LDT segment, returning a 16-bit segment
 *    index. Bit 2 of the index will be set, indicating that the
 *    segment is in the LDT rather than the GDT. Bits 0 and 1 are the
 *    Requested Privilege Level (RPL), initialized to 0 by default.
 *
 *    The returned LDT is permanently allocated.
 *
 *    Consecutive calls to LDTAlloc are guaranteed to allocate
 *    consecutive descriptors.
 */

static inline uint16
Segment_LDTAlloc(void)
{
   extern uint32 LDTAllocNext;
   uint16 seg = LDTAllocNext;
   LDTAllocNext += 8;              // Segment descriptors are 8 bytes long
   return seg + (8 + (1 << 2));    // Skip segment 0, set LDT flag
}


/*
 * Segment_LDTLookup --
 *
 *    Given a 16-bit segment index, look up the corresponding
 *    SegmentDesc in the LDT.
 */

static inline SegmentDesc *
Segment_LDTLookup(uint16 seg)
{
   return (SegmentDesc*) ((uint8*)LDT + (seg & 0xFFF8));
}


#endif /* __SEGMENT_H__ */

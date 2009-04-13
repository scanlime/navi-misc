/* -*- Mode: C; c-basic-offset: 3 -*-
 *
 * vbe.h - Support for the VESA BIOS Extension (VBE) video interface.
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

#ifndef __VBE_H__
#define __VBE_H__

#include "types.h"
#include "bios.h"

#define SIGNATURE_VESA        0x41534556
#define SIGNATURE_VBE2        0x32454256
#define MAX_SUPPORTED_MODES   128

#define VBE_MODEATTR_SUPPORTED       (1 << 0)
#define VBE_MODEATTR_VBE1_2          (1 << 1)
#define VBE_MODEATTR_BIOS_SUPPORTED  (1 << 2)
#define VBE_MODEATTR_COLOR           (1 << 3)
#define VBE_MODEATTR_GRAPHICS        (1 << 4)
#define VBE_MODEATTR_NONVGA          (1 << 5)
#define VBE_MODEATTR_NOBANKED        (1 << 6)
#define VBE_MODEATTR_LINEAR          (1 << 7)
#define VBE_MODEATTR_DOUBLESCAN      (1 << 8)

#define VBE_MODEFLAG_LINEAR          0x4000

#define VBE_MEMTYPE_TEXT             0x00
#define VBE_MEMTYPE_CGA              0x01
#define VBE_MEMTYPE_HGC              0x02
#define VBE_MEMTYPE_EGA              0x03
#define VBE_MEMTYPE_PACKED           0x04
#define VBE_MEMTYPE_DIRECT           0x06


typedef struct {
   uint32       signature;
   union {
      uint16    version;
      struct {
         uint8  verMinor;
         uint8  verMajor;
      };
   };
   far_ptr_t    oemString;
   uint32       capabilities;
   far_ptr_t    videoModes;
   uint16       totalMemory;

   /* VBE 2.0 */
   uint16       oemVersion;
   far_ptr_t    vendorName;
   far_ptr_t    productName;
   far_ptr_t    productRev;
   uint16       vbeAFVersion;
   far_ptr_t    accelModes;
} PACKED VBEControllerInfo;

typedef struct {
   uint16       attributes;
   uint8        winA, winB;
   uint16       granularity;
   uint16       winSize;
   uint16       segmentA, segmentB;
   far_ptr_t    winFunc;
   uint16       bytesPerLine;

   /* OEM mode info */
   uint16       width;
   uint16       height;
   uint8        cellWidth;
   uint8        cellHeight;
   uint8        numPlanes;
   uint8        bitsPerPixel;
   uint8        numBanks;
   uint8        memType;
   uint8        bankSizeKB;
   uint8        pageFit;
   uint8        reserved;

   /* VBE 1.2+ */
   struct {
      uint8     maskSize;
      uint8     fieldPos;
   } red, green, blue, reservedChannel;
   uint8        directColorInfo;

   /* VBE 2.0+ */
   void        *linearAddress;
   void        *offscreenAddress;
   uint16       offscreenSizeKB;
} PACKED VBEModeInfo;

typedef struct {
   VBEControllerInfo cInfo;
   uint8             numModes;
   uint16            modes[MAX_SUPPORTED_MODES];
   struct {
      uint16         mode;
      uint16         flags;
      VBEModeInfo    info;
   } current;
} VBEState;

extern VBEState gVBE;

fastcall Bool VBE_Init();
fastcall void VBE_GetModeInfo(uint16 mode, VBEModeInfo *info);
fastcall void VBE_SetMode(uint16 mode, uint16 modeFlags);

fastcall void VBE_SetStartAddress(int x, int y);
fastcall void VBE_SetPalette(int firstColor, int numColors, uint32 *colors);

fastcall void VBE_InitSimple(int width, int height, int bpp);

#endif /* __VBE_H_ */

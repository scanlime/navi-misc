/* -*- Mode: C; c-basic-offset: 3 -*-
 *
 * vbe.c - Support for the VESA BIOS Extension (VBE) video interface.
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

#include "vbe.h"
#include "console.h"

VBEState gVBE;

/*
 * VBE_Init --
 *
 *    Probe for VBE support, and retrieve information
 *    about the video adapter and its supported modes.
 *
 *    On success, returns TRUE and initializes gVBE.
 *    Returns FALSE if VBE is unsupported.
 */

fastcall Bool
VBE_Init()
{
   VBEState *self = &gVBE;
   Regs reg = {};
   VBEControllerInfo *cInfo = (void*) BIOS_SHARED->userdata;
   uint16 *modes;

   /* Let the BIOS know we support VBE2 */
   cInfo->signature = SIGNATURE_VBE2;

   Console_WriteString("Foo\n");

   /* "Get SuperVGA Information" command */
   reg.ax = 0x4f00;
   reg.di = PTR_32_TO_NEAR(cInfo, 0);
   BIOS_Call(0x10, &reg);
   if (reg.ax != 0x004F) {
      return FALSE;
   }
   Console_WriteString("Bar\n");

   /*
    * Make a copy of the VBEControllerInfo struct itself, and of the
    * mode list. Some VBE implementations place the mode list in
    * temporary memory (the reserved area after cInfo) so we may need
    * to copy it before making the next VBE call.
    */

   memcpy(&self->cInfo, cInfo, sizeof *cInfo);
   modes = PTR_FAR_TO_32(self->cInfo.videoModes);
   self->numModes = 0;
   while (*modes != 0xFFFF) {
      self->modes[self->numModes] = *modes;
      modes++;
      self->numModes++;
   }

   return TRUE;
}


/*
 * VBE_GetModeInfo --
 *
 *    Get information about a particular VBE video mode.
 *    Fills in the provided VBEModeInfo structure.
 */

fastcall void
VBE_GetModeInfo(uint16 mode, VBEModeInfo *info)
{
   Regs reg = {};
   VBEModeInfo *tempInfo = (void*) BIOS_SHARED->userdata;

   memset(tempInfo, 0, sizeof *info);

   reg.ax = 0x4f01;
   reg.cx = mode;
   reg.di = PTR_32_TO_NEAR(tempInfo, 0);
   BIOS_Call(0x10, &reg);

   memcpy(info, tempInfo, sizeof *info);
}


/*
 * VBE_SetMode --
 *
 *    Switch to a VESA BIOS SuperVGA mode.
 *    On return, the gVBE structure will have
 *    information about the new current mode.
 */

fastcall void
VBE_SetMode(uint16 mode, uint16 modeFlags)
{
   VBEState *self = &gVBE;

   self->current.mode = mode;
   self->current.flags = modeFlags;
   VBE_GetModeInfo(mode, &self->current.info);

   Regs reg = {};
   reg.ax = 0x4f02;
   reg.bx = mode | modeFlags;
   BIOS_Call(0x10, &reg);
}


/*
 * VBE_SetStartAddress --
 *
 *    Synchronously change the start address at which the video
 *    adapter will scan out to the monitor.
 */

fastcall void
VBE_SetStartAddress(int x, int y)
{
   Regs reg = {};
   reg.ax = 0x4f07;
   reg.bx = 0x0000;
   reg.cx = x;
   reg.dx = y;
   BIOS_Call(0x10, &reg);
}


/*
 * VBE_SetPalette --
 *
 *    Use the VESA BIOS (not the VGA registers) to update any number
 *    of palette entries.
 *
 *    Note that the entire block of palette entries must fit in the
 *    BIOS_SHARED scratch area. Currently this is 1 kilobyte, which
 *    is exactly the size of a full VBE palette.
 *
 *    Each palette entry is a 32-bit BGRX-format color. By default,
 *    each color component is 6 bits wide.
 */

fastcall void
VBE_SetPalette(int firstColor, int numColors, uint32 *colors)
{
   Regs reg = {};
   uint32 *tempColors = (void*) BIOS_SHARED->userdata;

   memcpy32(tempColors, colors, numColors);

   reg.ax = 0x4f09;
   reg.bx = 0x0000;  // Set palette data
   reg.cx = numColors;
   reg.dx = firstColor;
   reg.di = PTR_32_TO_NEAR(tempColors, 0);

   BIOS_Call(0x10, &reg);
}


/*
 * VBE_InitSimple --
 *
 *    Look for a linear video mode matching the requested
 *    size and depth and switch to it.
 *
 *    If VBE is not supported or the requested mode can't
 *    be found, we panic.
 *
 *    On return, gVBE.current will hold info about the
 *    new mode. In particular, gVBE.current.info.linearAddress
 *    will point to the beginning of framebuffer memory.
 */

fastcall void
VBE_InitSimple(int width, int height, int bpp)
{
   VBEState *self = &gVBE;
   int i;

   if (!VBE_Init()) {
      Console_Panic("VESA BIOS Extensions not available.");
   }

   for (i = 0; i < self->numModes; i++) {
      uint16 mode = self->modes[i];
      VBEModeInfo info;
      const uint32 requiredAttrs = VBE_MODEATTR_SUPPORTED |
                                   VBE_MODEATTR_GRAPHICS |
                                   VBE_MODEATTR_LINEAR;

      VBE_GetModeInfo(mode, &info);

      if ((info.attributes & requiredAttrs) == requiredAttrs &&
          info.width == width &&
          info.height == height &&
          info.bitsPerPixel == bpp) {

         VBE_SetMode(mode, VBE_MODEFLAG_LINEAR);
         return;
      }
   }

   Console_Panic("Can't find the requested video mode.");
}

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

Bool
VBE_Init()
{
   Regs16 reg = {};
   VBEControllerInfo *cInfo = (void*) BIOS_SHARED->userdata;
   uint16 *modes;

   /* Let the BIOS know we support VBE2 */
   cInfo->signature = SIGNATURE_VBE2;

   /* "Get SuperVGA Information" command */
   reg.ax = 0x4f00;
   reg.di = PTR_32_TO_NEAR(cInfo, 0);
   BIOS_Call(0x10, &reg);
   if (reg.ax != 0x004F) {
      return FALSE;
   }

   /*
    * Make a copy of the VBEControllerInfo struct itself, and of the
    * mode list. Some VBE implementations place the mode list in
    * temporary memory (the reserved area after cInfo) so we may need
    * to copy it before making the next VBE call.
    */

   memcpy(&gVBE.cInfo, cInfo, sizeof *cInfo);
   modes = PTR_FAR_TO_32(gVBE.cInfo.videoModes);
   gVBE.numModes = 0;
   while (*modes != 0xFFFF) {
      gVBE.modes[gVBE.numModes] = *modes;
      modes++;
      gVBE.numModes++;
   }

   return TRUE;
}


/*
 * VBE_GetModeInfo --
 *
 *    Get information about a particular VBE video mode.
 *    Fills in the provided VBEModeInfo structure.
 */

void
VBE_GetModeInfo(uint16 mode, VBEModeInfo *info)
{
   Regs16 reg = {};
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
 *
 *    This function fills in
 */

void
VBE_SetMode(uint16 mode, uint16 modeFlags)
{
   gVBE.current.mode = mode;
   gVBE.current.flags = modeFlags;
   VBE_GetModeInfo(mode, &gVBE.current.info);

   Regs16 reg = {};
   reg.ax = 0x4f02;
   reg.bx = mode | modeFlags;
   BIOS_Call(0x10, &reg);
}


/*
 * VBE_InitSimple --
 *
 *    Look for a linear video mode matching the requested
 *    size and depth, switch to it, and return a pointer to
 *    on-screen video memory.
 *
 *    If VBE is not supported or the requested mode can't
 *    be found, we panic.
 *
 *    On return, gVBE.current will hold info about the
 *    new mode. In particular, gVBE.current.info.linearAddress
 *    will point to the beginning of framebuffer memory.
 */

void
VBE_InitSimple(int width, int height, int bpp)
{
   int i;

   if (!VBE_Init()) {
      VGAText_Panic("VESA BIOS Extensions not available.");
   }

   for (i = 0; i < gVBE.numModes; i++) {
      uint16 mode = gVBE.modes[i];
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

   VGAText_Panic("Can't find the requested video mode.");
}

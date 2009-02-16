/* -*- Mode: C; c-basic-offset: 3 -*-
 *
 * Metalkit example: Get basic info about the available VESA BIOS
 * adapter and its modes.
 */

#include "types.h"
#include "console_vga.h"
#include "intr.h"
#include "vbe.h"

int
main(void)
{
   int i;

   ConsoleVGA_Init();
   Intr_Init();
   Intr_SetFaultHandlers(Console_UnhandledFault);
   Console_WriteString("Initializing VBE..\n\n");

   if (!VBE_Init()) {
      Console_Panic("VESA BIOS Extensions not available.");
   }

   Console_Format("Found VBE %x.%02x\n"
                  "\n"
                  " OEM: '%s'\n"
                  " Vendor: '%s'\n"
                  " Product: '%s'\n"
                  " Revision: '%s' \n"
                  "\n"
                  "Found %d video modes:\n",
                  gVBE.cInfo.verMajor,
                  gVBE.cInfo.verMinor,
                  PTR_FAR_TO_32(gVBE.cInfo.oemString),
                  PTR_FAR_TO_32(gVBE.cInfo.vendorName),
                  PTR_FAR_TO_32(gVBE.cInfo.productName),
                  PTR_FAR_TO_32(gVBE.cInfo.productRev),
                  gVBE.numModes);

   for (i = 0; i < gVBE.numModes; i++) {
      uint16 mode = gVBE.modes[i];
      VBEModeInfo info;

      VBE_GetModeInfo(mode, &info);

      Console_Format("Mode 0x%x: %dx%d \tbpp=%d \tattr=%016b\n",
                     mode, info.width, info.height, info.bitsPerPixel, info.attributes);
   }

   Console_Flush();

   return 0;
}

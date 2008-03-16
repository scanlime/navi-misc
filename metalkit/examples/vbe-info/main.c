/* -*- Mode: C; c-basic-offset: 3 -*-
 *
 * Metalkit example: Get basic info about the available VESA BIOS
 * adapter and its modes.
 */

#include "types.h"
#include "vgatext.h"
#include "intr.h"
#include "vbe.h"

int
main(void)
{
   int i;

   Intr_Init();
   Intr_SetFaultHandlers(VGAText_DefaultFaultHandler);
   VGAText_Init();

   VGAText_WriteString("Initializing VBE..\n\n");

   if (!VBE_Init()) {
      VGAText_Panic("VESA BIOS Extensions not available.");
   }

   VGAText_Format("Found VBE %2x.%2x\n"
		  "\n"
		  " OEM: '%s'\n"
		  " Vendor: '%s'\n"
		  " Product: '%s'\n"
		  " Revision: '%s' \n"
		  "\n"
		  "Found %4x video modes:\n",
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

      VGAText_Format("Mode %4x: %4xx%4x bpp=%2x attr=%4x\n",
		     mode, info.width, info.height, info.bitsPerPixel, info.attributes);
   }

   return 0;
}

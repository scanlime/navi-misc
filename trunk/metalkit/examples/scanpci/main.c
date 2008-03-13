/* -*- Mode: C; c-basic-offset: 3 -*-
 *
 * Metalkit example: Scan the PCI bus, and display a list of devices
 * on the screen in VGA text mode.
 */

#include "types.h"
#include "vgatext.h"
#include "pci.h"
#include "intr.h"

int
main(void)
{
   PCIScanState busScan = {};

   Intr_Init();
   Intr_SetFaultHandlers(VGAText_DefaultFaultHandler);

   VGAText_Init();
   VGAText_WriteString("Scanning for PCI devices:\n\n");

   while (PCI_ScanBus(&busScan)) {
      VGAText_Format(" %2x:%2x.%1x  %4x:%4x\n",
                     busScan.addr.bus, busScan.addr.device,
                     busScan.addr.function, busScan.vendorId,
                     busScan.deviceId);
   }
 
   VGAText_WriteString("\nDone.\n");

   return 0;
}

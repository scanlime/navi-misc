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
   VGAText_WriteString("PCI...\n");

   while (PCI_ScanBus(&busScan)) {
      VGAText_WriteChar(' ');
      VGAText_WriteHex(busScan.addr.bus, 2);
      VGAText_WriteChar(':');
      VGAText_WriteHex(busScan.addr.device, 2);
      VGAText_WriteChar('.');
      VGAText_WriteHex(busScan.addr.function, 1);
      VGAText_WriteChar(' ');
      VGAText_WriteHex(busScan.vendorId, 4);
      VGAText_WriteChar(':');
      VGAText_WriteHex(busScan.deviceId, 4);
      VGAText_WriteChar('\n');
   }
 
   VGAText_WriteString("\nDone.\n");

   return 0;
}

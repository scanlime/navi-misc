/* -*- Mode: C; c-basic-offset: 3 -*-
 *
 * Metalkit example: Scan the PCI bus, and display a list of devices
 * on the screen in VGA text mode.
 */

#include "types.h"
#include "console_vga.h"
#include "pci.h"
#include "intr.h"

int
main(void)
{
   PCIScanState busScan = {};

   ConsoleVGA_Init();
   Intr_Init();
   Intr_SetFaultHandlers(Console_UnhandledFault);

   Console_WriteString("Scanning for PCI devices:\n\n");

   while (PCI_ScanBus(&busScan)) {
      Console_Format(" %02x:%02x.%01x  %04x:%04x\n",
                     busScan.addr.bus, busScan.addr.device,
                     busScan.addr.function, busScan.vendorId,
                     busScan.deviceId);
   }

   Console_WriteString("\nDone.\n");
   Console_Flush();
   
   return 0;
}

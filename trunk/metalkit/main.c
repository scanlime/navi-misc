/* -*- Mode: C; c-basic-offset: 3 -*- */

#include "types.h"
#include "vgatext.h"
#include "pci.h"

int
main(void)
{
   int i = 0;
   PCIScanState busScan = {};
   PCIAddress svgaDevice;

   VGAText_Init();

   VGAText_WriteString("Hello 32-bit World!\n\n");

   VGAText_WriteString("Scanning PCI bus:\n\n");   
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

   if (PCI_FindDevice(0x15AD, 0x0405, &svgaDevice)) {
      VGAText_WriteString("\nYou have a VMware SVGA device.\n");
   }

   return 0;
}

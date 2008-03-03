/* -*- Mode: C; c-basic-offset: 3 -*-
 *
 * pci.h - Simple PCI configuration interface. This implementation
 *         only supports type 1 accesses to configuration space,
 *         and it ignores the PCI BIOS.
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

#ifndef __PCI_H__
#define __PCI_H__

#include "types.h"

typedef union PCIConfigSpace {
   uint32 words[16];
   struct {
      uint16 vendorId;
      uint16 deviceId;
      uint16 command;
      uint16 status;
      uint16 revisionId;
      uint8  subclass;
      uint8  classCode;
      uint8  cacheLineSize;
      uint8  latTimer;
      uint8  headerType;
      uint8  BIST;
      uint32 BAR[6];
      uint32 cardbusCIS;
      uint16 subsysVendorId;
      uint16 subsysId;
      uint32 expansionRomAddr;
      uint32 reserved0;
      uint32 reserved1;
      uint8  intrLine;
      uint8  intrPin;
      uint8  minGrant;
      uint8  maxLatency;
   };
} __attribute__ ((__packed__)) PCIConfigSpace;

typedef struct PCIAddress {
   uint8 bus, device, function;
} PCIAddress;

typedef struct PCIScanState {
   uint16     vendorId;
   uint16     deviceId;
   PCIAddress nextAddr;
   PCIAddress addr;
} PCIScanState;

uint32 PCI_ConfigRead32(const PCIAddress *addr, uint16 offset);
uint16 PCI_ConfigRead16(const PCIAddress *addr, uint16 offset);
uint8 PCI_ConfigRead8(const PCIAddress *addr, uint16 offset);
void PCI_ConfigWrite32(const PCIAddress *addr, uint16 offset, uint32 data);
void PCI_ConfigWrite16(const PCIAddress *addr, uint16 offset, uint16 data);
void PCI_ConfigWrite8(const PCIAddress *addr, uint16 offset, uint8 data);

Bool PCI_ScanBus(PCIScanState *state);
Bool PCI_FindDevice(uint16 vendorId, uint16 deviceId, PCIAddress *addrOut);

#endif /* __VGA_TEXT_H__ */

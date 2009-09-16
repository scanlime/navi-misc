/*
 * Main file for 'memhost', the host-side component of the RAM Tracer.
 * This tool configures the hardware and streams captured data to disk.
 *
 * Copyright (C) 2009 Micah Dowty
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdio.h>
#include <stdbool.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

#include "fastftdi.h"
#include "hw_common.h"
#include "hw_trace.h"

/*
 * Private functions
 */

static void patchCAMWrite(FTDIDevice *dev, uint32_t address,
                          uint32_t mask, uint16_t index);


int main(int argc, char **argv)
{
  FTDIDevice dev;
  int err;
  static FILE *patchFile;

  if (argc < 3 || argc > 5) {
     fprintf(stderr, "usage: %s <fpga bitstream> <sysclock mhz> "
             "[<output file> [<patch binary>]]\n"
             " Note that the normal sysclock frequency is 16.756 MHz, and\n"
             " it is necessary to underclock to around 2 MHz for RAM tracing.\n",
             argv[0]);
     return 1;
  }

  err = FTDIDevice_Open(&dev);
  if (err) {
     fprintf(stderr, "USB: Error opening device\n");
     return;
  }

  if (argc >= 5) {
     patchFile = fopen(argv[4], "rb");
     if (!patchFile) {
        perror("opening patch file");
        return 1;
     }
  }

  /*
   * Hardware Setup
   */

  err = FPGAConfig_LoadFile(&dev, argv[1]);
  if (err)
    return 1;

  err = FTDIDevice_SetMode(&dev, FTDI_INTERFACE_A,
                           FTDI_BITMODE_SYNC_FIFO, 0xFF, 0);
  if (err)
    return 1;

  // Set the DSi's oscillator frequency using our FPGA's clock synthesizer
  HW_SetSystemClock(&dev, atof(argv[2]));

  if (patchFile) {
     uint32_t addr = 0x280200;   // arm7
     //uint32_t addr = 0x3fee00;   // arm9

     static uint8_t patch[16384];
     int size, i;

     // XXX: hardcoded two 512-byte patches
     patchCAMWrite(&dev, addr >> 1, 0x1ff >> 1, 0);
     HW_ConfigWrite(&dev, REG_PATCH_OFFSETS + 0, 0x0000 - (addr >> 1));
     patchCAMWrite(&dev, (addr + 0x200) >> 1, 0x1ff >> 1, 1);
     HW_ConfigWrite(&dev, REG_PATCH_OFFSETS + 1, 0x0000 - (addr >> 1));

     size = fread(patch, 1, sizeof patch, patchFile);
     fprintf(stderr, "PATCH: Loaded %d bytes from disk, patching at 0x%08x\n",
             size, addr);

     for (i = 0; i < 8192; i++) {
        uint16_t value = patch[i << 1] | (patch[(i << 1) + 1 ] << 8);
        HW_ConfigWrite(&dev, REG_PATCH_CONTENT + i, value);
     }
  }

  if (argc >= 4)
     HW_TraceToFile(&dev, argv[3]);

  FTDIDevice_Close(&dev);
  return 0;
}


/*
 * patchCAMWrite --
 *
 *    Write an address and mask to a particular patch index in the
 *    FPGA's Content Addressable Memory (CAM). Any '1' bits in the
 *    mask are "dont care" bits. When a burst read address matches
 *    the address and mask, a patch with the specified index is
 *    applied.
 */

static void
patchCAMWrite(FTDIDevice *dev, uint32_t address, uint32_t mask, uint16_t index)
{
   HW_ConfigWrite(dev, REG_CAM_ADDR_LOW, address);
   HW_ConfigWrite(dev, REG_CAM_ADDR_HIGH, address >> 16);
   HW_ConfigWrite(dev, REG_CAM_MASK_LOW, mask);
   HW_ConfigWrite(dev, REG_CAM_MASK_HIGH, mask >> 16);
   HW_ConfigWrite(dev, REG_CAM_INDEX, index);  // Must be last, triggers the write
}

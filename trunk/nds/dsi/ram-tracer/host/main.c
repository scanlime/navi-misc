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
#include <signal.h>
#include <stdlib.h>
#include <string.h>

#include "fastftdi.h"
#include "hw_common.h"
#include "hw_trace.h"
#include "hw_patch.h"


int main(int argc, char **argv)
{
   HWPatch p;
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

   HW_Init(&dev, argv[1]);
   HW_SetSystemClock(&dev, atof(argv[2]));

   HWPatch_Init(&p);
   HWPatch_AllocRegion(&p, 0xffd7bc, 0x20);
   HW_LoadPatch(&dev, &p);

   if (argc >= 4)
      HW_TraceToFile(&dev, argv[3]);

   FTDIDevice_Close(&dev);
   return 0;
}

/*
 * hw_trace.c - Client program for the RAM tracing hardware.
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
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>

#include "hw_trace.h"


/*
 * Private functions
 */

static bool detectOverrun(uint8_t *buffer, int length);
static int readCallback(uint8_t *buffer, int length,
                        FTDIProgressInfo *progress, void *userdata);
static void sigintHandler(int signum);


/*
 * Global data
 */

static FILE *outputFile;
static bool exitRequested;
static bool streamStartFound;


/*
 * HW_TraceToFile --
 *
 *    A very high-level function to trace memory activity to disk,
 *    with progress updates on stderr.
 */

void
HW_TraceToFile(FTDIDevice *dev, const char *filename)
{
   int err;

   outputFile = fopen(filename, "wb");
   if (!outputFile) {
      perror("Error opening output file");
      exit(1);
   }

   /*
    * Drain any junk out of the read buffer and discard it before
    * enabling memory traces.
    */

   HW_ConfigWrite(dev, REG_TRACEFLAGS, 0);
   while (FTDIDevice_ReadByteSync(dev, FTDI_INTERFACE_A, NULL) >= 0);
   HW_ConfigWrite(dev, REG_TRACEFLAGS, TRACEFLAG_WRITES | TRACEFLAG_READS);

   /*
    * Stream the captured data to disk
    */

   signal(SIGINT, sigintHandler);
   err = FTDIDevice_ReadStream(dev, FTDI_INTERFACE_A, readCallback, NULL, 64, 16);
   if (err < 0 && !exitRequested)
      exit(1);

   fclose(outputFile);
   outputFile = NULL;

   fprintf(stderr, "\nCapture ended.\n");
}


static bool
detectOverrun(uint8_t *buffer, int length)
{
   while (length) {
      if (*buffer != 0xFF)
         return false;
      buffer++;
      length--;
   }
   return true;
}


static int
readCallback(uint8_t *buffer, int length, FTDIProgressInfo *progress, void *userdata)
{
   if (length) {
      if (!streamStartFound) {
         /*
          * This is the beginning of the stream. Look for the first flag byte,
          * and skip anything prior to it. This synchronizes us to the first packet.
          *
          * XXX: This shouldn't be necessary, since we flush all buffers before
          *      starting the capture. I don't know whether this is here because
          *      of a bug in the FPGA RTL, this program, or some quirk of the
          *       FTDI chip.
          */

         while (length) {
            if (0x80 & *buffer) {
               streamStartFound = true;
               break;
            }
            length--;
            buffer++;
         }
      }

      /*
       * We don't actually parse the received data, but do check for
       * buffer overflows.  If the hardware buffer overruns, the FPGA
       * will start sending 0xFF bytes as fast as it can.
       */
      if (detectOverrun(buffer, length)) {
         fprintf(stderr,
                 "\n\n"
                 "*** Hardware buffer overrun! ***\n"
                 "The USB bus or PC can't keep up with the incoming "
                 "data. Capture has been aborted.\n"
                 "\n");
         return 1;
      }

      if (fwrite(buffer, length, 1, outputFile) != 1) {
         perror("Write error");
         return 1;
      }
   }

   if (progress)
      fprintf(stderr, "  %3d:%02d [ %9.3f MB captured ] %7.1f kB/s current, "
              "%7.1f kB/s average\r",
              (int)progress->totalTime / 60, (int)progress->totalTime % 60,
              progress->current.totalBytes / (1024.0 * 1024.0),
              progress->currentRate / 1024.0,
              progress->totalRate / 1024.0);

   return exitRequested ? 1 : 0;
}


static void
sigintHandler(int signum)
{
   exitRequested = true;
}

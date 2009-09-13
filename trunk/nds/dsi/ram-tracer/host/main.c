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


/*
 * Hardware configuration registers
 */

#define REG_SYSCLK       0x0000
#define REG_TRACEFLAGS   0x0001

#define TRACEFLAG_READS   (1 << 0)
#define TRACEFLAG_WRITES  (1 << 1)


/*
 * Global data
 */

static FILE *outputFile;
static bool exitRequested;
static bool streamStartFound;


/*
 * Private functions
 */

static bool detectOverrun(uint8_t *buffer, int length);
static int readCallback(uint8_t *buffer, int length,
                        FTDIProgressInfo *progress, void *userdata);
static void sigintHandler(int signum);
static void configWrite(FTDIDevice *dev, uint16_t addr, uint16_t data);
static void setSysClock(FTDIDevice *dev, float mhz);


int main(int argc, char **argv)
{
  FTDIDevice dev;
  int err;

  if (argc != 4) {
     fprintf(stderr, "usage: %s <fpga bitstream> <sysclock mhz> <output file>\n"
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

  outputFile = fopen(argv[3], "wb");
  if (!outputFile) {
     perror("opening output file");
     return 1;
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
  setSysClock(&dev, atof(argv[2]));

  /*
   * Drain any junk out of the read buffer and discard it before
   * enabling memory traces.
   */

  while (FTDIDevice_ReadByteSync(&dev, FTDI_INTERFACE_A, NULL) >= 0);
  configWrite(&dev, REG_TRACEFLAGS, TRACEFLAG_WRITES | TRACEFLAG_READS);

  /*
   * Stream the captured data to disk
   */

  signal(SIGINT, sigintHandler);
  err = FTDIDevice_ReadStream(&dev, FTDI_INTERFACE_A, readCallback, NULL, 64, 16);
  if (err < 0 && !exitRequested)
     return 1;

  /*
   * Cleanup
   */

  FTDIDevice_Close(&dev);
  fclose(outputFile);

  fprintf(stderr, "\nCapture ended.\n");

  return 0;
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


/*
 * configWrite --
 *
 *    Write to a configuration register on the FPGA. These
 *    registers are used for global settings and for storing
 *    RAM patches. Registers are 16 bits wide, and they exist
 *    in a 16-bit virtual address space.
 */

static void
configWrite(FTDIDevice *dev, uint16_t addr, uint16_t data)
{
   /*
    * Note that the hardware supports receiving many config packets
    * per USB packet, but so far we send just one at a time for simplicity.
    */

   uint8_t packet[6];

   // XXX: One padding byte. The FT2232H and/or the FPGA seem to eat the
   //      first byte sometimes?
   packet[0] = 0x00;

   // Pack the data as described in usb_comm.v
   packet[1] = 0x80 | ((addr & 0xC000) >> 12) | ((data & 0xC000) >> 14);
   packet[2] = (addr & 0x3F80) >> 7;
   packet[3] = addr & 0x007F;
   packet[4] = (data & 0x3F80) >> 7;
   packet[5] = data & 0x007F;

   if (FTDIDevice_WriteSync(dev, FTDI_INTERFACE_A, packet, sizeof packet)) {
      perror("Error writing configuration register");
      exit(1);
   }
}


/*
 * setSysClock --
 *
 *    Set the system clock to an approximation of the given frequency, in MHz.
 *    We'll display a message with the actual frequency being set.
 */

static void
setSysClock(FTDIDevice *dev, float mhz)
{
   const double synthStep = 240.0 / 0x80000;
   int regValue = (mhz / synthStep) + 0.5;
   double actual = regValue * synthStep;

   if (regValue > 0xffff) {
      fprintf(stderr, "CLOCK: Frequency %.06f (0x%04x) is out of range\n",
              actual, regValue);
      exit(1);
   }

   fprintf(stderr, "CLOCK: Setting system clock to %.06f MHz (0x%04x)\n",
           actual, regValue);

   configWrite(dev, REG_SYSCLK, regValue);
}

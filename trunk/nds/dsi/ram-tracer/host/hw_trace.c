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
#include <string.h>
#include <assert.h>

#include "hw_trace.h"
#include "memtrace_fmt.h"
#include "iohook_defs.h"
#include "iohook_svc.h"

#define MIN(a,b)  ((a) > (b) ? (b) : (a))

typedef union {
   uint16_t words[IOH_PACKET_LEN / sizeof(uint16_t)];
   struct {
      uint32_t data[7];
      uint32_t footer;
   };
} IOHookBuffer;


/*
 * Private functions
 */

static bool detectOverrun(uint8_t *buffer, int length);
static int readCallback(uint8_t *buffer, int length,
                        FTDIProgressInfo *progress, void *userdata);
static void sigintHandler(int signum);


/*
 * Global data
 *
 * XXX: A lot of this can be cleaned up by passing in userdata to
 *      the USB callback. The only mandatory thing here currently
 *      is exitRequested.
 */

static FILE *outputFile;
static bool useIOHooks;
static bool exitRequested;
static bool streamStartFound;
static uint64_t timestamp;
static uint8_t packetBuf[4];
static int packetBufSize;
static uint32_t lastAddr;
static uint32_t lastReadAddr;
static uint32_t lastWriteAddr;
static uint32_t burstIndex;
static uint8_t ioHookSequence;
static HWPatch *hwPatch;
static uint8_t *ioHookPatch;
static FTDIDevice *hwDev;


/*
 * HW_Trace --
 *
 *    A very high-level function to trace memory activity.
 *    Writes progress to stderr. If 'filename' is non-NULL, writes
 *    the output to disk.
 *
 *    If iohook is true, enables I/O hooks when magic RAM addresses
 *    are written.
 *
 *    If resetDSI is true, resets the DSi's CPUs synchronously with
 *    the beginning of the trace.
 */

void
HW_Trace(FTDIDevice *dev, HWPatch *patch, const char *filename,
         bool iohook, bool resetDSI)
{
   int err;
   uint32_t traceFlags;

   // Blank line between initialization messages and live tracing
   fprintf(stderr, "\n");

   useIOHooks = iohook;
   exitRequested = false;
   timestamp = 0;
   ioHookSequence = 0;
   hwPatch = patch;
   hwDev = dev;

   if (filename) {
      outputFile = fopen(filename, "wb");
      if (!outputFile) {
         perror("Error opening output file");
         exit(1);
      }
   } else {
      outputFile = NULL;
   }

   /*
    * Always trace writes. Trace reads only if we're writing
    * them to disk, not if we're just running I/O hooks.
    */

   traceFlags = TRACEFLAG_WRITES;
   if (outputFile)
      traceFlags |= TRACEFLAG_READS;

   /*
    * Drain any junk out of the read buffer and discard it before
    * enabling memory traces.
    */

   HW_ConfigWrite(dev, REG_TRACEFLAGS, 0);
   if (resetDSI)
      HW_ConfigWrite(dev, REG_POWERFLAGS, POWERFLAG_DSI_RESET);
   while (FTDIDevice_ReadByteSync(dev, FTDI_INTERFACE_A, NULL) >= 0);
   HW_ConfigWrite(dev, REG_TRACEFLAGS, traceFlags);
   if (resetDSI)
      HW_ConfigWrite(dev, REG_POWERFLAGS, 0);

   /*
    * Capture data until we're interrupted.
    *
    * XXX: We'd like to use more than one FTDI packet per transfer, but
    *      currently there's a bug somewhere which is causing us to get copies
    *      of the FTDI packet header embedded in our data. Until that's fixed,
    *      compensate by using lots of concurrent transfers.
    */

   signal(SIGINT, sigintHandler);
   err = FTDIDevice_ReadStream(dev, FTDI_INTERFACE_A, readCallback, NULL, 1, 256);
   if (err < 0 && !exitRequested)
      exit(1);

   if (outputFile) {
      fclose(outputFile);
      outputFile = NULL;
   }

   fprintf(stderr, "\nCapture ended.\n");
}


/*
 * HWTrace_InitIOHookPatch --
 *
 *    Prepare a hardware patch to be used for sending response packets
 *    for the I/O hook. This need only be called when I/O hooks are enabled.
 *
 *    Must be called before the patch hardware is programmed.
 */

void
HWTrace_InitIOHookPatch(HWPatch *patch)
{
   ioHookPatch = HWPatch_AllocRegion(patch, IOH_ADDR, IOH_PACKET_LEN);
}


/*
 * dataError --
 *
 *    Report an error in the received trace data.
 */

static void
dataError(const char *title, const char *description)
{
   HWTrace_HideStatus();
   fprintf(stderr,
           "*** %s! ***\n"
           "%s\n\n",
           title, description);
}


/*
 * ioHookChecksum --
 *
 *    Calculate the checksum of an IOHookBuffer.
 */

static uint8_t
ioHookChecksum(const IOHookBuffer *buf)
{
   uint32_t w =  (buf->data[0] + buf->data[1] + buf->data[2] + buf->data[3] +
                  buf->data[4] + buf->data[5] + buf->data[6]);
   return (w + (w << 8) + (w << 16) + (w << 24)) >> 24;
}


/*
 * ioHookTrace --
 *
 *    Trace handler for I/O Hooks. This is called for every word written
 *    in a burst at the hook address, only while the I/O hook is enabled.
 *    Returns true on success, false on failure.
 */

static bool
ioHookTrace(uint32_t index, uint16_t word)
{
   static IOHookBuffer buf;

   buf.words[index] = word;
   if (index == 15) {
      /*
       * Received a complete I/O Hook burst. Validate it and dispatch it.
       */

      uint8_t calcSum = ioHookChecksum(&buf);
      uint8_t rxSum = buf.footer >> IOH_CHECK_SHIFT;
      uint8_t rxSeq = buf.footer >> IOH_SEQ_SHIFT;
      uint8_t rxSvc = buf.footer >> IOH_SVC_SHIFT;
      uint8_t rxLen = buf.footer >> IOH_LEN_SHIFT;
      uint8_t txLen;

      const char *errDetail = "The received data was corrupted. This could indicate a"
                              " data integrity error in the memory tracer, or a code"
                              " error in the patch.";

      if (calcSum != rxSum) {
         dataError("I/O Hook Checksum Error", errDetail);
         return false;
      }
      if (ioHookSequence != rxSeq) {
         dataError("I/O Hook Sequence Error", errDetail);
         return false;
      }
      if (rxLen > IOH_DATA_LEN) {
         dataError("I/O Hook Data Length Error", errDetail);
         return false;
      }

      // Handle the hook packet. This returns the response length.
      txLen = IOH_HandlePacket(rxSvc, buf.data, rxLen);

      if (txLen) {
         // Build a response packet, and send it to the hardware.
         buf.footer &= (IOH_SEQ_MASK | IOH_SVC_MASK);
         buf.footer |= txLen << IOH_LEN_SHIFT;
         buf.footer |= ioHookChecksum(&buf) << IOH_CHECK_SHIFT;

         memcpy(ioHookPatch, &buf, sizeof buf);
         HW_UpdatePatchRegion(hwDev, hwPatch, ioHookPatch, sizeof buf);
      }

      ioHookSequence++;
   }

   return true;
}


/*
 * parsePacket --
 *
 *    Decode a single received packet. Look for communications errors
 *    and, if applicable, invoke I/O hooks.
 *    Returns true on success, false on failure.
 */

static bool
parsePacket(uint8_t *buffer)
{
   MemPacket packet = MemPacket_FromBytes(buffer);
   MemPacketType type = MemPacket_GetType(packet);
   uint16_t word = MemPacket_RW_Word(packet);

   // Overflow errors are always fatal
   if (MemPacket_IsOverflow(packet)) {
      dataError("Hardware buffer overrun",
                "The USB bus or PC can't keep up with the incoming "
                "data. Capture has been aborted.");
      return false;
   }

   // Complain about serious but non-fatal data errors.
   if (!MemPacket_IsAligned(packet)) {
      dataError("Packet alignment error",
                "A trace packet is not properly aligned. Some USB data "
                "has been dropped or corrupted.");
      return true;
   }
   if (!MemPacket_IsChecksumCorrect(packet)) {
      dataError("Packet checksum error",
                "A trace packet has an incorrect checksum. Some USB data "
                "has been dropped or corrupted.");
      return true;
   }

   timestamp += MemPacket_GetDuration(packet);

   switch (type) {

   case MEMPKT_ADDR:
      lastAddr = MemPacket_GetPayload(packet) << 1;
      burstIndex = 0;
      break;

   case MEMPKT_READ:
      lastReadAddr = lastAddr + (burstIndex << 1);
      burstIndex++;
      break;

   case MEMPKT_WRITE:
      lastWriteAddr = lastAddr + (burstIndex << 1);
      if (useIOHooks && lastAddr == (IOH_ADDR & 0xffffff)) {
         if (!ioHookTrace(burstIndex, word))
            return false;
      }
      burstIndex++;
      break;
   }

   return true;
}


/*
 * parseBlock --
 *
 *    Decode a block of received data.
 *    Returns true on success, false on failure.
 */

static inline bool
parseBlock(uint8_t *buffer, int length)
{
   if (packetBufSize) {
      // Process any partial packet from last time
      int l = MIN(length, sizeof packetBuf - packetBufSize);
      memcpy(packetBuf + packetBufSize, buffer, l);
      buffer += l;
      length -= l;

      if (l + packetBufSize == sizeof packetBuf) {
         // Got a full packet
         if (!parsePacket(packetBuf)) {
            return false;
         }
         packetBufSize = 0;
      } else {
         packetBufSize += l;
      }
   }

   // Process full packets
   while (length >= sizeof(MemPacket)) {
      if (!parsePacket(buffer)) {
         return false;
      }
      length -= sizeof(MemPacket);
      buffer += sizeof(MemPacket);
   }

   // Save any remainder
   if (length) {
      assert(packetBufSize == 0);
      memcpy(packetBuf, buffer, length);
      packetBufSize = length;
   }

   return true;
}


/*
 * readCallback --
 *
 *    Callback from fastftdi, processes one contiguous block of trace
 *    data from the device. This block is sent to disk if we have a
 *    file open, and commands are parsed out of it.
 */

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
       * Write to disk first, so if there's a bug in parseBlock we can
       * use the trace to debug it.
       */
      if (outputFile) {
         if (fwrite(buffer, length, 1, outputFile) != 1) {
            perror("Write error");
            return 1;
         }
      }

      if (!parseBlock(buffer, length)) {
         return 1;
      }
   }

   if (progress)
      fprintf(stderr, "%10.02fs [ %9.3f MB captured ] %7.1f kB/s current, "
              "%7.1f kB/s average - RD:%08x WR:%08x\r",
              timestamp / (double)RAM_CLOCK_HZ,
              progress->current.totalBytes / (1024.0 * 1024.0),
              progress->currentRate / 1024.0,
              progress->totalRate / 1024.0,
              lastReadAddr, lastWriteAddr);

   return exitRequested ? 1 : 0;
}


/*
 * sigintHandler --
 *
 *    SIGINT handler, so we can gracefully exit when the user hits ctrl-C.
 */

static void
sigintHandler(int signum)
{
   exitRequested = true;
}


/*
 * HWTrace_HideStatus --
 *
 *    Temporarily hide (erase) the status line on stderr.
 */

void
HWTrace_HideStatus(void)
{
   char spaces[110];
   memset(spaces, ' ', sizeof spaces);
   spaces[sizeof spaces - 1] = '\0';
   fprintf(stderr, "%s\r", spaces);
}

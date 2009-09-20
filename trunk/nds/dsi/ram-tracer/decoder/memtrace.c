/*
 * memtrace.c - Decoder library for reading memory trace logs.
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

#define _FILE_OFFSET_BITS 64

#include <string.h>
#include <assert.h>
#include "memtrace.h"
#include "memtrace_fmt.h"


/*
 * Local functions
 */

bool MemTraceData(MemTraceState *state, MemOp *op, MemPacket packet,
                  MemTraceResult *result);


/*
 * MemTrace_Open --
 *
 *    Open a binary memory trace log, in the raw format saved by
 *    our logging FPGA. Returns true on success, false on error.
 */

bool
MemTrace_Open(MemTraceState *state, const char *filename)
{
   memset(state, 0, sizeof *state);
   state->file = fopen(filename, "rb");
   return state->file != NULL;
}


/*
 * MemTrace_Close --
 *
 *    Close a trace log, clean up after MemTrace_Open.
 */

void
MemTrace_Close(MemTraceState *state)
{
   fclose(state->file);
   state->file = NULL;
}


/*
 * MemTraceReadBuffered --
 *
 *    Internal function for buffered reads. Should be a little faster
 *    than calling fread repeatedly. Returns true on success, false on
 *    EOF. (Does not differentiate EOF from other errors currently!)
 */

static inline bool
MemTraceReadBuffered(MemTraceState *state, uint8_t *bytes, uint32_t size)
{
   int i;
   uint8_t *src;

   assert(state->fileBufHead <= state->fileBufTail);

   if (size + state->fileBufHead > state->fileBufTail) {
      /* Not enough data in the buffer. */

      size_t result;

      /* Relocate any existing data to the beginning of the buffer */
      state->fileBufTail -= state->fileBufHead;
      memmove(state->fileBuf, state->fileBuf + state->fileBufHead, state->fileBufTail);
      state->fileBufHead = 0;

      /* Fill the rest of the buffer from disk (well, from stdio's buffer) */
      result = fread(state->fileBuf + state->fileBufTail, 1,
                     sizeof state->fileBuf - state->fileBufTail,
                     state->file);
      if (result < 1) {
         /* Nothing to read */
         return false;
      }
      state->fileBufTail += result;
   }

   if (size + state->fileBufHead > state->fileBufTail) {
      /* We read something, but not enough. EOF. */
      return false;
   }

   /*
    * This is such a small copy that the function call overhead on
    * memcpy isn't likely to make it worthwhile. Also, this loop can
    * be unrolled by a smart compiler when 'size' is constant.
    */

   src = state->fileBuf + state->fileBufHead;
   for (i = 0; i < size; i++) {
      *(bytes++) = *(src++);
   }
   state->fileBufHead += size;
   state->fileOffset += size;
   assert(state->fileBufHead <= state->fileBufTail);

   return true;
}



/*
 * MemTrace --
 *
 *    Advance to the next memory operation in the log.
 *    The current timestamp and memory contents in 'state'
 *    are updated. If 'nextOp' is not NULL, it is filled in
 *    with details about this memory operation.
 *
 *    Returns a MemTraceResult which can indicate success,
 *    end of file, or error. On EOF or error, no operation is
 *    written to 'nextOp'.
 */

MemTraceResult
MemTrace_Next(MemTraceState *state, MemOp *nextOp)
{
   /*
    * We can read any number of packets from the file.
    * As soon as we've found a single read/write burst, we're done.
    */
   MemOp op;
   bool done = false;
   MemTraceResult result = MEMTR_SUCCESS;

   op.length = 0;
   op.type = MEMOP_INVALID;

   while (!done) {
      MemPacket packet;
      uint8_t packetBytes[sizeof packet];

      if (!MemTraceReadBuffered(state, packetBytes, sizeof packetBytes)) {
         return MEMTR_EOF;
      }
      packet = MemPacket_FromBytes(packetBytes);

      if (!MemPacket_IsAligned(packet)) {
         // Half-hearted attempt to recover from sync errors.
         // We could do better than this...
         if (!MemTraceReadBuffered(state, packetBytes, 1)) {
            return MEMTR_EOF;
         }

         return MEMTR_ERR_SYNC;
      }

      if (!MemPacket_IsChecksumCorrect(packet)) {
         return MEMTR_ERR_CHECKSUM;
      }

      state->timestamp.clocks += MemPacket_GetDuration(packet);
      state->timestamp.seconds = state->timestamp.clocks / (double)RAM_CLOCK_HZ;

      switch (MemPacket_GetType(packet)) {

      case MEMPKT_ADDR:
         // Addresses end this burst, but we store the address for next time.
         state->nextAddr = MemPacket_GetPayload(packet);
         if (op.length) {
            done = true;
         }
         break;

      case MEMPKT_READ:
         if (op.type == MEMOP_WRITE) {
            return MEMTR_ERR_BADBURST;
         }
         op.type = MEMOP_READ;
         done = MemTraceData(state, &op, packet, &result);
         break;

      case MEMPKT_WRITE:
         if (op.type == MEMOP_READ) {
            return MEMTR_ERR_BADBURST;
         }
         op.type = MEMOP_WRITE;
         done = MemTraceData(state, &op, packet, &result);
         break;
      }
   }

   if (nextOp) {
      *nextOp = op;
   }

   return result;
}


/*
 * MemTraceData --
 *
 *    Internal function for processing word read/write packets.
 *
 *    We split the packet into timestamp, UB/LB, and data,
 *    and use the data to update 'state' and 'op'.
 *
 *    If this function returns 'true', the current burst
 *    ends after this packet.
 */

bool
MemTraceData(MemTraceState *state, MemOp *op, MemPacket packet,
             MemTraceResult *result)
{
   bool ub = MemPacket_RW_UpperByte(packet);
   bool lb = MemPacket_RW_LowerByte(packet);
   uint16_t word = MemPacket_RW_Word(packet);
   bool byteWide = !(ub && lb);

   if (op->length == 0) {
      // Initial address
      op->addr = state->nextAddr << 1;
   }

   state->nextAddr++;

   if (byteWide && op->length) {
      // We don't support byte and word access in the same burst
      *result = MEMTR_ERR_BADBURST;
      return true;
   }

   if (byteWide) {
      if (lb) {
         state->memory[op->addr + op->length++] = word & 0xFF;
      } else {
         state->memory[++op->addr + op->length++] = word >> 8;
      }
      return true;
   }

   state->memory[MEM_MASK & (op->addr + op->length++)] = word & 0xFF;
   state->memory[MEM_MASK & (op->addr + op->length++)] = word >> 8;

   return false;
}


/*
 * MemTrace_ErrorString --
 *
 *    Make a human-readable version of a MemTraceResult.
 */

const char *
MemTrace_ErrorString(MemTraceResult result)
{
   static const char *strings[] = {
      "Success",
      "End of file",
      "Packet synchronization error",
      "Packet checksum error",
      "Malformed read/write burst",
   };

   if (result < 0 || result > sizeof strings / sizeof strings[0]) {
      return "(Unknown error)";
   }

   return strings[result];
}

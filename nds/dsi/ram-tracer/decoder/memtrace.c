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
#include "memtrace.h"


/*
 * Local functions
 */

bool MemTraceData(MemTraceState *state, MemOp *op, uint32_t payload,
                  MemTraceResult *result);


/*
 * Internal definitions for the hardware's raw format:
 */

enum {
   TYPE_ADDR = 0,
   TYPE_READ,
   TYPE_WRITE,
   TYPE_TIMESTAMP
};


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
         return false;
      }
      state->fileBufTail += result;
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
      uint8_t packetBytes[4];
      uint32_t packet, payload, typecode, check, computedCheck;
      size_t nItems = fread(packetBytes, sizeof packetBytes, 1, state->file);

      if (nItems < 1) {
         return MEMTR_EOF;
      }

      // Reassemble a 32-bit big-endian packet from the bytes
      packet = (packetBytes[0] << 24) | (packetBytes[1] << 16) |
         (packetBytes[2] << 8) | packetBytes[3];

      /*
      * Check the alignment. Each byte's MSB is a flag byte,
      * and the flag is only 1 for the first byte in a packet.
      */
      if ((packet & 0x80808080) != 0x80000000) {
         // Half-hearted attempt to recover from sync errors.
         // We could do better than this...
         fgetc(state->file);

         return MEMTR_ERR_SYNC;
      }

      /*
      * Unpack into its component pieces. Each packet has a 23-bit
      * payload, a 2-bit type code, and a 3-bit checksum.
      */

      typecode = (packet >> 29) & 0x03;
      payload = ((packet >> 3) & 0x0F) |
         ((packet >> 4) & 0x7F0) |
         ((packet >> 5) & 0x3F800) |
         ((packet >> 6) & 0x7C0000);
      check = packet & 0x07;

      /*
      * The check bits are a 3-bit checksum over the payload and typecode.
      */

      computedCheck = 0x7 & (typecode +
                             (payload & 0x7) +
                             ((payload >> 3) & 0x7) +
                             ((payload >> 6) & 0x7) +
                             ((payload >> 9) & 0x7) +
                             ((payload >> 12) & 0x7) +
                             ((payload >> 15) & 0x7) +
                             ((payload >> 18) & 0x7) +
                             ((payload >> 21) & 0x7));

      if (check != computedCheck) {
         return MEMTR_ERR_CHECKSUM;
      }

      /*
      * Type-specific actions...
      */

      switch (typecode) {

      case TYPE_ADDR:
         // Addresses end this burst, but we store the address for next time.
         state->nextAddr = payload;
         state->timestamp.clocks++;
         if (op.length) {
            done = true;
         }
         break;

      case TYPE_TIMESTAMP:
         state->timestamp.clocks += payload;
         break;

      case TYPE_READ:
         if (op.type == MEMOP_WRITE) {
            return MEMTR_ERR_BADBURST;
         }
         op.type = MEMOP_READ;
         done = MemTraceData(state, &op, payload, &result);
         break;

      case TYPE_WRITE:
         if (op.type == MEMOP_READ) {
            return MEMTR_ERR_BADBURST;
         }
         op.type = MEMOP_WRITE;
         done = MemTraceData(state, &op, payload, &result);
         break;

      default:
         // Can't happen, this is a 2-bit field.
         break;
      }
   }

   // Calculate seconds from clock cycles
   state->timestamp.seconds = state->timestamp.clocks / (double)RAM_CLOCK_HZ;

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
MemTraceData(MemTraceState *state, MemOp *op, uint32_t payload,
             MemTraceResult *result)
{
   uint32_t timestamp = payload >> 18;
   uint32_t ub = (payload >> 17) & 1;
   uint32_t lb = (payload >> 16) & 1;
   uint32_t word = payload & 0xFFFF;
   bool byteWide = !(ub && lb);

   if (op->length == 0) {
      // Initial address
      op->addr = state->nextAddr << 1;
   }

   state->timestamp.clocks += timestamp + 1;
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

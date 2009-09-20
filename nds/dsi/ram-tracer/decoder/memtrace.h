/*
 * memtrace.h - Decoder library for reading memory trace logs.
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

#ifndef __MEMTRACE_H
#define __MEMTRACE_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>


/*
 * MemOp - One memory operation (burst read/write)
 */

typedef enum {
   MEMOP_INVALID,
   MEMOP_READ,
   MEMOP_WRITE,
} MemOpType;

typedef struct {
   MemOpType type;
   uint32_t  addr;    // In bytes
   uint32_t  length;  // In bytes
} MemOp;


/*
 * MemTraceState - Current state of the memory trace log.
 */

#define MEM_SIZE_BYTES (16 * 1024 * 1024)
#define MEM_MASK       (MEM_SIZE_BYTES - 1)

typedef struct {
   struct {
      uint64_t  clocks;
      double    seconds;
   } timestamp;

   uint64_t  fileOffset;

   uint8_t  memory[MEM_SIZE_BYTES];

   /* Private */

   FILE *file;
   uint32_t fileBufHead;
   uint32_t fileBufTail;
   uint8_t fileBuf[64 * 1024];
   uint32_t nextAddr;             // In words
} MemTraceState;


/*
 * MemTraceResult - Result code (success/error) type.
 */

typedef enum {
   MEMTR_SUCCESS = 0,
   MEMTR_EOF,            // End of input file
   MEMTR_ERR_SYNC,       // Packet synchronization error
   MEMTR_ERR_CHECKSUM,   // Packet checksum error
   MEMTR_ERR_BADBURST,   // Malformed read/write burst
} MemTraceResult;


/*
 * Public functions
 */

bool MemTrace_Open(MemTraceState *state, const char *filename);
void MemTrace_Close(MemTraceState *state);

MemTraceResult MemTrace_Next(MemTraceState *state, MemOp *nextOp);

const char *MemTrace_ErrorString(MemTraceResult result);


#endif /* __MEMTRACE_H */

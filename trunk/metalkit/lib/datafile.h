/* -*- Mode: C; c-basic-offset: 3 -*-
 *
 * datafile.h - Macros for using raw data files included via objcopy.
 *
 * This file is part of Metalkit, a simple collection of modules for
 * writing software that runs on the bare metal. Get the latest code
 * at http://svn.navi.cx/misc/trunk/metalkit/
 *
 * Copyright (c) 2008-2009 Micah Dowty
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

#ifndef __DATAFILE_H__
#define __DATAFILE_H__

#include "types.h"
#include "puff.h"

typedef struct DataFile {
   uint8 *ptr;
   uint32 size;
} DataFile;

#define DECLARE_DATAFILE(symbol, filename)          \
   extern uint8 _binary_ ## filename ## _start[];   \
   extern uint8 _binary_ ## filename ## _size[];    \
   static const DataFile symbol[1] = {{             \
      (uint8*) _binary_ ## filename ## _start,      \
      (uint32) _binary_ ## filename ## _size,       \
   }}

static inline uint32
DataFile_Decompress(const DataFile *f, void *buffer, uint32 bufferSize)
{
   unsigned long sourcelen = f->size;
   unsigned long destlen = bufferSize;

   if (puff(buffer, &destlen, f->ptr, &sourcelen)) {
      asm volatile ("int3");
   }

   return destlen;
}

static inline uint32
DataFile_GetDecompressedSize(const DataFile *f)
{
   return DataFile_Decompress(f, NULL, 0);
}

#endif /* __DATAFILE_H_ */

/*
 * mem_transfer.h -- Data type for representing one basic memory
 *                   operation.  Our lowest-level memory operation is
 *                   a 'transfer', a read or write with an address,
 *                   length, timestamp, and data. This class should
 *                   be independent of the underlying log format.
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

#ifndef __MEM_TRANSFER_H
#define __MEM_TRANSFER_H

#include <stdint.h>

class MemTransfer {
 public:
  static const int MAX_LENGTH = 4096;
  
  typedef uint64_t OffsetType;
  typedef uint32_t AddressType;
  typedef uint32_t LengthType;
  typedef uint32_t DurationType;

  MemTransfer(OffsetType offset = 0) {
    Seek(0);
  }   

  /*
   * Change the offset of this transfer. Seeking resets the
   * transfer's metadata. (The LogReader can use byteCount to estimate
   * the position of the next packet- so we must invalidate it in
   * transfers that have been seeked but not read.)
   */
  void Seek(OffsetType offset) {
    logOffset = offset;
    address = (AddressType) -1;
    byteCount = 0;
    duration = 0;
  }   

  OffsetType LogOffset() {
    return logOffset;
  }


  AddressType address;
  LengthType byteCount;
  DurationType duration;
  
  enum {
    READ,
    WRITE,

    // Error types
    ERROR_OVERFLOW,   // Buffer overflow indicator
    ERROR_SYNC,       // Packet boundary sync problem
    ERROR_CHECKSUM,   // Packet data checksum problem
    ERROR_PROTOCOL,   // Higher-level memory protocol error
  } type;

  uint8_t buffer[MAX_LENGTH];

private:
  OffsetType logOffset;
};


#endif /* __MEM_TRANSFER_H */

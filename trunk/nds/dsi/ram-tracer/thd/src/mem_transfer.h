/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-
 *
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

#include <wx/string.h>
#include <stdint.h>

struct MemTransfer {
    static const int MAX_LENGTH = 4096;

    typedef uint64_t OffsetType;
    typedef uint32_t AddressType;
    typedef uint32_t LengthType;
    typedef uint32_t DurationType;   // Duration of one command
    typedef uint64_t ClockType;      // Global clock values

    MemTransfer(OffsetType _offset = 0, OffsetType _id = 0)
        : offset(_offset),
          id(_id),
          address(0),
          byteCount(0),
          duration(0),
          type(ERROR_UNAVAIL)
    {}


    typedef enum {
        READ,
        WRITE,

        // Error types
        ERROR_OVERFLOW,   // Buffer overflow indicator
        ERROR_SYNC,       // Packet boundary sync problem
        ERROR_CHECKSUM,   // Packet data checksum problem
        ERROR_PROTOCOL,   // Higher-level memory protocol error
        ERROR_UNAVAIL,    // Data not (currently) available
    } TypeEnum;

    TypeEnum type;
    AddressType address;
    LengthType byteCount;
    DurationType duration;
    OffsetType offset;
    OffsetType id;

    uint8_t buffer[MAX_LENGTH];

    const wxString getTypeName() {
        return getTypeName(type);
    }

    static const wxString getTypeName(TypeEnum type) {
        switch (type) {
        case READ:           return wxT("Read");
        case WRITE:          return wxT("Write");
        case ERROR_OVERFLOW: return wxT("Overflow Error");
        case ERROR_SYNC:     return wxT("Sync Error");
        case ERROR_CHECKSUM: return wxT("Checksum Error");
        case ERROR_PROTOCOL: return wxT("Protocol Error");
        case ERROR_UNAVAIL:  return wxT("(Unavailable)");
        default:             return wxT("(Invalid)");
        }
    }
};


/*
 * An iterator that can walk over a single MemTransfer, segmenting it
 * into chunks that are aligned on address boundaries of (1 << log2)
 * bytes.
 */

template <int log2> class AlignedIterator {
public:
    static const int SIZE = 1 << log2;
    static const int SHIFT = log2;
    static const int MASK = SIZE - 1;

    AlignedIterator(MemTransfer &mt)
    {
        mtByteCount = mt.byteCount;

        // Calculate ranges
        addrFirst = mt.address;
        addrLast = mt.address + mt.byteCount - 1;
        blockFirst = addrFirst >> SHIFT;
        blockLast = addrLast >> SHIFT;

        // Point to first block
        mtOffset = 0;
        blockOffset = addrFirst & MASK;
        blockId = blockFirst;

        updateLen();
    }

    /*
     * Point to the next chunk and returns true if there are more
     * chunks, returns false if the previous chunk was the last one.
     */
    bool next()
    {
        ++blockId;
        if (blockId > blockLast)
            return false;

        mtOffset += len;
        blockOffset = 0;
        updateLen();
        return true;
    }

    // Current offset within the MemTransfer
    MemTransfer::LengthType mtOffset;

    // Current offset within the current block
    MemTransfer::LengthType blockOffset;

    // Current block number
    MemTransfer::LengthType blockId;

    // Number of bytes within this block
    MemTransfer::LengthType len;

private:
    void updateLen()
    {
        len = std::min<MemTransfer::LengthType>(SIZE - blockOffset,
                                                mtByteCount - mtOffset);
    }

    MemTransfer::LengthType mtByteCount;
    MemTransfer::AddressType addrFirst;
    MemTransfer::AddressType addrLast;
    MemTransfer::AddressType blockFirst;
    MemTransfer::AddressType blockLast;
};


#endif /* __MEM_TRANSFER_H */

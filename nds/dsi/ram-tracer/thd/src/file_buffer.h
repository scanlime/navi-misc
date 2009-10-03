/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-
 *
 * file_buffer.h -- Simple and inlineable file read buffer, designed for high
 *                  performance when reading many small packets that are usually
 *                  sequential.
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

#ifndef __FILE_BUFFER_H
#define __FILE_BUFFER_H

#include <wx/file.h>
#include <wx/debug.h>
#include <stdint.h>
#include <stdio.h>


class FileBuffer {
    static const int BLOCK_SHIFT = 14;
    static const int BLOCK_SIZE = 1 << BLOCK_SHIFT;
    static const int BLOCK_MASK = BLOCK_SIZE - 1;
    static const int NUM_BLOCKS = 2;

public:
    void Open(const wxChar *filename) {
        bHint = 0;
        for (int i = 0; i < NUM_BLOCKS; i++) {
            bPosition[i] = wxInvalidOffset;
        }
        file.Open(filename);
    }

    void Close() {
        file.Close();
    }

    /*
     * Get a pointer to an in-memory buffer containing data at 'offset'
     * in the file, with at least 'size' valid bytes after the
     * pointer. This ensures that the data is in one of our in-memory
     * buffers, then returns a pointer to that buffer.
     *
     * The returned pointer is only guaranteed to be valid until the
     * next call to Get(), or when this object is destroyed.
     */

    uint8_t *Get(wxFileOffset offset, uint32_t size) {
        wxFileOffset blockAddr = offset & ~BLOCK_MASK;
        uint32_t blockOffset = offset & BLOCK_MASK;
        uint32_t blockRemaining = BLOCK_SIZE - blockOffset;

        /* Fast path- block aligned */
        if (size <= blockRemaining) {

            /* Fast path- in the same block as last time. */
            if (bPosition[bHint] == blockAddr && blockOffset + size <= bSize[bHint]) {
                return bData[bHint] + blockOffset;
            }

            /* Switch blocks, then try again */
            bHint = !bHint;
            if (bPosition[bHint] == blockAddr && blockOffset + size <= bSize[bHint]) {
                return bData[bHint] + blockOffset;
            }

        } else {
            /* Unaligned block */
            blockAddr = offset;
            blockOffset = 0;
        }

        /* Read a new block in from disk. */
        if (file.Seek(blockAddr) != blockAddr) {
            return NULL;
        }
        size_t result = file.Read(bData[bHint], BLOCK_SIZE);

        if (result == wxInvalidOffset) {
            return NULL;
        }
        bPosition[bHint] = blockAddr;
        bSize[bHint] = result;
        if (blockOffset + size > bSize[bHint]) {
            /* EOF */
            return NULL;
        }
        return bData[bHint] + blockOffset;
    }

private:
    wxFile file;
    int bHint;
    wxFileOffset bPosition[NUM_BLOCKS];
    uint32_t bSize[NUM_BLOCKS];
    uint8_t bData[NUM_BLOCKS][BLOCK_SIZE];
};


#endif /* __FILE_BUFFER_H */

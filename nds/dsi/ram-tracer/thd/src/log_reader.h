/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-
 *
 * log_reader.h -- Encapsulates the details of reading the low-level log file format.
 *                 To add new file formats, this is the only object that should need
 *                 to change at all.
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

#ifndef __LOG_READER_H
#define __LOG_READER_H

#include <wx/filename.h>

#include "file_buffer.h"
#include "mem_transfer.h"

class LogReader {
public:

    LogReader() {
        // Must Open() a log before using.
    }

    LogReader(const wxChar *path) {
        Open(path);
    }

    LogReader(const LogReader &toClone) {
        Open(toClone.FileName().GetFullPath());
    }

    void Open(const wxChar *path);
    void Close();

    wxFileName FileName() const {
        return fileName;
    }

    uint64_t MemSize() {
        return 16 * 1024 * 1024;
    }

    // Read the transfer at mt.logOffset
    bool Read(MemTransfer &mt);

    // Seek to the previous transfer (don't read it)
    bool Next(MemTransfer &mt);

    // Seek to the next transfer (don't read it)
    bool Prev(MemTransfer &mt);

    static double GetDefaultClockHZ();

private:
    wxFileName fileName;
    FileBuffer file;
};

#endif /* __LOG_READER_H */

/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-
 *
 * thd_transfertable.cpp - A wxGridTableBase subclass which displays a list of
 *                         memory transfers from a LogIndex.
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

#include "thd_transfertable.h"


THDTransferTable::THDTransferTable(LogIndex *_index, double _clockHz)
    : index(_index), clockHz(_clockHz)
{}

int
THDTransferTable::GetNumberRows()
{
    return index->GetNumTransfers();
}

int
THDTransferTable::GetNumberCols()
{
    return COL_MAX;
}

bool
THDTransferTable::IsEmptyCell(int row, int col)
{
    // No cells are empty
    return false;
}

wxString
THDTransferTable::GetValue(int row, int col)
{
    transferPtr_t tp = index->GetTransferSummary(row);

    switch (col) {

    case COL_TIME:
        return wxString::Format(wxT("%.06f"), tp->time / clockHz);

    case COL_TYPE:
        return tp->getTypeName();

    case COL_ADDRESS:
        return wxString::Format(wxT("%08x"), tp->address);

    case COL_LENGTH:
        return wxString::Format(wxT("%d"), tp->byteCount);

    }
    assert(0);
}

void
THDTransferTable::SetValue(int row, int col, const wxString &value)
{
    // Not editable
    assert(0);
}

wxString
THDTransferTable::GetColLabelValue(int col)
{
    switch (col) {
    case COL_TIME:     return wxT("Time");
    case COL_TYPE:     return wxT("Type");
    case COL_ADDRESS:  return wxT("Address");
    case COL_LENGTH:   return wxT("Length");
    }
    assert(0);
}

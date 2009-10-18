/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-
 *
 * thd_transfertable.h - A wxGridTableBase subclass which displays a list of
 *                       memory transfers from a LogIndex.
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

#ifndef __THD_TRANSFERTABLE_H
#define __THD_TRANSFERTABLE_H

#include <wx/grid.h>
#include "log_index.h"


class THDTransferTable : public wxGridTableBase {
public:
    THDTransferTable(LogIndex *index, double clockHz);
    ~THDTransferTable();

    void AutoSizeColumns(wxGrid &grid);

    virtual int GetNumberRows();
    virtual int GetNumberCols();

    virtual bool IsEmptyCell(int row, int col);
    virtual wxString GetValue(int row, int col);
    virtual void SetValue(int row, int col, const wxString &value);
    virtual wxString GetColLabelValue( int col );
    virtual wxGridCellAttr *GetAttr(int row, int col, wxGridCellAttr::wxAttrKind kind);

    // Columns
    enum {
        COL_TIME,
        COL_TYPE,
        COL_ADDRESS,
        COL_LENGTH,
        COL_MAX,  // Must be last
    };

    // New width for COL_TYPE used when displaying errors
    static const int ERROR_WIDTH = COL_MAX - COL_TYPE;

private:
    void AutoSizeColumn(wxGrid &grid, int col, wxString prototype);

    wxGridCellAttr *defaultAttr;
    wxGridCellAttr *numericAttr;
    wxGridCellAttr *readAttr;
    wxGridCellAttr *writeAttr;
    wxGridCellAttr *errorAttrs[ERROR_WIDTH];
    
    LogIndex *index;
    double clockHz;
};

#endif /* __THD_TRANSFERTABLE_H */

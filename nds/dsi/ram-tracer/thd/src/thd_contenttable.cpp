/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-
 *
 * thd_contenttable.cpp - A wxGrid-based widget which displays the contents of
 *                        memory at a particular time. Content decoding is handled
 *                        by separate Visualizer classes. The content table is configured
 *                        with a list of visualizers which map to columns in the table.
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

#include <wx/dcclient.h>
#include <boost/bind.hpp>
#include "thd_contenttable.h"


THDContentTable::THDContentTable(THDModel *_model)
    : model(_model)
{
}

THDContentTable::~THDContentTable()
{
}

int
THDContentTable::GetNumberRows()
{
    if (bytesPerRow > 0)
        return (model->index->GetMemSize() + bytesPerRow - 1) / bytesPerRow;
    else
        return 0;
}

int
THDContentTable::GetNumberCols()
{
    return columns.size();
}

bool
THDContentTable::IsEmptyCell(int row, int col)
{
    // No cells are empty
    return false;
}

wxString
THDContentTable::GetValue(int row, int col)
{
    THDContentColumn &colObj = columns[col];
    THDVisBlock block = GetBlockForCell(row, col);
    return colObj.visualizer->GetStringValue(block);
}

void
THDContentTable::SetValue(int row, int col, const wxString &value)
{
    // Not editable
    assert(0);
}


wxGridCellAttr *
THDContentTable::GetAttr(int row, int col,
                          wxGridCellAttr::wxAttrKind kind)
{
    THDContentColumn &colObj = columns[col];
    THDVisBlock block = GetBlockForCell(row, col);
    return colObj.visualizer->GetAttr(block);
}


THDVisBlock
THDContentTable::GetBlockForCell(int row, int col)
{
    THDContentColumn &colObj = columns[col];

    AddressType addr = row;
    addr *= bytesPerRow;
    addr += colObj.addrOffset;

    return THDVisBlock(addr, colObj.visualizer->GetBlockSize(), (uint8_t*)"XXXXXXX");
}


THDContentGrid::THDContentGrid(wxWindow *_parent, THDModel *_model)
    : wxGrid(_parent, wxID_ANY),
      model(_model),
      table(_model)
{
    ClearColumns();
    Refresh();

    // Attach model signals
    modelCursorChangeConn = model->cursorChanged.connect(
       boost::bind(&THDContentGrid::modelCursorChanged, this));
}


void
THDContentGrid::Refresh()
{
    // Update the table to account for current index state.

    BeginBatch();

    SetTable(&table, false);
    SetRowLabelSize(0);
    SetColLabelSize(0);

    EnableEditing(false);
    EnableDragRowSize(false);
    EnableDragColSize(false);

    // Scroll by entire rows
    int rowSize = GetRowSize(0);
    if (rowSize > 0)
        SetScrollLineY(rowSize);

    // Size all columns
    for (int col = 0; col < table.columns.size(); col++) {
        THDContentColumn &colObj = table.columns[col];

        wxClientDC dc(GetParent());
        int width = colObj.visualizer->GetWidth(dc);

        SetColMinimalWidth(col, width);
        SetColSize(col, width);
    }

    EndBatch();
}


void
THDContentGrid::ClearColumns()
{
    table.columns.clear();
    table.bytesPerRow = 0;
}


void
THDContentGrid::AddColumns(visualizerPtr_t viz, int count)
{
    int offset = table.bytesPerRow;
    int blockSize = viz->GetBlockSize();

    for (int i = 0; i < count; i++) {
        table.columns.push_back(THDContentColumn(viz, offset));
        offset += blockSize;
    }

    table.bytesPerRow = offset;
}


void
THDContentGrid::modelCursorChanged()
{
}

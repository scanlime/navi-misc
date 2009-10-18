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

#include <wx/settings.h>
#include "thd_transfertable.h"


THDTransferTable::THDTransferTable(LogIndex *_index, double _clockHz)
    : index(_index), clockHz(_clockHz)
{
    wxFont defaultFont = wxSystemSettings::GetFont(wxSYS_SYSTEM_FONT);
    wxFont fixedFont = wxFont(defaultFont.GetPointSize(),
                              wxFONTFAMILY_MODERN,  // Fixed width
                              wxFONTSTYLE_NORMAL,
                              wxFONTWEIGHT_NORMAL);

    /*
     * wxWidgets doesn't make it easy to get a default wxGridCellAttr.
     * This creates one from scratch, duplicating some of the
     * functionality of wxGrid itself. This is lame, but I don't see a
     * better way at the moment.
     */

    defaultAttr = new wxGridCellAttr();

    defaultAttr->SetDefAttr(defaultAttr);
    defaultAttr->SetKind(wxGridCellAttr::Default);
    defaultAttr->SetFont(defaultFont);
    defaultAttr->SetAlignment(wxALIGN_LEFT, wxALIGN_CENTRE);
    defaultAttr->SetRenderer(new wxGridCellStringRenderer);

    defaultAttr->SetTextColour(
        wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
    defaultAttr->SetBackgroundColour(
        wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));

    /*
     * Clone this attribute and make one for numeric data.
     */

    numericAttr = defaultAttr->Clone();
    numericAttr->SetDefAttr(defaultAttr);

    numericAttr->SetFont(fixedFont);
    numericAttr->SetAlignment(wxALIGN_RIGHT, wxALIGN_CENTRE);
}

THDTransferTable::~THDTransferTable()
{
    wxSafeDecRef(defaultAttr);
    wxSafeDecRef(numericAttr);
}

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

wxGridCellAttr *
THDTransferTable::GetAttr(int row, int col,
                          wxGridCellAttr::wxAttrKind kind)
{
    /*
     * Since our data is much too big for wxGrid's normal attribute
     * storage, we short-circuit much of the attribute code in wxGrid
     * and calculate our own cell attributes on-demand right here.
     */

    wxGridCellAttr *attr;

    switch (col) {

    case COL_TIME:
    case COL_ADDRESS:
    case COL_LENGTH:
        attr = numericAttr;
        break;

    default:
        attr = defaultAttr;
    }

    attr->IncRef();
    return attr;
}

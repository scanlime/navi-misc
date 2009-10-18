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
#include <wx/dcclient.h>
#include "color_rgb.h"
#include "thd_transfertable.h"


THDTransferTable::THDTransferTable(THDModel *_model)
    : model(_model)
{
    wxFont defaultFont = wxSystemSettings::GetFont(wxSYS_SYSTEM_FONT);
    wxFont fixedFont = wxFont(defaultFont.GetPointSize(),
                              wxFONTFAMILY_MODERN,  // Fixed width
                              wxFONTSTYLE_NORMAL,
                              wxFONTWEIGHT_NORMAL);

    ColorRGB fgColor(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
    ColorRGB bgColor(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));

    ColorRGB readColor = bgColor.blend(ColorRGB(0x0000FF), 0x40);
    ColorRGB writeColor = bgColor.blend(ColorRGB(0xFF0000), 0x40);

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
    defaultAttr->SetTextColour(fgColor);
    defaultAttr->SetBackgroundColour(bgColor);

    /*
     * Clone this attribute and make one for numeric data.
     */

    numericAttr = defaultAttr->Clone();
    numericAttr->SetDefAttr(defaultAttr);

    numericAttr->SetFont(fixedFont);
    numericAttr->SetAlignment(wxALIGN_RIGHT, wxALIGN_CENTRE);

    /*
     * Read and Write attributes
     */

    readAttr = defaultAttr->Clone();
    readAttr->SetDefAttr(defaultAttr);
    readAttr->SetAlignment(wxALIGN_CENTRE, wxALIGN_CENTRE);
    readAttr->SetBackgroundColour(readColor);

    writeAttr = defaultAttr->Clone();
    writeAttr->SetDefAttr(defaultAttr);
    writeAttr->SetAlignment(wxALIGN_CENTRE, wxALIGN_CENTRE);
    writeAttr->SetBackgroundColour(writeColor);

    /*
     * To display error codes, we turn COL_TYPE into a wide cell.  To
     * do this, we need to set the size of COL_TYPE itself to its new
     * width, and mark the other columns with pointers back to
     * COL_TYPE.
     */

    errorAttrs[0] = defaultAttr->Clone();
    errorAttrs[0]->SetDefAttr(defaultAttr);
    errorAttrs[0]->SetSize(1, ERROR_WIDTH);

    for (int i = 1; i < ERROR_WIDTH; i++) {
        errorAttrs[i] = defaultAttr->Clone();
        errorAttrs[i]->SetDefAttr(defaultAttr);
        errorAttrs[i]->SetSize(0, -i);
    }
}

THDTransferTable::~THDTransferTable()
{
    wxSafeDecRef(defaultAttr);
    wxSafeDecRef(numericAttr);
    wxSafeDecRef(readAttr);
    wxSafeDecRef(writeAttr);

    for (int i = 0; i < ERROR_WIDTH; i++) {
        wxSafeDecRef(errorAttrs[i]);
    }
}

int
THDTransferTable::AutoSizeColumns(wxGrid &grid)
{
    /*
     * Set reasonable sizes for all columns.
     * We don't want to use wxGrid's normal auto-sizing,
     * since it iterates over every row in the table.
     *
     * Returns the total horizontal size of the grid.
     */

    int w = (AutoSizeColumn(grid, COL_TYPE, wxT("  Write  ")) +
             AutoSizeColumn(grid, COL_TIME, wxT(" 0000.000000s ")) +
             AutoSizeColumn(grid, COL_ADDRESS, wxT("00000000 ")) +
             AutoSizeColumn(grid, COL_LENGTH, wxT("000 ")));

    /*
     * Add space for a vertical scrollbar
     */

    w += wxSystemSettings::GetMetric(wxSYS_VSCROLL_X);

    /*
     * Just like wxGrid::AutoSize(), we need to make sure we're sizing
     * to a multiple of the scroll step, so we don't get a horizontal
     * scroll bar.
     */

    int xUnit, yUnit;
    grid.GetScrollPixelsPerUnit(&xUnit, &yUnit);
    return (w + xUnit - 1) / xUnit * xUnit;
}

int
THDTransferTable::AutoSizeColumn(wxGrid &grid, int col, wxString prototype)
{
    /*
     * Set the current and minimum size for a single column, by
     * looking up that column's font and measuring the width of a
     * given 'prototype' string.
     */

    wxGridCellAttr *attr = GetAttr(-1, col, wxGridCellAttr::Default);
    wxFont font = attr->GetFont();

    wxClientDC dc(grid.GetParent());
    wxCoord w, h;
    dc.GetTextExtent(prototype, &w, &h, NULL, NULL, &font);
    wxSafeDecRef(attr);

    grid.SetColMinimalWidth(col, w);
    grid.SetColSize(col, w);

    return w;
}

int
THDTransferTable::GetNumberRows()
{
    return model->index->GetNumTransfers();
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
    transferPtr_t tp = model->index->GetTransferSummary(row);

    switch (col) {

    case COL_TIME:
        return wxString::Format(model->formatClock(tp->time) + wxT(" "));

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
    case COL_LENGTH:   return wxT("Len");
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

    transferPtr_t tp = model->index->GetTransferSummary(row);
    wxGridCellAttr *attr = defaultAttr;

    switch (col) {

    case COL_TIME:
    case COL_ADDRESS:
    case COL_LENGTH:
        attr = numericAttr;
        break;

    case COL_TYPE:
        if (tp->type == MemTransfer::READ)
            attr = readAttr;
        else if (tp->type == MemTransfer::WRITE)
            attr = writeAttr;
        break;
    }

    /*
     * If this row has an error code, the COL_TYPE cell
     * is actually ERROR_WIDTH cells wide. Override the
     * attributes on all cells in this range.
     */
    if (tp->type != MemTransfer::READ && tp->type != MemTransfer::WRITE
        && col >= COL_TYPE && col < (COL_TYPE + ERROR_WIDTH)) {
        attr = errorAttrs[col - COL_TYPE];
    }

    attr->IncRef();
    return attr;
}

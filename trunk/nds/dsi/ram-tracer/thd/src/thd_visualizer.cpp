/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-
 *
 * thd_visualizer.cpp - Visualizers are classes that take small discrete chunks of
 *                      raw memory data, and visualize them in some way. Visualizers
 *                      can be as simple as printing a word in hex, or as complex
 *                      as an interactive disassembler.
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
#include "color_rgb.h"
#include "thd_visualizer.h"


/***********************************************************
 * THDVisBase
 */

THDVisBase::THDVisBase()
{
    /*
     * Set up the default cell attribute.
     * Our default font is fixed-width.
     */

    wxFont sysFont = wxSystemSettings::GetFont(wxSYS_SYSTEM_FONT);
    wxFont fixedFont = wxFont(sysFont.GetPointSize(),
                              wxFONTFAMILY_MODERN,  // Fixed width
                              wxFONTSTYLE_NORMAL,
                              wxFONTWEIGHT_NORMAL);

    ColorRGB fgColor(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
    ColorRGB bgColor(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));

    defaultAttr = new wxGridCellAttr();
    defaultAttr->SetDefAttr(defaultAttr);
    defaultAttr->SetKind(wxGridCellAttr::Default);

    defaultAttr->SetFont(fixedFont);
    defaultAttr->SetAlignment(wxALIGN_LEFT, wxALIGN_CENTRE);
    defaultAttr->SetRenderer(new wxGridCellStringRenderer);
    defaultAttr->SetTextColour(fgColor);
    defaultAttr->SetBackgroundColour(bgColor);
}

THDVisBase::~THDVisBase()
{
    wxSafeDecRef(defaultAttr);
}

int
THDVisBase::GetWidth(wxDC &dc)
{
    /*
     * The default implementation of GetWidth measures the size of an
     * all-zeroes data block.
     */

    int blockSize = GetBlockSize();
    uint8_t *buffer = new uint8_t[blockSize];
    memset(buffer, 0, blockSize);
    THDVisBlock block(0, blockSize, buffer);

    wxString prototype = GetStringValue(block) + wxT(" ");
    wxGridCellAttr *attr = GetAttr(block);
    wxFont font = attr->GetFont();

    wxCoord w, h;
    dc.GetTextExtent(prototype, &w, &h, NULL, NULL, &font);

    delete[] buffer;
    wxSafeDecRef(attr);

    return w;
}

wxGridCellAttr *
THDVisBase::GetAttr(THDVisBlock &block)
{
    defaultAttr->IncRef();
    return defaultAttr;
}


/***********************************************************
 * THDVisAddress
 */

AddressType
THDVisAddress::GetBlockSize()
{
    return 0;
}

wxString
THDVisAddress::GetStringValue(THDVisBlock &block)
{
    return wxString::Format(wxT("%08x:"), block.address);
}


/***********************************************************
 * THDVisHex
 */

AddressType
THDVisHex::GetBlockSize()
{
    return 1;
}

wxString
THDVisHex::GetStringValue(THDVisBlock &block)
{
    return wxString::Format(wxT("%02x"), block.data[0]);
}

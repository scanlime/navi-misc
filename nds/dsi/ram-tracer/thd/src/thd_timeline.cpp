/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-
 *
 * thd_timeline.cpp -- A graphical timeline widget for Temporal Hex Dump
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

#include <wx/dcbuffer.h>
#include <wx/rawbmp.h>
#include "thd_timeline.h"


BEGIN_EVENT_TABLE(THDTimeline, wxPanel)
    EVT_PAINT(THDTimeline::OnPaint)
    EVT_SIZE(THDTimeline::OnSize)
    EVT_MOUSE_EVENTS(THDTimeline::OnMouseEvent)
END_EVENT_TABLE()


bool operator == (SliceKey const &a, SliceKey const &b)
{
    return a.begin == b.begin && a.end == b.end;
}

std::size_t hash_value(SliceKey const &k)
{
    boost::hash<uint64_t> intHasher;
    return intHasher(k.begin);
}


THDTimeline::THDTimeline(wxWindow *_parent, LogIndex *_index)
  : wxPanel(_parent, wxID_ANY, wxPoint(0, 0), wxSize(800, SLICE_HEIGHT)),
    index(_index),
    sliceGenerator(this),
    sliceCache(SLICE_CACHE_SIZE, &sliceGenerator)
{}


void
THDTimeline::OnMouseEvent(wxMouseEvent &event)
{
    wxPoint pos;
    event.GetPosition(&pos.x, &pos.y);

    if (event.LeftDown()) {
        dragOrigin = pos;
        savedView = view;
    }

    if (event.Dragging()) {
        view.origin = savedView.origin - (pos.x - dragOrigin.x) * savedView.scale;
        Refresh();
    }

    static const double ZOOM_FACTOR = 1.2;

    if (event.GetWheelRotation() < 0) {
        zoom(ZOOM_FACTOR, pos.x);
    }
    if (event.GetWheelRotation() > 0) {
        zoom(1 / ZOOM_FACTOR, pos.x);
    }

    event.Skip();
}


void
THDTimeline::zoom(double factor, int xPivot)
{
    LogIndex::ClockType newScale = view.scale * factor + 0.5;

    if (newScale < 1)
        newScale = 1;

    view.origin += xPivot * (view.scale - newScale);
    view.scale = newScale;
    Refresh();
}

void
THDTimeline::OnPaint(wxPaintEvent &event)
{
    wxAutoBufferedPaintDC dc(this);

    int width, height;
    GetSize(&width, &height);

    const int xMin = 0;
    const int xMax = xMin + width - 1;

    {
        typedef wxPixelData<wxBitmap, wxNativePixelFormat> PixelData;
        PixelData data(bufferBitmap);
        LogIndex::ClockType clk = view.origin;
        PixelData::Iterator column(data);

        column.MoveTo(data, xMin, 0);

        for (int x = xMin; x <= xMax; x++) {
            SliceKey key = { clk, clk + view.scale };
            SliceValue &slice = sliceCache.get(key);
            clk = key.end;

            PixelData::Iterator pixOut = column;
            uint32_t *pixIn = slice.pixels;
            ++column;

            for (int y = 0; y < SLICE_HEIGHT; y++) {
                *(uint32_t*)pixOut.m_ptr = *pixIn;
                ++pixIn;
                pixOut.OffsetY(data, 1);
            }
        }
    }

    dc.DrawBitmap(bufferBitmap, 0, 0, false);

    event.Skip();
}


void
THDTimeline::OnSize(wxSizeEvent &event)
{
    /*
     * On resize, reallocate the bufferBitmap. To avoid reallocating
     * it too often during smooth resize, round the width outward.
     */

    static const int WIDTH_ROUNDING = 1 << 8;

    int width, height;
    GetSize(&width, &height);

    int roundedWidth = (width | (WIDTH_ROUNDING - 1)) + 1;
    if (roundedWidth != bufferBitmap.GetWidth()) {
        bufferBitmap.Create(roundedWidth, SLICE_HEIGHT);
    }

    event.Skip();
}


void
THDTimeline::SliceGenerator::fn(SliceKey &key, SliceValue &value)
{
    boost::shared_ptr<LogInstant> begin = timeline->index->GetInstant(key.begin);
    boost::shared_ptr<LogInstant> end = timeline->index->GetInstant(key.end);

    //    for (int stratum = 0; stratum < index->GetNumStrata(); stratum++) {

    for (int y = 0; y < SLICE_HEIGHT; y++) {
        value.pixels[y] = begin->readTotals.get(y);
    }
}

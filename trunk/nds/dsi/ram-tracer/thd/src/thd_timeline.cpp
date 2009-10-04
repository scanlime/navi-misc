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
#include "thd_timeline.h"

#define ID_REFRESH_TIMER  1

BEGIN_EVENT_TABLE(THDTimeline, wxPanel)
    EVT_PAINT(THDTimeline::OnPaint)
    EVT_SIZE(THDTimeline::OnSize)
    EVT_MOUSE_EVENTS(THDTimeline::OnMouseEvent)
    EVT_TIMER(ID_REFRESH_TIMER, THDTimeline::OnRefreshTimer)
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
    sliceCache(SLICE_CACHE_SIZE, &sliceGenerator),
    refreshTimer(this, ID_REFRESH_TIMER)
{}


void
THDTimeline::OnMouseEvent(wxMouseEvent &event)
{
    event.GetPosition(&cursor.x, &cursor.y);

    if (event.LeftDown()) {
        dragOrigin = cursor;
        savedView = view;
    }

    if (event.Dragging()) {
        view.origin = savedView.origin - (cursor.x - dragOrigin.x) * savedView.scale;
        clampView();
        Refresh();
    }

    static const double ZOOM_FACTOR = 1.2;

    if (event.GetWheelRotation() < 0) {
        zoom(ZOOM_FACTOR, cursor.x);
    }
    if (event.GetWheelRotation() > 0) {
        zoom(1 / ZOOM_FACTOR, cursor.x);
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
    clampView();
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
        pixelData_t data(bufferBitmap);
        bool incomplete = false;

        for (int x = xMin; x <= xMax; x++) {
            if (!renderSlice(data, x))
                incomplete = true;
        }

        if (incomplete) {
            refreshTimer.Start(1000 / REFRESH_FPS, wxTIMER_ONE_SHOT);
        }
    }

    dc.DrawBitmap(bufferBitmap, 0, 0, false);

    event.Skip();
}


bool
THDTimeline::renderSlice(pixelData_t &data, int x)
{
    /*
     * Render one vertical slice to the provided data buffer.
     * If the slice is available, returns true. If no data
     * is ready yet, draws placeholder data and returns false.
     */

    LogIndex::ClockType clk = view.origin + view.scale * x;
    SliceKey key = { clk, clk + view.scale };

    pixelData_t::Iterator pixOut(data);
    pixOut.OffsetX(data, x);

    try {
        /*
         * Get this slice's image from the cache, and copy it
         * to the bufferBitmap.
         */

        SliceValue &slice = sliceCache.get(key);
        uint32_t *pixIn = slice.pixels;

        for (int y = 0; y < SLICE_HEIGHT; y++) {
            *(uint32_t*)pixOut.m_ptr = *pixIn;
            ++pixIn;
            pixOut.OffsetY(data, 1);
        }

        return true;

    } catch (LazyCacheMiss &e) {
        /*
         * If no data is ready yet, our cache will start
         * generating it asynchronously, but for now we'll
         * display a placeholder checkerboard pattern.
         *
         * If any slices are incomplete, we'll remember to
         * retry later, so the display progressively updates
         * as slices become available.
         */

        for (int y = 0; y < SLICE_HEIGHT; y++) {
            uint32_t color = (x ^ y) & 8 ? 0x222222 : 0x555555;
            *(uint32_t*)pixOut.m_ptr = color;
            pixOut.OffsetY(data, 1);
        }

        return false;
    }
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

    clampView();
    event.Skip();
}


void
THDTimeline::OnRefreshTimer(wxTimerEvent &event)
{
    Refresh();
}


void
THDTimeline::clampView(void)
{
    /*
     * Clamp the current TimelineView to the allowed range.
     */

    if ((int64_t)view.origin < 0)
        view.origin = 0;

    LogIndex::ClockType duration = index->GetDuration();

    if (view.origin > duration)
        view.origin = duration;
}

void
THDTimeline::SliceGenerator::fn(SliceKey &key, SliceValue &value)
{
    boost::shared_ptr<LogInstant> begin = timeline->index->GetInstant(key.begin);
    boost::shared_ptr<LogInstant> end = timeline->index->GetInstant(key.end);

    for (int y = 0; y < SLICE_HEIGHT; y++) {
        uint64_t readDelta = end->readTotals.get(y) - begin->readTotals.get(y);
        uint64_t writeDelta = end->writeTotals.get(y) - begin->writeTotals.get(y);
        uint64_t zeroDelta = end->zeroTotals.get(y) - begin->zeroTotals.get(y);
        uint32_t color = 0;

        if (zeroDelta && zeroDelta >= readDelta && zeroDelta >= writeDelta) {
            color = 0x0000ff;
        } else if (writeDelta && writeDelta >= readDelta && writeDelta >= zeroDelta) {
            color = 0x00ff00;
        } else if (readDelta) {
            color = 0xff0000;
        }

        value.pixels[y] = color;
    }
}

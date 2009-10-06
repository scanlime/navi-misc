/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-
 *
 * thd_timeline.h -- A graphical timeline widget for Temporal Hex Dump
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

#ifndef __THD_TIMELINE_H
#define __THD_TIMELINE_H

#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/timer.h>
#include <wx/rawbmp.h>
#include <vector>

#include "log_index.h"
#include "lazy_cache.h"
#include "color_rgb.h"


/*
 * Support for hashable slice keys, used in the slice cache.
 */

struct SliceKey {
    LogIndex::ClockType begin;
    LogIndex::ClockType end;
};

bool operator == (SliceKey const &a, SliceKey const &b);
std::size_t hash_value(SliceKey const &k);


/*
 * The range of log strata represented by each vertical pixel
 */

struct StrataRange {
    int begin;
    int end;
};


/*
 * View origin and scale for the timeline
 */

struct TimelineView {
    TimelineView() : origin(0), scale(100000) {}
    LogIndex::ClockType origin;
    LogIndex::ClockType scale;
};


/*
 * Timeline widget
 */

class THDTimeline : public wxPanel {
public:
    THDTimeline(wxWindow *parent, LogIndex *index);

    void OnPaint(wxPaintEvent &event);
    void OnSize(wxSizeEvent &event);
    void OnMouseEvent(wxMouseEvent &event);
    void OnRefreshTimer(wxTimerEvent &event);

    DECLARE_EVENT_TABLE();

private:
    static const int SLICE_HEIGHT      = 256;
    static const int SLICE_CACHE_SIZE  = 1 << 16;
    static const int REFRESH_FPS       = 60;
    static const int MAX_SLICE_AGE     = 30;
    static const int INDEXING_FPS      = 5;

    // Vertical positions within a slice. Might want to make these dynamic later.
    static const int SLICE_STRATA_TOP       = 1;
    static const int SLICE_STRATA_BOTTOM    = 192;
    static const int SLICE_BANDWIDTH_TOP    = 193;
    static const int SLICE_BANDWIDTH_BOTTOM = 255;

    // Timeline color scheme
    static const int COLOR_BG_TOP     = 0xffffff;
    static const int COLOR_BG_BOTTOM  = 0xcccccc;
    static const int COLOR_READ       = 0x2d7db3;
    static const int COLOR_WRITE      = 0xcb0c29;
    static const int COLOR_ZERO       = 0xc57d0c;

    static const int SHADE_CHECKER_1  = 0xaa;
    static const int SHADE_CHECKER_2  = 0xbb;

    struct SliceValue {
        ColorRGB pixels[SLICE_HEIGHT];
    };

    typedef LazyCache<SliceKey, SliceValue> sliceCache_t;
    typedef wxPixelData<wxBitmap, wxNativePixelFormat> pixelData_t;

    struct SliceGenerator : public sliceCache_t::generator_t {
        SliceGenerator(THDTimeline *_timeline) : timeline(_timeline) {}
        virtual void fn(SliceKey &key, SliceValue &value);
        THDTimeline *timeline;
    };

    void zoom(double factor, int xPivot);
    void viewChanged(void);
    bool renderSlice(pixelData_t &data, int x);
    bool renderSliceRange(pixelData_t &data, int xMin, int xMax);
    bool renderSliceRange(wxBitmap &bmp, int xMin, int xMax);

    SliceKey getSliceKeyForPixel(int x);
    StrataRange getStrataRangeForPixel(int y);

    LogIndex *index;
    sliceCache_t sliceCache;
    SliceGenerator sliceGenerator;
    wxBitmap bufferBitmap;
    std::vector<uint8_t> bufferAges;
    wxTimer refreshTimer;

    wxPoint dragOrigin;
    wxPoint cursor;
    TimelineView view;
    TimelineView savedView;
    bool needSliceEnqueue;
};

#endif /* __THD_TIMELINE_H */

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

    LogIndex::ClockType getCenter()
    {
        return (begin + end) >> 1;
    }
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
 * An indicator overlay on top of the timeline widget, showing a
 * position and some other information.
 */

class THDTimelineOverlay {
public:
    THDTimelineOverlay() : visible(false) {}

    void RefreshRects(wxWindow &win);
    void Paint(wxDC &dc);

    bool operator ==(const THDTimelineOverlay &other)
    {
        if (visible != other.visible)
            return false;

        if (visible)
            return (pos == other.pos && labels == other.labels);
        else
            // Content doesn't matter when invisible
            return true;
    }

    bool operator !=(const THDTimelineOverlay &other)
    {
        return !(*this == other);
    }

    bool visible;
    wxPoint pos;
    std::vector<wxString> labels;

private:
    static const int LABEL_OFFSET_X    = 20;
    static const int LABEL_OFFSET_Y    = 8;
    static const int LABEL_BOX_PAD     = 3;
    static const int LABEL_REFRESH_PAD = 5;

    wxRect GetLabelsRect(wxDC &dc, wxSize widgetSize);
    void PaintLabels(wxRect r, wxDC &dc);
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
    friend class THDTimelineOverlay;

    static const int SLICE_HEIGHT      = 256;
    static const int SLICE_CACHE_SIZE  = 1 << 16;
    static const int REFRESH_FPS       = 20;
    static const int MAX_SLICE_AGE     = 30;
    static const int INDEXING_FPS      = 5;

    // Horizontal supersampling
    static const int SUBPIXEL_SHIFT = 2;
    static const int SUBPIXEL_COUNT = 1 << SUBPIXEL_SHIFT;

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
    typedef wxNativePixelFormat pixelFormat_t;
    typedef wxPixelData<wxBitmap, pixelFormat_t> pixelData_t;

    struct SliceGenerator : public sliceCache_t::generator_t {
        SliceGenerator(THDTimeline *_timeline) : timeline(_timeline) {}
        virtual void fn(SliceKey &key, SliceValue &value);
        THDTimeline *timeline;
    };

    void zoom(double factor, int xPivot);
    void pan(int pixels);

    void viewChanged(void);
    void updateBitmapForViewChange(TimelineView &oldView, TimelineView &newView);

    bool renderSlice(pixelData_t &data, int x);
    bool renderSliceRange(pixelData_t &data, int xMin, int xMax);
    bool renderSliceRange(wxBitmap &bmp, int xMin, int xMax);

    SliceKey getSliceKeyForPixel(int x);
    SliceKey getSliceKeyForSubpixel(int x, int subpix);

    StrataRange getStrataRangeForPixel(int y);

    LogIndex *index;
    sliceCache_t sliceCache;
    SliceGenerator sliceGenerator;
    wxBitmap bufferBitmap;
    std::vector<uint8_t> bufferAges;
    wxTimer refreshTimer;

    bool allocated;         // Is our buffer allocated?
    bool slicesDirty;       // Are any slices potentially not up to date on our bitmap?
    bool needSliceEnqueue;  // Should we request slice rendering?

    wxPoint dragOrigin;
    wxPoint cursor;
    TimelineView view;
    THDTimelineOverlay overlay;
};

#endif /* __THD_TIMELINE_H */

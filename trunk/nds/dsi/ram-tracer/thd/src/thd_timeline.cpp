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
#include <string.h>
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
    }

    if (event.Dragging()) {
        pan(dragOrigin.x - cursor.x);
        dragOrigin = cursor;
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
    /*
     * Zoom the timeline in/out, using 'xPivot' as the point to zoom
     * into or out of.  The pivot is measured in pixels from the left
     * side of the widget.
     *
     * This updates the view, calls viewChanged(), and provides
     * immediate interactivity by scaling the current contents of the
     * image buffer and bufferAges.
     */

    LogIndex::ClockType newScale = view.scale * factor + 0.5;
    TimelineView oldView = view;

    if (newScale < 1)
        newScale = 1;

    view.origin += xPivot * (view.scale - newScale);
    view.scale = newScale;

    viewChanged();
    updateBitmapForViewChange(oldView, view);
}


void
THDTimeline::pan(int pixels)
{
    /*
     * Slide the view left or right. Moves the graph left by 'pixels',
     * which may be negative.
     *
     * This updates the view, calls viewChanged(), and provides
     * immediate interactivity by scrolling the current contents of
     * the image buffer and bufferAges.
     */

    TimelineView oldView = view;
    view.origin += pixels * view.scale;

    viewChanged();
    updateBitmapForViewChange(oldView, view);
}


void
THDTimeline::updateBitmapForViewChange(TimelineView &oldView, TimelineView &newView)
{
    /*
     * Update the bufferBitmap and bufferAges to account for a view change.
     * Every column in the new view is populated using the closest available
     * column in the old view, or it is explicitly expired if no matching
     * column is available.
     *
     * This is a generalization of the buffer update we perform for
     * both zooming and panning. It's not the most efficient thing
     * ever, but we only run this when the user interacts with the
     * graph so it isn't super high-frequency.
     *
     * Call this _after_ viewChanged(), so we get a clipped view.
     */

    int width = bufferBitmap.GetWidth();
    int height = bufferBitmap.GetHeight();

    /*
     * Step 1: Resample the age buffer, and calculate a map of which
     *         old column goes with which new column. This lets us
     *         do the actual image scaling horizontally (more cache
     *         friendly) without having to repeat the division at
     *         every pixel.
     */

    LogIndex::ClockType clock = newView.origin;
    std::vector<uint8_t> newAges(width);
    std::vector<int> oldColumns(width);

    for (int col = 0; col < width; col++) {
        int64_t oldClock = clock - oldView.origin + (oldView.scale >> 1);
        int oldCol = oldClock / (int64_t)oldView.scale;

        if (oldCol < 0 || oldCol >= width) {
            // No corresponding old column
            newAges[col] = 0xFF;
            oldColumns[col] = -1;

        } else {
            // Transfer the old column
            newAges[col] = bufferAges[oldCol];
            oldColumns[col] = oldCol;
        }

        clock += newView.scale;
    }

    bufferAges = newAges;

    /*
     * Step 2: Make a new image for the buffer bitmap, resampling it from the
     *         old bitmap.
     */

    pixelData_t pixData(bufferBitmap);
    int stride = pixData.GetRowStride();
    int pixelSize = pixelFormat_t::SizePixel;
    int totalSize = stride * height;
    uint8_t *temp = new uint8_t[totalSize];
    uint8_t *pixBuffer = (uint8_t*) pixData.GetPixels().m_ptr;

    uint8_t *pixIn = pixBuffer;
    uint8_t *pixOut = temp;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int srcX = oldColumns[x];
            if (srcX >= 0) {
                *(uint32_t*)pixOut = *(uint32_t*)(pixIn + srcX * pixelSize);
            }
            pixOut += pixelSize;
        }
        pixIn += stride;
    }

    memcpy(pixBuffer, temp, totalSize);
    delete[] temp;
}


void
THDTimeline::OnPaint(wxPaintEvent &event)
{
    wxPaintDC dc(this);

    int width, height;
    GetSize(&width, &height);

    bool isComplete = renderSliceRange(bufferBitmap, 0, width-1);

    dc.DrawBitmap(bufferBitmap, 0, 0, false);

    if (index->GetState() == index->INDEXING) {
        // Redraw slowly if we're indexing.
        refreshTimer.Start(1000 / INDEXING_FPS, wxTIMER_ONE_SHOT);
    } else if (!isComplete) {
        // Redraw quickly if we're still waiting for more data.
        refreshTimer.Start(1000 / REFRESH_FPS, wxTIMER_ONE_SHOT);
    }

    /*
     * We only enqueue new slices on the first paint after user
     * interaction.  If we're refreshing due to our REFRESH or
     * INDEXING timers, enqueueing slices will just waste CPU time and
     * draw the slices in an unintended order.
     *
     * (If we enqueued slices on every render, we'd be continuously
     * changing the order in which slices are queued- so every time
     * the work thread picks a new slice to work on, it will be
     * effectively random.)
     *
     * We do need to keep enqueueing slices during indexing, since the
     * log duration will be constantly changing.
     */
    needSliceEnqueue = index->GetState() == index->INDEXING;

    event.Skip();
}


bool
THDTimeline::renderSliceRange(pixelData_t &data, int xMin, int xMax)
{
   /*
    * The LazyThread will process the newest requests first. We'd
    * like the first completed rendering to be near the mouse
    * cursor, since that's likely to be where the user is paying
    * the most attention.
    *
    * So, we'll carefully choose our rendering order so that we paint
    * slices near cursor.x last. We alternate between left side and
    * right side.
    */

    int focus = cursor.x;
    bool complete = true;

    if (focus >= xMax) {
        // Focus past right edge: Render left to right

        for (int x = xMin; x <= xMax; x++)
            if (!renderSlice(data, x))
                complete = false;

    } else if (focus <= xMin) {
        // Focus past left edge: Render right to left

        for (int x = xMax; x >= xMin; x--)
            if (!renderSlice(data, x))
                complete = false;

    } else {
        // Inside range. Render out-to-in

        int i = std::max<int>(focus - xMin, xMax - focus) + 1;

        while (1) {
            int x = focus - i;
            if (x >= xMin && x <= xMax)
                if (!renderSlice(data, x))
                    complete = false;

            if (i == 0) {
                // Center slice has only one side.
                break;
            }

            x = focus + i;
            if (x >= xMin && x <= xMax)
                if (!renderSlice(data, x))
                    complete = false;

            --i;
        }
    }

    return complete;
}


bool
THDTimeline::renderSliceRange(wxBitmap &bmp, int xMin, int xMax)
{
    pixelData_t data(bufferBitmap);
    return renderSliceRange(data, xMin, xMax);
}


SliceKey
THDTimeline::getSliceKeyForPixel(int x)
{
    LogIndex::ClockType clk = view.origin + view.scale * x;
    SliceKey key = { clk, clk + view.scale };
    return key;
}


SliceKey
THDTimeline::getSliceKeyForSubpixel(int x, int subpix)
{
    LogIndex::ClockType clk = view.origin + view.scale * x;
    clk <<= SUBPIXEL_SHIFT;
    clk += view.scale * subpix;
    SliceKey key = { clk >> SUBPIXEL_SHIFT, (clk + view.scale) >> SUBPIXEL_SHIFT };
    return key;
}


StrataRange
THDTimeline::getStrataRangeForPixel(int y)
{
    // TODO: Eventually the first/last strata will be dynamic, so we can zoom in.
    const int strataBegin = 0;
    const int strataEnd = index->GetNumStrata();

    const int pixelHeight = SLICE_STRATA_BOTTOM - SLICE_STRATA_TOP;
    const int strataCount = strataEnd - strataBegin;

    y -= SLICE_STRATA_TOP;

    StrataRange range;

    range.begin = (y * strataCount + pixelHeight/2) / pixelHeight;
    range.end = ((y+1) * strataCount + pixelHeight/2) / pixelHeight;

    return range;
}


bool
THDTimeline::renderSlice(pixelData_t &data, int x)
{
    /*
     * Render one vertical slice to the provided data buffer.
     * If the slice is available, returns true. If no data
     * is ready yet, draws placeholder data and returns false.
     */

    pixelData_t::Iterator pixOut(data);
    pixOut.OffsetX(data, x);

    /*
     * Get this pixel's slices from the cache, and merge them into
     * the bufferBitmap.
     */

    bool haveAllSlices = true;
    ColorAccumulator acc[SLICE_HEIGHT];

    for (int s = 0; s < SUBPIXEL_COUNT; s++) {
        try {
            SliceValue &slice = sliceCache.get(getSliceKeyForSubpixel(x, s),
                                               needSliceEnqueue);

            for (int y = 0; y < SLICE_HEIGHT; y++) {
                acc[y] += slice.pixels[y];
            }

        } catch (LazyCacheMiss &e) {
            haveAllSlices = false;
        }
    }

    if (haveAllSlices) {
        for (int y = 0; y < SLICE_HEIGHT; y++) {
            ColorAccumulator a = acc[y];
            a >>= SUBPIXEL_SHIFT;
            ColorRGB c(a);

            // Emphasize edges
            if (c.value == COLOR_BG_TOP) {
                ColorAccumulator above = acc[std::max(0,y-1)];
                ColorAccumulator below = acc[std::min(SLICE_HEIGHT-1,y+1)];

                above >>= SUBPIXEL_SHIFT;
                below >>= SUBPIXEL_SHIFT;

                ColorRGB ca(above);
                ColorRGB cb(below);

                if (ca.value != COLOR_BG_TOP)
                    c = ColorRGB(COLOR_BG_TOP) - (ColorRGB(COLOR_BG_TOP) - ca) * 3.0f;

                if (cb.value != COLOR_BG_TOP)
                    c = ColorRGB(COLOR_BG_TOP) - (ColorRGB(COLOR_BG_TOP) - cb) * 3.0f;
            }

            pixOut.Red() = c.red();
            pixOut.Green() = c.green();
            pixOut.Blue() = c.blue();
            pixOut.OffsetY(data, 1);
        }

        // This slice is up to date
        bufferAges[x] = 0;

        return true;

    } else {
        /*
         * If no data is ready yet, our cache will be generating it
         * asynchronously. If data isn't available yet but the current
         * contents of the buffer isn't too old, we'll leave it
         * alone. If it's older than MAX_SLICE_AGE frames, though,
         * we'll display a placeholder checkerboard pattern.
         *
         * If any slices are incomplete, we'll remember to retry
         * later, so the display progressively updates as slices
         * become available.
         */

        if (bufferAges[x] < MAX_SLICE_AGE) {
            bufferAges[x]++;

        } else {
            // Checkerboard

            for (int y = 0; y < SLICE_HEIGHT; y++) {
                uint8_t shade = (x ^ y) & 8 ? SHADE_CHECKER_1 : SHADE_CHECKER_2;
                pixOut.Red() = shade;
                pixOut.Green() = shade;
                pixOut.Blue() = shade;
                pixOut.OffsetY(data, 1);
            }
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

        /*
         * Start out with each column at the maximum age possible,
         * since the existing contents of the buffer are totally
         * incorrect.
         */
        bufferAges = std::vector<uint8_t>(roundedWidth, 0xFF);
    }

    viewChanged();
    event.Skip();
}


void
THDTimeline::OnRefreshTimer(wxTimerEvent &event)
{
    Refresh();
}


void
THDTimeline::viewChanged(void)
{
    /*
     * The view changed. We need to:
     *
     *   1. Clamp the current TimelineView to the allowed range
     *   2. Remember to enqueue new slices to draw on the next paint
     *   3. Queue up a repaint
     */

    if ((int64_t)view.origin < 0)
        view.origin = 0;

    LogIndex::ClockType duration = index->GetDuration();

    if (view.origin > duration)
        view.origin = duration;

    needSliceEnqueue = true;

    Refresh();
}


void
THDTimeline::SliceGenerator::fn(SliceKey &key, SliceValue &value)
{
    /*
     * Retrieve cached LogInstants for the beginning and end of this slice.
     */

    // Allowable deviation from correct begin/end timestamps
    LogIndex::ClockType fuzz = (key.end - key.begin) >> 2;

    instantPtr_t begin = timeline->index->GetInstant(key.begin, fuzz);
    instantPtr_t end = timeline->index->GetInstant(key.end, fuzz);

    int numStrata = timeline->index->GetNumStrata();
    LogIndex::ClockType timeDiff = end->time - begin->time;

    /*
     * Rescale the log strata to fit in the available pixels.
     */

    int y;
    for (y = 0; y < SLICE_STRATA_TOP; y++)
        value.pixels[y] = COLOR_BG_TOP;

    for (; y < SLICE_STRATA_BOTTOM; y++) {
        /*
         * Which log stratum is the bottom of this pixel in?
         */

        uint64_t readDelta = 0;
        uint64_t writeDelta = 0;
        uint64_t zeroDelta = 0;

        StrataRange range = timeline->getStrataRangeForPixel(y);

        for (int current = range.begin; current < range.end; current++) {
            readDelta += end->readTotals.get(current) -
                begin->readTotals.get(current);
            writeDelta += end->writeTotals.get(current) -
                begin->writeTotals.get(current);
            zeroDelta += end->zeroTotals.get(current) -
                begin->zeroTotals.get(current);
        }

        /*
         * Calculate a color for this pixel
         */

        ColorRGB color(0);

        if (readDelta || writeDelta || zeroDelta) {
            /*
             * If anything at all is happening in this pixel, we want
             * it to be obvious- so use a color that stands out
             * against a white background. But we'll shift the color
             * to indicate how much of the transfer within this pixel
             * is made up of reads, writes, or zero writes.
             */

            double total = readDelta + writeDelta;
            float readAlpha = readDelta / total;
            float writeAlpha = (writeDelta - zeroDelta) / total;
            float zeroAlpha = zeroDelta / total;

            color += ColorRGB(COLOR_READ) * readAlpha;
            color += ColorRGB(COLOR_WRITE) * writeAlpha;
            color += ColorRGB(COLOR_ZERO) * zeroAlpha;
        } else {
            color = COLOR_BG_TOP;
        }

        value.pixels[y] = color;
    }

    for (; y < SLICE_BANDWIDTH_TOP; y++)
        value.pixels[y] = COLOR_BG_TOP;

    /*
     * Calculate and graph the bandwidth in this slice, as a stacked
     * graph showing read/write/zero bandwidth.
     */

    uint64_t readTotal = 0;
    uint64_t writeTotal = 0;
    uint64_t zeroTotal = 0;

    for (int s = 0; s < numStrata; s++) {
        readTotal += end->readTotals.get(s) - begin->readTotals.get(s);
        writeTotal += end->writeTotals.get(s) - begin->writeTotals.get(s);
        zeroTotal += end->zeroTotals.get(s) - begin->zeroTotals.get(s);
    }

    double readBandwidth = timeDiff ? readTotal / (double)timeDiff : 0;
    double writeBandwidth = timeDiff ? writeTotal / (double)timeDiff : 0;
    double zeroBandwidth = timeDiff ? zeroTotal / (double)timeDiff : 0;

    const double vScale = (SLICE_BANDWIDTH_BOTTOM - SLICE_BANDWIDTH_TOP) * -0.5;
    int origin = SLICE_BANDWIDTH_BOTTOM;
    int rH = origin + readBandwidth * vScale + 0.75;
    int rwH = origin + (readBandwidth + writeBandwidth - zeroBandwidth) * vScale + 0.5;
    int rwzH = origin + (readBandwidth + writeBandwidth) * vScale + 0.5;

    for (; y < rwzH; y++)
        value.pixels[y] = COLOR_BG_BOTTOM;
    for (; y < rwH; y++)
        value.pixels[y] = COLOR_ZERO;
    for (; y < rH; y++)
        value.pixels[y] = COLOR_WRITE;
    for (; y < origin; y++)
        value.pixels[y] = COLOR_READ;
}

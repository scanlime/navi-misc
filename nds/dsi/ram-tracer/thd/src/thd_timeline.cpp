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
#include <boost/bind.hpp>
#include <string.h>
#include "thd_timeline.h"

#define ID_REFRESH_TIMER  1

BEGIN_EVENT_TABLE(THDTimeline, wxPanel)
    EVT_PAINT(THDTimeline::OnPaint)
    EVT_SIZE(THDTimeline::OnSize)
    EVT_MOUSE_EVENTS(THDTimeline::OnMouseEvent)
    EVT_TIMER(ID_REFRESH_TIMER, THDTimeline::OnRefreshTimer)
    EVT_KEY_DOWN(THDTimeline::OnKeyDown)
    EVT_SET_FOCUS(THDTimeline::OnFocus)
    EVT_KILL_FOCUS(THDTimeline::OnFocus)
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


THDTimeline::THDTimeline(wxWindow *_parent, THDModel *_model)
  : wxPanel(_parent, wxID_ANY, wxPoint(0, 0), wxSize(800, SLICE_HEIGHT)),
    model(_model),
    index(_model->index),
    sliceGenerator(this),
    sliceCache(SLICE_CACHE_SIZE, &sliceGenerator),
    refreshTimer(this, ID_REFRESH_TIMER),
    allocated(false),
    slicesDirty(true),
    needSliceEnqueue(true),
    isDragging(false),
    hasFocus(false)
{
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);

    // Attach model signals
    model->cursorChanged.connect(boost::bind(&THDTimeline::modelCursorChanged, this));
}


void
THDTimeline::OnMouseEvent(wxMouseEvent &event)
{
    THDTimelineOverlay newOverlay = overlay;

    event.GetPosition(&cursor.x, &cursor.y);

    /*
     * Update state according to this mouse event.
     */

    if (event.LeftDown()) {
        dragOrigin = cursor;
        isDragging = false;
    }

    if (event.Dragging()) {
        pan(dragOrigin.x - cursor.x);
        dragOrigin = cursor;
        isDragging = true;
    }

    if (event.LeftUp() && !isDragging) {
        // Left click
        SetFocus();
        model->moveCursorToTime(getSliceKeyForPixel(cursor.x).getCenter());
    }

    if (event.ShiftDown()) {
        // Shift-wheel: Panning

        static const int WHEEL_PAN = 64;

        if (event.GetWheelRotation() < 0)
            pan(WHEEL_PAN);
        if (event.GetWheelRotation() > 0)
            pan(-WHEEL_PAN);

    } else {
        // Wheel: Zooming

        static const double ZOOM_FACTOR = 1.2;

        if (event.GetWheelRotation() < 0)
            zoom(ZOOM_FACTOR, cursor.x);
        if (event.GetWheelRotation() > 0)
            zoom(1 / ZOOM_FACTOR, cursor.x);
    }

    if (event.Leaving()) {
        // On mouse exit, remove the overlay if it was a mouse cursor overlay.
        if (overlay.style == THDTimelineOverlay::STYLE_MOUSE_CURSOR)
            updateOverlay(THDTimelineOverlay::STYLE_HIDDEN);
    } else if (event.Entering()) {
        // Enable the overly on cursor entry
        updateOverlay(THDTimelineOverlay::STYLE_MOUSE_CURSOR);
    } else {
        // Update the existing overlay
        updateOverlay(overlay.style);
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

    /*
     * If scale is very small (single-digits) the multiplicative zoom
     * may not change it by a whole number. In these cases, make sure it
     * changes at least a little.
     */
    if (newScale == view.scale)
        if (factor > 1.0)
            newScale++;
        else if (newScale)
            newScale--;

    // Minimum scale is 1 clock cycle per pixel
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
THDTimeline::panTo(LogIndex::ClockType focus)
{
    /*
     * Slide the view left or right, trying to keep 'focus' in the
     * middle 2/3 of the screen.
     *
     * Tries to move the display by an integral number of pixels, to
     * make better use of our slice cache.
     */

    TimelineView oldView = view;

    int width, height;
    GetSize(&width, &height);

    LogIndex::ClockType twoThirds = view.scale * (width * 2 / 3);
    LogIndex::ClockType oneThird = twoThirds / 2;

    LogIndex::ClockType minOrigin = focus - view.scale * width*2/3;
    LogIndex::ClockType maxOrigin = focus - view.scale * width*1/3;
    LogIndex::ClockType jumpDistance = twoThirds + oneThird;

    if (view.origin < minOrigin) {
        // Pan left

        if (view.origin < minOrigin - jumpDistance) {
            // Long jump to the left
            view.origin = minOrigin;
        } else {
            // Pixel-aligned scroll to the left
            do {
                view.origin += view.scale;
            } while (view.origin < minOrigin);
        }

    } else if (view.origin > maxOrigin) {
        // Pan right

        if (view.origin > maxOrigin + jumpDistance) {
            // Long jump to the right
            view.origin = maxOrigin;
        } else {
            // Pixel-aligned scroll to the left
            do {
                view.origin -= view.scale;
            } while (view.origin > maxOrigin);
        }

    } else {
        // Already within limits
        return;
    }

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
    wxAutoBufferedPaintDC dc(this);

    int width, height;
    GetSize(&width, &height);

    /*
     * Step 1: Update the bufferBitmap, where we store fully rendered slices.
     *         This bitmap contains the graph proper, but not any overlays.
     *         We only draw this if slicesDirty has been set.
     */

    if (allocated && slicesDirty) {
        wxRegionIterator update(GetUpdateRegion());
        int minSlice = bufferBitmap.GetWidth();
        int maxSlice = 0;

        // Update only the necessary slices
        while (update) {
            minSlice = std::min(minSlice, update.GetX());
            maxSlice = std::max(maxSlice, update.GetX() + update.GetW() - 1);
            update++;
        }

        bool complete = renderSliceRange(bufferBitmap, minSlice, maxSlice);

        /*
         * If we just evaluated every slice, we can use 'complete' to
         * conclusively set slicesDirty and enable or disable
         * continued updates. If not, we need to leave it at its
         * former value.  A future full repaint will set it (and
         * eventually stop any ongoing repaint).
         */
        if (minSlice <= 0 && maxSlice >= width - 1) {
            slicesDirty = !complete;
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
        needSliceEnqueue = index->GetState() != index->COMPLETE;
    }

    updateRefreshTimer(slicesDirty || overlay.incomplete);

    /*
     * Step 2: Draw the bufferBitmap on the screen. This is the base
     *         layer over top of which we'll draw overlays.
     */

    if (allocated)
        dc.DrawBitmap(bufferBitmap, 0, 0, false);

    /*
     * Step 3: Draw overlay. This is a position indicator crosshair
     *         plus some text. If the overlay was incomplete, give
     *         it an additional update.
     */

    if (overlay.incomplete)
        updateOverlay(overlay.style);

    overlay.Paint(dc, hasFocus);

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
    * slices near our 'focus' (overlay position) last. We alternate
    * between left side and right side.
    */

    int focus = overlay.pos.x;
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


void
THDTimeline::updateRefreshTimer(bool waitingForData)
{
    if (!refreshTimer.IsRunning()) {
        if (index->GetState() == index->INDEXING) {
            // Redraw slowly if we're indexing.
            refreshTimer.Start(1000 / INDEXING_FPS, wxTIMER_ONE_SHOT);
        } else if (waitingForData) {
            // Redraw quickly if we're still waiting for more data.
            refreshTimer.Start(1000 / REFRESH_FPS, wxTIMER_ONE_SHOT);
        }
    }
}


void
THDTimeline::updateOverlay(THDTimelineOverlay::style_t style)
{
    /*
     * Rebuild the overlay information, in the provided style.
     * This collects any necessary information, generates the
     * new THDTimelineOverlay, and installs it.
     */

    THDTimelineOverlay newOverlay(style);

    switch (style) {

    case THDTimelineOverlay::STYLE_MOUSE_CURSOR: {
        /*
         * Follow the mouse cursor, and display information about its location.
         */

        SliceKey sliceKey = getSliceKeyForPixel(cursor.x);

        newOverlay.pos = cursor;
        newOverlay.labels.clear();
        newOverlay.addLabel(wxT("Cursor:"));
        newOverlay.addLabel(model->formatClock(sliceKey.getCenter()));

        if (cursor.y >= SLICE_STRATA_TOP && cursor.y < SLICE_STRATA_BOTTOM) {
            // Cursor is in strata range. Show address.

            StrataRange strata = getStrataRangeForPixel(cursor.y);
            LogIndex::AddressType addr = index->GetStratumFirstAddress(strata.begin);
            newOverlay.addLabel(wxString::Format(wxT("0x%08x"), addr));
        }

        if (cursor.y >= SLICE_BANDWIDTH_TOP && cursor.y < SLICE_BANDWIDTH_BOTTOM) {
            /*
             * Cursor is in bandwidth range. Measure the bandwidth,
             * converting from bytes per clock cycle into kilobytes
             * per second:
             *
             *    byte/clock * clock/sec * kb/byte = kb/sec
             */

            const double scale = model->clockHz / 1024.0;

            SliceValue *slice = sliceCache.get(sliceKey, true);
            if (slice) {
                newOverlay.addLabel(wxString::Format(wxT("Read: %.01f kB/s"),
                                                     slice->readBandwidth * scale));
                newOverlay.addLabel(wxString::Format(wxT("Write: %.01f kB/s"),
                                                     slice->writeBandwidth * scale));
            } else {
                // Schedule a refresh, so we get this data asynchronously.
                newOverlay.incomplete = true;
                updateRefreshTimer(true);

                newOverlay.addLabel(wxT("Read: -"));
                newOverlay.addLabel(wxT("Write: -"));
            }
        }
        break;
    }

    case THDTimelineOverlay::STYLE_MODEL_CURSOR: {
        /*
         * Follow the THDModel's cursor, and display information about
         * the hilighted transaction.
         */

        newOverlay.labels.clear();

        // Mandatory cursor time (horizontal)
        newOverlay.pos.x = getPixelForClock(model->cursor.time);

        // Optional transfer information
        if (model->cursor.transferId != THDModelCursor::NO_TRANSFER) {
            transferPtr_t tp =
                model->index->GetTransferSummary(model->cursor.transferId);

            // Transfer type and length
            wxString header = tp->getTypeName();
            if (tp->byteCount == 1)
                header += wxT(", 1 byte");
            else if (tp->byteCount > 1)
                header += wxString::Format(wxT(", %d bytes"), tp->byteCount);
            newOverlay.addLabel(header);
        }

        // Optional address (vertical).
        if (model->cursor.address == THDModelCursor::NO_ADDRESS) {
            // Park just out of view.
            newOverlay.pos.y = -2;
        } else {
            newOverlay.pos.y = getPixelForAddress(model->cursor.address);
            newOverlay.addLabel(wxString::Format(wxT("0x%08x"), model->cursor.address));
        }

        newOverlay.addLabel(model->formatClock(model->cursor.time));
        break;
    }

    default:
        break;
    }

    /*
     * If we changed the overlay, apply this change and request repainting.
     */

    if (newOverlay != overlay) {
        overlay.RefreshRects(*this);
        newOverlay.RefreshRects(*this);
        overlay = newOverlay;
    }
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


int
THDTimeline::getPixelForClock(LogIndex::ClockType clock)
{
    return (clock - view.origin + view.scale / 2) / view.scale;
}


int
THDTimeline::getPixelForStratum(int s)
{
    const int strataBegin = 0;
    const int strataEnd = index->GetNumStrata();

    const int pixelHeight = SLICE_STRATA_BOTTOM - SLICE_STRATA_TOP;
    const int strataCount = strataEnd - strataBegin;

    return (s - strataBegin) * pixelHeight / strataCount + SLICE_STRATA_TOP;
}


int
THDTimeline::getPixelForAddress(LogIndex::AddressType addr)
{
    return getPixelForStratum(index->GetStratumForAddress(addr));
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
        SliceValue *slice = sliceCache.get(getSliceKeyForSubpixel(x, s),
                                           needSliceEnqueue);

        if (slice) {
            for (int y = 0; y < SLICE_HEIGHT; y++) {
                acc[y] += slice->pixels[y];
            }
        } else {
            haveAllSlices = false;
        }
    }

    TimelineGrid grid(this);

    if (haveAllSlices) {
        bool xGrid = grid.testX(x);

        for (int y = 0; y < SLICE_HEIGHT; y++) {
            ColorAccumulator a = acc[y];
            a >>= SUBPIXEL_SHIFT;
            ColorRGB c(a);

            /*
             * Edge emphasis:
             *
             * Our transfers are plotted as instantaneous events, so
             * as you zoom into a single transfer, it gets very light
             * and hard to see. We'd like to make individual transfers
             * very visible, but to still preserve the visual
             * intensity distinctions that arise from our
             * supersampling and blending.
             *
             * This is a simple algorithm that tries to accomplish
             * that goal:
             *
             *   - Any edge pixel (non-background color, bordered on either
             *     side by background) is emphasized using ColorRGB::increaseContrast().
             *     This can saturate the color and lose intensity precision, but it
             *     makes the pixel easy to see.
             *
             *   - An edge pixel's original pre-emphasis color will
             *     'bleed' onto the neighbouring background
             *     pixel(s). This makes the pixel easier to see still,
             *     plus it restores some of the lost intensity
             *     resolution, since very intense original pixels will
             *     now be even darker, whereas very light original
             *     pixels will see little effect from the color bleed.
             */
            {
                static const float EMPHASIS = 4.0f;

                ColorAccumulator above = acc[std::max(0,y-1)];
                ColorAccumulator below = acc[std::min(SLICE_HEIGHT-1,y+1)];

                above >>= SUBPIXEL_SHIFT;
                below >>= SUBPIXEL_SHIFT;

                ColorRGB ca(above);
                ColorRGB cb(below);

                if (c.value == COLOR_BG_TOP) {
                    // This is a background pixel

                    if (ca.value != COLOR_BG_TOP) {
                        // Bleed color from the pixel above
                        c = ca;
                    } else if (cb.value != COLOR_BG_TOP) {
                        // Bleed color from the pixel below
                        c = cb;
                    }
                } else if (ca.value == COLOR_BG_TOP || cb.value == COLOR_BG_TOP) {
                    // This pixel is an edge. Emphasize it.
                    c = c.increaseContrast(COLOR_BG_TOP, EMPHASIS);
                }
            }

            // Draw grid lines
            if (xGrid)
                c = c.blend(COLOR_GRID);

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

        allocated = true;
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
THDTimeline::OnKeyDown(wxKeyEvent &event)
{
    /*
     * Keyboard navigation support
     */

    static const double ZOOM_FACTOR = 1.5;
    static const int PAGE_SIZE = 100;

    switch (event.GetKeyCode()) {

    case WXK_LEFT:
    case WXK_UP:
        model->cursorPrev();
        break;

    case WXK_RIGHT:
    case WXK_DOWN:
        model->cursorNext();
        break;

    case WXK_PAGEUP:
        model->cursorPrev(PAGE_SIZE);
        break;

    case WXK_PAGEDOWN:
        model->cursorNext(PAGE_SIZE);
        break;

    case '-':
    case '_':
        zoom(ZOOM_FACTOR, overlay.pos.x);
        break;

    case '=':
    case '+':
        zoom(1 / ZOOM_FACTOR, overlay.pos.x);
        break;

    default:
        event.Skip();
    }
}


void
THDTimeline::OnFocus(wxFocusEvent &event)
{
    hasFocus = event.GetEventType() == wxEVT_SET_FOCUS;

    // Focus is displayed in our overlay label area
    overlay.RefreshRects(*this);
}


void
THDTimeline::modelCursorChanged()
{
    /*
     * Some widget changed the THDModel's cursor.  Scroll to
     * this new position, and hilight it with our overlay.
     */

    panTo(model->cursor.time);
    updateOverlay(THDTimelineOverlay::STYLE_MODEL_CURSOR);
}


void
THDTimeline::viewChanged()
{
    /*
     * The view changed. We need to:
     *
     *   1. Clamp the current TimelineView to the allowed range
     *   2. Remember to enqueue new slices to draw on the next paint
     *   3. Queue up a repaint
     */

    LogIndex::ClockType duration = index->GetDuration();

    int width, height;
    GetSize(&width, &height);

    LogIndex::ClockType clkWidth = width * view.scale;

    if (duration > clkWidth) {
        // Clamp to end of log, if the log isn't smaller than the widget

        LogIndex::ClockType clkMax = duration - clkWidth;

        if (view.origin > clkMax)
            view.origin = clkMax;

        // Clamp to the beginning
        if ((int64_t)view.origin < 0)
            view.origin = 0;

    } else {
        // Log is smaller than the widget, always display it at the left side
        view.origin = 0;
    }

    needSliceEnqueue = true;
    slicesDirty = true;

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

    value.readBandwidth = timeDiff ? readTotal / (double)timeDiff : 0;
    value.writeBandwidth = timeDiff ? writeTotal / (double)timeDiff : 0;
    value.zeroBandwidth = timeDiff ? zeroTotal / (double)timeDiff : 0;

    const double vScale = (SLICE_BANDWIDTH_BOTTOM - SLICE_BANDWIDTH_TOP) * -0.5;
    int origin = SLICE_BANDWIDTH_BOTTOM;
    int rH = origin + value.readBandwidth * vScale + 0.5;
    int rwH = origin + (value.readBandwidth +
                        value.writeBandwidth -
                        value.zeroBandwidth) * vScale + 0.5;
    int rwzH = origin + (value.readBandwidth +
                         value.writeBandwidth) * vScale + 0.5;

    for (; y < rwzH; y++)
        value.pixels[y] = COLOR_BG_BOTTOM;
    for (; y < rwH; y++)
        value.pixels[y] = COLOR_ZERO;
    for (; y < rH; y++)
        value.pixels[y] = COLOR_WRITE;
    for (; y < origin; y++)
        value.pixels[y] = COLOR_READ;
}


void
THDTimelineOverlay::RefreshRects(wxWindow &win)
{
    bool inTopHalf = (pos.y >= THDTimeline::SLICE_STRATA_TOP &&
                      pos.y < THDTimeline::SLICE_STRATA_BOTTOM);

    int width, height;
    win.GetSize(&width, &height);

    // Horizontal crosshair
    if (inTopHalf)
        win.RefreshRect(wxRect(0, pos.y - 1, width, 3));

    // Vertical crosshair
    win.RefreshRect(wxRect(pos.x - 1, 0, 3, height));

    // Label text. Include some margin for the outline.
    wxClientDC dc(win.GetParent());
    wxRect labelsRect = GetLabelsRect(dc, wxSize(width, height));
    labelsRect.Inflate(LABEL_REFRESH_PAD);
    win.RefreshRect(labelsRect);
}


void
THDTimelineOverlay::Paint(wxDC &dc, bool focusRect)
{
    if (style == STYLE_HIDDEN)
        return;

    wxBrush labelBrush(ColorRGB(THDTimeline::COLOR_BOX_BG), wxSOLID);
    wxPen labelPen(ColorRGB(THDTimeline::COLOR_BOX_BORDER), 1, wxSOLID);
    wxPen focusPen(ColorRGB(THDTimeline::COLOR_FOCUS), 1, wxDOT);
    wxBrush outlineBrush(ColorRGB(THDTimeline::COLOR_OUTLINES), wxSOLID);
    wxBrush vBrush(ColorRGB(THDTimeline::COLOR_CURSOR), wxSOLID);

    bool inTopHalf = (pos.y >= THDTimeline::SLICE_STRATA_TOP &&
                      pos.y < THDTimeline::SLICE_STRATA_BOTTOM);

    int width, height;
    dc.GetSize(&width, &height);

    wxRect labelsRect = GetLabelsRect(dc, wxSize(width, height));

    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(outlineBrush);

    // Horizontal crosshair (hollow)
    if (inTopHalf) {
        dc.DrawRectangle(0, pos.y - 1, width, 1);
        dc.DrawRectangle(0, pos.y + 1, width, 1);
    }

    // Vertical crosshair outline
    dc.DrawRectangle(pos.x - 1, 0, 3, height);

    // Vertical crosshair foreground
    dc.SetBrush(vBrush);
    dc.DrawRectangle(pos.x, 0, 1, height);

    // Label box
    dc.SetBrush(labelBrush);
    dc.SetPen(labelPen);
    dc.DrawRectangle(labelsRect.x - LABEL_BOX_PAD,
                     labelsRect.y - LABEL_BOX_PAD,
                     labelsRect.width + LABEL_BOX_PAD * 2,
                     labelsRect.height + LABEL_BOX_PAD * 2);

    // Focus rectangle (inside label box)
    if (focusRect) {
        dc.SetPen(focusPen);
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        dc.DrawRectangle(labelsRect.x - LABEL_BOX_PAD + 1,
                         labelsRect.y - LABEL_BOX_PAD + 1,
                         labelsRect.width + LABEL_BOX_PAD * 2 - 2,
                         labelsRect.height + LABEL_BOX_PAD * 2 - 2);
    }

    // Label text
    PaintLabels(labelsRect, dc);
}


wxRect
THDTimelineOverlay::GetLabelsRect(wxDC &dc, wxSize widgetSize)
{
    wxRect r(0, 0, 0, 0);

    /*
     * Calculate the overall extents of all label lines
     */

    for (std::vector<wxString>::iterator i = labels.begin();
         i != labels.end(); i++) {

        wxCoord width, height;
        dc.GetTextExtent(*i, &width, &height);
        r.width = std::max(r.width, width);
        r.height += height;
    }

    /*
     * Position the labels adjacent to 'pos'.  Normally it goes below
     * and to the right, typically to the right of the mouse cursor.
     * If we're too close to the bottom of the screen, put it above,
     * and if we're too close to the right, put it to the left.
     */

    r.x = pos.x + LABEL_OFFSET_X;
    r.y = pos.y + LABEL_OFFSET_Y;

    if (r.y + r.height >= widgetSize.y) {
        r.y = pos.y - r.height - LABEL_OFFSET_Y;
    }

    if (r.x + r.width >= widgetSize.x) {
        r.x = pos.x - r.width - LABEL_OFFSET_X;
    }

    return r;
}


void
THDTimelineOverlay::PaintLabels(wxRect r, wxDC &dc)
{
    for (std::vector<wxString>::iterator i = labels.begin();
         i != labels.end(); i++) {

        wxCoord height;
        dc.DrawText(*i, r.x, r.y);
        dc.GetTextExtent(*i, NULL, &height);
        r.y += height;
    }
}


TimelineGrid::TimelineGrid(THDTimeline *_timeline)
    : timeline(_timeline)
{
    /*
     * How big should the grid lines be? Pick an obvious multiple or
     * fraction of a second.
     */

    const int minGridPixels = 5;
    LogIndex::ClockType minGridClocks = timeline->view.scale * minGridPixels;

    static const float scaleList[] = {
        0.001f,  // 1ms
        1.0f,    // 1s
        10.0f,   // 10s
        60.0f,   // 1min
    };

    for (int i = 0; i < sizeof scaleList / sizeof scaleList[0]; i++) {
        xInterval = timeline->model->clockHz * scaleList[i];
        if (xInterval >= minGridClocks)
            break;
    }
}


bool
TimelineGrid::testX(int x)
{
    // Draw a grid line at coordinate 'x'?

    SliceKey sliceKey = timeline->getSliceKeyForPixel(x);
    return (int)(sliceKey.begin / xInterval) != (int)(sliceKey.end / xInterval);
}

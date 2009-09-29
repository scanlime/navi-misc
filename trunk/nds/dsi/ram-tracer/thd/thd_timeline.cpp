/*
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

BEGIN_EVENT_TABLE(THDTimeline, wxPanel)
  EVT_PAINT(THDTimeline::OnPaint)
END_EVENT_TABLE()


THDTimeline::THDTimeline(wxWindow *_parent, LogIndex *_index)
: wxPanel(_parent, wxID_ANY, wxPoint(0, 0), wxSize(800, 128)),
  index(_index)
{}


void
THDTimeline::OnPaint(wxPaintEvent &event)
{
  wxAutoBufferedPaintDC dc(this);
  dc.Clear();

  int width, height;
  GetSize(&width, &height);

  for (int x = 0; x < width; x++) {
    LogIndex::ClockType scale = 10000;
    LogIndex::ClockType time = x * scale;
    boost::shared_ptr<LogInstant> instant = index->GetInstant(time, scale/4);

    int readTotal = 0;
    for (int block = 0; block < index->GetNumBlocks(); block++) {
      readTotal += instant->blockReadTotals[block];
    }

    int value = readTotal & 63;
    dc.DrawLine(x, height - value, x, height);

    printf("x = %d\n", x);
  }

  event.Skip();
}

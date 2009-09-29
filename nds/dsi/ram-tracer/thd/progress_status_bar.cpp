/*
 * progress_status_bar.h -- A wxStatusBar subclass that includes a progress bar.
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

#include <math.h>
#include "progress_status_bar.h"


BEGIN_EVENT_TABLE(ProgressStatusBar, wxStatusBar)
  EVT_SIZE(ProgressStatusBar::OnSize)
END_EVENT_TABLE()


ProgressStatusBar::ProgressStatusBar(wxWindow *parent)
: wxStatusBar(parent),
  gauge(this, wxID_ANY, RANGE_MAX)
{
  int widths[] = { -1, 96, 200 };

  SetFieldsCount(FIELDCOUNT, widths);
}


void
ProgressStatusBar::OnSize(wxSizeEvent& event)
{
  wxRect rect;
  GetFieldRect(FIELD_PROGRESS, rect);
  gauge.SetSize(rect);

  event.Skip();
}


void
ProgressStatusBar::SetProgress(double progress)
{
  gauge.SetValue((int)(RANGE_MAX * progress));
  gauge.Show();
}


void
ProgressStatusBar::HideProgress()
{
  gauge.Hide();
}


void
ProgressStatusBar::SetDuration(double seconds)
{
  SetStatusText(wxString::Format(wxT("%d:%05.2f"),
				 (int) (seconds / 60.0), fmod(seconds, 60.0)),
		FIELD_DURATION);
}

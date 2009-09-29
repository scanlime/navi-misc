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

#ifndef __PROGRESS_STATUS_BAR_H
#define __PROGRESS_STATUS_BAR_H

#include <wx/statusbr.h>
#include <wx/gauge.h>


class ProgressStatusBar : public wxStatusBar {
 public:
  ProgressStatusBar(wxWindow *parent);

  void SetProgress(double progress);
  void HideProgress();

  void SetDuration(double seconds);

  void OnSize(wxSizeEvent& event);

  DECLARE_EVENT_TABLE();

 private:
  enum {
    FIELD_MAIN = 0,
    FIELD_DURATION,
    FIELD_PROGRESS,
    FIELDCOUNT,  // Must be last
  };

  wxGauge gauge;
  static const int RANGE_MAX = 0x10000;
};

#endif /* __PROGRESS_STATUS_BAR_H */

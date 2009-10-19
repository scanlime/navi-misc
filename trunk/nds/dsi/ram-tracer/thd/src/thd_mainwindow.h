/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-
 *
 * thd_mainwindow.h -- Main UI window for Temporal Hex Dump
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

#ifndef __THD_MAINWINDOW_H
#define __THD_MAINWINDOW_H

#include <wx/frame.h>
#include <wx/grid.h>

#include "progress_status_bar.h"
#include "log_reader.h"
#include "log_index.h"
#include "thd_timeline.h"
#include "thd_transfertable.h"
#include "thd_contenttable.h"
#include "thd_model.h"


class THDMainWindow : public wxFrame {
public:
    THDMainWindow();
    virtual ~THDMainWindow();

    void Open(wxString fileName);

    void OnIndexProgress(wxCommandEvent &event);

    DECLARE_EVENT_TABLE();

private:
    void RefreshTables();

    LogReader reader;
    LogIndex index;
    ProgressStatusBar *statusBar;
    THDTimeline *timeline;
    THDTransferGrid *transferGrid;
    THDContentGrid *contentGrid;

    THDModel model;
};

#endif /* __THD_MAINWINDOW_H */

/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-
 *
 * thd_mainwindow.cpp -- Main UI window for Temporal Hex Dump
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

#include <wx/sizer.h>
#include <wx/splitter.h>
#include "thd_mainwindow.h"
#include "thd_timeline.h"

BEGIN_EVENT_TABLE(THDMainWindow, wxFrame)
END_EVENT_TABLE()


THDMainWindow::THDMainWindow()
  : wxFrame(NULL, -1, wxT("Temporal Hex Dump"),
            wxDefaultPosition, wxSize(1000, 700),
            wxDEFAULT_FRAME_STYLE | wxMAXIMIZE),
    clockHz(LogReader::GetDefaultClockHZ())
{
    Connect(index.GetProgressEvent(),
            wxCommandEventHandler(THDMainWindow::OnIndexProgress));
    index.SetProgressReceiver(this);

    statusBar = new ProgressStatusBar(this);
    SetStatusBar(statusBar);

    /*
     * Vertical layout: Timeline, then splitter
     */

    wxSizer *vbox = new wxBoxSizer(wxVERTICAL);

    timeline = new THDTimeline(this, &index); 
    vbox->Add(timeline, 0, wxEXPAND);

    splitter = new wxSplitterWindow(this);
    vbox->Add(splitter, 1, wxEXPAND);

    /*
     * Horizontal split: Transfers and contents
     */

    transferGrid = new wxGrid(splitter, wxID_ANY);
    transferTable = new THDTransferTable(&index, clockHz);

    contentGrid = new wxGrid(splitter, wxID_ANY);

    splitter->SplitVertically(transferGrid, contentGrid);

    SetSizer(vbox);
}


THDMainWindow::~THDMainWindow()
{
    SetStatusBar(NULL);
    delete statusBar;
    delete timeline;
    delete transferGrid;
    delete contentGrid;
    delete transferTable;
    delete splitter;
}


void
THDMainWindow::Open(wxString fileName)
{
    index.Close();
    reader.Close();
    reader.Open(fileName);
    index.Open(&reader);
    RefreshTables();
}


void
THDMainWindow::RefreshTables()
{
    /*
     * Update tables to account for the current index state.
     */

    transferGrid->BeginBatch();
    transferGrid->SetTable(transferTable);
    transferGrid->SetRowLabelSize(0);
    transferGrid->EnableEditing(false);
    transferGrid->EndBatch();

    contentGrid->BeginBatch();
    contentGrid->EnableEditing(false);
    contentGrid->EndBatch();
}


void
THDMainWindow::OnIndexProgress(wxCommandEvent& WXUNUSED(event))
{
    statusBar->SetDuration(index.GetDuration() / (double)clockHz);

    switch (index.GetState()) {

    case LogIndex::IDLE:
        statusBar->SetStatusText(wxT("Idle"));
        statusBar->SetProgress(index.GetProgress());
        break;

    case LogIndex::INDEXING:
        statusBar->SetStatusText(wxT("Building index..."));
        statusBar->SetProgress(index.GetProgress());
        break;

    case LogIndex::FINISHING:
        statusBar->SetStatusText(wxT("Finishing index..."));
        statusBar->SetProgress(index.GetProgress());
        break;

    case LogIndex::COMPLETE:
        statusBar->SetStatusText(wxT("Index complete."));
        statusBar->HideProgress();
        break;

    case LogIndex::ERROR:
        statusBar->SetStatusText(wxT("Indexing error!"));
        statusBar->SetProgress(index.GetProgress());
        break;

    }

    RefreshTables();
}

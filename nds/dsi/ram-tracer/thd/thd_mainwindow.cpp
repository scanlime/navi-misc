/*
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

#include "thd_mainwindow.h"


BEGIN_EVENT_TABLE(THDMainWindow, wxFrame)
END_EVENT_TABLE()


THDMainWindow::THDMainWindow()
: wxFrame(NULL, -1, wxT("Temporal Hex Dump")),
  clockHz(LogReader::GetDefaultClockHZ())
{
  Connect(index.GetProgressEvent(), wxCommandEventHandler(THDMainWindow::OnIndexProgress));
  index.SetProgressReceiver(this);

  statusBar = new ProgressStatusBar(this);
  SetStatusBar(statusBar);
}


THDMainWindow::~THDMainWindow()
{
  SetStatusBar(NULL);
  delete statusBar;
}


void
THDMainWindow::Open(wxString fileName)
{
  index.Close();
  reader.Close();
  reader.Open(fileName);
  index.Open(&reader);
}


void
THDMainWindow::OnIndexProgress(wxCommandEvent& WXUNUSED(event))
{
  statusBar->SetProgress(index.GetProgress());
  statusBar->SetDuration(index.GetDuration() / (double)clockHz);

  switch (index.GetState()) {

  case LogIndex::IDLE:
    statusBar->SetStatusText(wxT("Idle"));
    break;

  case LogIndex::INDEXING:
    statusBar->SetStatusText(wxT("Building index..."));
    break;

  case LogIndex::COMPLETE:
    statusBar->SetStatusText(wxT("Index complete."));
    break;

  case LogIndex::ERROR:
    statusBar->SetStatusText(wxT("Indexing error!"));
    break;

  }
}

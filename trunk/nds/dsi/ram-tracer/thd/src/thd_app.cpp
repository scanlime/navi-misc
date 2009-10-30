/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-
 *
 * thd_app.cpp -- wxApplication for Temporal Hex Dump
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

#include "thd_app.h"

bool
THDApp::OnInit()
{
#ifdef __WXMAC__
    /*
     * On Mac OS, we have one frame per opened document, and the application
     * doesn't exit when the last window is closed.
     */
    SetExitOnFrameDelete(false);

    if (argv[1])
        MacOpenFile(argv[1]);

#else
    /*
     * On other platforms, there is one frame per process.
     */
        
    frame = new THDMainWindow();
    frame->Show();
    SetTopWindow(frame);

    if (argv[1])
        frame->Open(argv[1]);
#endif

    return true;
}

void
THDApp::MacOpenFile(const wxString &fileName)
{
    THDMainWindow *newFrame = new THDMainWindow();
    newFrame->Show();
    newFrame->Open(fileName);
}

IMPLEMENT_APP(THDApp)

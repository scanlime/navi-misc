/* -*- Mode: C; c-basic-offset: 4 -*-
 *
 * Main implementation of SBTHardware, which emulates video using the
 * Nintendo DS's primary screen.
 *
 * Copyright (c) 2009 Micah Dowty <micah@navi.cx>
 *
 *    Permission is hereby granted, free of charge, to any person
 *    obtaining a copy of this software and associated documentation
 *    files (the "Software"), to deal in the Software without
 *    restriction, including without limitation the rights to use,
 *    copy, modify, merge, publish, distribute, sublicense, and/or sell
 *    copies of the Software, and to permit persons to whom the
 *    Software is furnished to do so, subject to the following
 *    conditions:
 *
 *    The above copyright notice and this permission notice shall be
 *    included in all copies or substantial portions of the Software.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *    OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef _SBTHARDWAREMAIN_H_
#define _SBTHARDWAREMAIN_H_

#include "sbtHardwareCommon.h"


class SBTHardwareMain : public SBTHardwareCommon
{
 public:
    virtual void drawScreen(SBTProcess *proc, uint8_t *framebuffer);

 protected:
    bool vidBuffer;

    virtual void writeSpeakerTimestamp(uint32_t timestamp);
    virtual void pollKeys();
};


#endif // _SBTHARDWAREMAIN_H_

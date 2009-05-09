/* -*- Mode: C++; c-basic-offset: 4 -*-
 *
 * An implementation of SBTHardware which displays on the Nintendo
 * DS's sub engine.
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

#ifndef _HWSUB_H_
#define _HWSUB_H_

#include "hwCommon.h"


class HwSub : public HwCommon
{
 public:
    virtual void reset();
    virtual void drawScreen(SBTProcess *proc, uint8_t *framebuffer);

 protected:
    static const unsigned MAP_BASE_OFFSET =
        (SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(uint16_t)) / (16 * 1024);

    int bg;
    uint16_t *backbuffer;
};


#endif // _HWSUB_H_

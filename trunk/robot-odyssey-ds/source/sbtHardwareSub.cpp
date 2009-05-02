/* -*- Mode: C; c-basic-offset: 4 -*-
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

#include <nds.h>
#include <stdio.h>
#include "sbtHardwareSub.h"
#include "videoConvert.h"


void SBTHardwareSub::reset()
{
    SBTHardwareCommon::reset();

    // Mode 5: Two tiled 'text' layers, two extended background layers
    videoSetModeSub(MODE_5_2D);

    /*
     * XXX: We're using bank C, which is only 128kB: not enough to double-buffer.
     */
    vramSetBankC(VRAM_C_SUB_BG_0x06200000);

    bg = bgInitSub(3, BgType_Bmp16, BgSize_B16_256x256, 0, 0);
    backbuffer= bgGetGfxPtr(bg);
}


void SBTHardwareSub::drawScreen(SBTProcess *proc, uint8_t *framebuffer)
{
    VideoConvert::scaleCGAto256(framebuffer, backbuffer);
    SBTHardwareCommon::drawScreen(proc, framebuffer);
}

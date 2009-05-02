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

    videoSetModeSub(MODE_0_2D);
    consoleDemoInit();
    vramSetBankD(VRAM_D_SUB_SPRITE);

    iprintf("Foo!\n");

    oamInit(&oamSub, SpriteMapping_1D_32, false);

    spr = oamAllocateGfx(&oamSub, SpriteSize_64x64, SpriteColorFormat_256Color);

    for (int i = 0; i < 256; i++) {
        SPRITE_PALETTE_SUB[i] = rand();
    }

    oamSet(&oamSub, 0, 20, 20, 0, 0, SpriteSize_64x64,
           SpriteColorFormat_256Color, spr, -1, false, false, false, false, false);

    swiWaitForVBlank();
    oamUpdate(&oamSub);
}


void SBTHardwareSub::drawScreen(SBTProcess *proc, uint8_t *framebuffer)
{
    swiCopy(framebuffer, spr, 64*64/2);

    SBTHardwareCommon::drawScreen(proc, framebuffer);
}

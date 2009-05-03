/* -*- Mode: C; c-basic-offset: 4 -*-
 *
 * An implementation of SBTHardware which displays portions of the
 * screen using hardware sprites.
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
#include <string.h>
#include "hwSprites.h"
#include "videoConvert.h"


void HwSprites::reset()
{
    HwCommon::reset();

    videoSetModeSub(MODE_0_2D);
    consoleDemoInit();
    vramSetBankD(VRAM_D_SUB_SPRITE);

    iprintf("Foo!\n");

    oamInit(&oamSub, SpriteMapping_1D_256, false);

    spr = oamAllocateGfx(&oamSub, SpriteSize_64x64, SpriteColorFormat_16Color);

    /* CGA */
    SPRITE_PALETTE_SUB[0] = 0;  // Transparent
    SPRITE_PALETTE_SUB[1] = RGB8(0x55,0xFF,0xFF);
    SPRITE_PALETTE_SUB[2] = RGB8(0xFF,0x55,0xFF);
    SPRITE_PALETTE_SUB[3] = RGB8(0xFF,0xFF,0xFF);

    oamSet(&oamSub, 0, 20, 20, 0, 0, SpriteSize_64x64,
           SpriteColorFormat_16Color, spr, 0, false, false, false, false, false);
}


void HwSprites::drawScreen(SBTProcess *proc, uint8_t *framebuffer)
{
    VideoConvert::CGAto16ColorTiles(framebuffer, spr, 64, 90, 64, 64);
    VideoConvert::CGAclear(framebuffer, 64, 90, 64, 64);

    static int angle;
    angle += 400;
    oamRotateScale(&oamSub, 0, angle, 0x100 * 5/4, 0x100);
    oamUpdate(&oamSub);

    HwCommon::drawScreen(proc, framebuffer);
}

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
#include "hwSpriteScraper.h"
#include "videoConvert.h"


void HwSpriteScraper::reset()
{
    HwCommon::reset();
    memset(rects, 0, sizeof rects);

    /*
     * Set up the position of each ScraperRect.
     */
    for (int i = 0; i < NUM_SPRITES; i++) {
        SpriteScraperRect *rect = &rects[i];
        int row = i >> 1;
        int column = i & 1;

        rect->x = rect->width * column;
        rect->y = rect->height * row;
    }
}

SpriteScraperRect *HwSpriteScraper::allocRect(OamState *oam)
{
    /*
     * Look for a free sprite rect
     */
    for (int i=0; i < NUM_SPRITES; i++) {
        SpriteScraperRect *rect = &rects[i];
        if (rect->buffer == NULL) {

            rect->oam = oam;
            rect->buffer = oamAllocateGfx(oam, SpriteSize_64x64,
                                          SpriteColorFormat_16Color);

            /*
             * Do one full clear/render cycle before showing a frame.
             * Until that cycle is finished, we'll show a blank sprite.
             */
            rect->live = true;
            rect->livePrev = false;
            dmaFillHalfWords(0, rect->buffer, rect->width * rect->height / 2);

            return rect;
        }
    }
    sassert(false, "Out of SpriteScraperRects");
    return NULL;
}

void HwSpriteScraper::freeRect(SpriteScraperRect *rect)
{
    sassert(rect->buffer, "Freeing non-allocated sprite");

    oamFreeGfx(rect->oam, rect->buffer);
    rect->buffer = NULL;
    rect->oam = NULL;
    rect->live = false;
    rect->livePrev = false;
}

void HwSpriteScraper::drawScreen(SBTProcess *proc, uint8_t *framebuffer)
{
    /*
     * Blit all sprites have been live for at least two frames.
     * This ensures that the background behind a sprite is cleared
     * before we display the first frame after making it live.
     */
    for (int i = 0; i < NUM_SPRITES; i++) {
        SpriteScraperRect *rect = &rects[i];

        if (rect->live && rect->livePrev) {
            VideoConvert::CGAWideto16ColorTiles(framebuffer, rect->buffer,
                                                rect->x, rect->y,
                                                rect->width, rect->height);
        }
        rect->livePrev = rect->live;
    }

    /*
     * Clear behind all live sprites, only after we blit.
     */
    for (int i = 0; i < NUM_SPRITES; i++) {
        SpriteScraperRect *rect = &rects[i];

        if (rect->live) {
            VideoConvert::CGAclear(framebuffer, rect->cgaX(), rect->y,
                                   rect->cgaWidth(), rect->height);
        }
    }

    HwCommon::drawScreen(proc, framebuffer);
}

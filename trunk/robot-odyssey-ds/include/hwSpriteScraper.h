/* -*- Mode: C++; c-basic-offset: 4 -*-
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

#ifndef _HWSPRITESCRAPER_H_
#define _HWSPRITESCRAPER_H_

#include "hwCommon.h"


/*
 * Information about one scraped source rectangle on the framebuffer.
 */
class SpriteScraperRect
{
 public:
    /*
     * 64x64 is fairly wide, but 32x64 is a little bit too narrow to
     * hold a robot that's carrying something. This gives us plenty of
     * buffer room around robots to capture objects they're holding.
     */
    static const int width = 64;
    static const int height = 64;

    /*
     * Normal scale factors for X and Y.
     *
     * This applies a 4/5 scaledown in order to correct for CGA's
     * nonsquare pixels (and make the sprites the same aspect ratio as
     * the main display), and a 2x scaleup to expand the widepixels.
     */
    static const int scaleX = 0x100 / 2 * 5/4;
    static const int scaleY = 0x100;

    /*
     * Sprite format
     */
    static const SpriteSize size = SpriteSize_64x64;
    static const SpriteColorFormat format = SpriteColorFormat_16Color;

    /* Coordinates of top-left corner, in widepixels. */
    int x, y;

    /*
     * Are we actively scraping this rectangle? You can set this flag
     * to false without freeing the rect in order to freeze updates.
     */
    bool live;

    /* CGA pixel X */
    inline int cgaX() {
        return x * 2;
    }

    /* CGA pixel width */
    inline int cgaWidth() {
        return width * 2;
    }

    /*
     * Center coordinates, in units compatible with Robot Odyssey.
     * X axis is in widepixels from the left edge of the screen,
     * Y axis is scanlines from the bottom of the screen.
     */
    inline int centerX() {
        return x + width/2;
    }
    inline int centerY() {
        return 192 - (y + height/2);
    }

    /*
     * Sprite data buffer, in VRAM.
     */
    uint16_t *buffer;

 private:
    friend class HwSpriteScraper;
    OamState *oam;
    bool livePrev;
};


class HwSpriteScraper : public HwCommon
{
 public:
    HwSpriteScraper();

    virtual void drawScreen(SBTProcess *proc, uint8_t *framebuffer);

    /* Affine matrix to use for our sprites */
    static const int MATRIX_ID = 0;

    /*
     * Number of sprites we can support.
     * A 64x64 sprite is 128x64 CGA pixels,
     * so we can fit a 2x3 grid of them onscreen.
     */
    static const int NUM_SPRITES = 6;

    SpriteScraperRect *allocRect(OamState *oam);
    void freeRect(SpriteScraperRect *rect);

 protected:
    SpriteScraperRect rects[NUM_SPRITES];
};


#endif // _HWSPRITESCRAPER_H_

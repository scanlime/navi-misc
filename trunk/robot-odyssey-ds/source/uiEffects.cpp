/* -*- Mode: C++; c-basic-offset: 4 -*-
 *
 * Miscellaneous special effects for use in the UI.
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
#include "uiEffects.h"
#include "spriteDraw.h"


EffectMarquee32::EffectMarquee32(OamState *oam) {
    this->oam = oam;

    for (int frame = 0; frame < NUM_FRAMES; frame++) {
        frames[frame] = oamAllocateGfx(oam, SpriteSize_32x32, SpriteColorFormat_16Color);
        SpriteDraw draw(32, 32, frames[frame]);

        /* Clear the frame to transparent */
        draw.rect(0, 0, SPRITE_SIZE, SPRITE_SIZE, BG_COLOR);

        /*
         * Draw dots that march around the edge of the sprite clockwise
         */

        const int sw = SPRITE_SIZE - DOT_WIDTH;
        const int numDots = 7;
        const int dotSpacing = 16;
        int x = frame;

        for (int i = 0; i < numDots; i++) {
            /*
             * Which edge is the dot on?
             */
            if (x <= sw*1) {
                drawDot(&draw, x, 0);
            } else if (x <= sw*2) {
                drawDot(&draw, sw, x - sw*1);
            } else if (x <= sw*3) {
                drawDot(&draw, sw - (x - sw*2), sw);
            } else if (x <= sw*4) {
                drawDot(&draw, 0, sw - (x - sw*3));
            } else {
                drawDot(&draw, x - sw*4, 0);
            }
            x += dotSpacing;
        }
    }
}

EffectMarquee32::~EffectMarquee32() {
    for (int frame = 0; frame < NUM_FRAMES; frame++) {
        oamFreeGfx(oam, frames[frame]);
        frames[frame] = NULL;
    }
}

void EffectMarquee32::drawDot(SpriteDraw *draw, int x, int y) {
    draw->rect(x, y, DOT_WIDTH, DOT_WIDTH, BORDER_COLOR);
    draw->rect(x+1, y+1, DOT_WIDTH-2, DOT_WIDTH-2, DOT_COLOR);
}

uint16_t *EffectMarquee32::getFrameGfx(int frame) {
    return frames[frame & (NUM_FRAMES - 1)];
}

/* -*- Mode: C++; c-basic-offset: 4 -*-
 *
 * SpriteDraw is a class for doing very simple runtime drawing on
 * 16-color tiled sprite images.
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
#include "spriteDraw.h"

void SpriteDraw::pixel(int x, int y, int color) {
    /*
     * This doesn't have to be fast...
     */

    int tileWidth = width >> 3;
    int tileX = x >> 3;
    int tileY = y >> 3;
    int tileNumber = tileX + tileY * tileWidth;
    uint16_t *tilePtr = gfx + (tileNumber << 4);

    int nybbleX = x & 3;
    int wordX = (x >> 2) & 1;
    int wordY = y & 7;
    int wordNumber = (wordY << 1) | wordX;
    uint16_t *wordPtr = tilePtr + wordNumber;

    int shift = nybbleX << 2;
    int mask = 0xF << shift;

    *wordPtr = (*wordPtr & (~mask)) | (color << shift);
}

void SpriteDraw::rect(int x, int y, int width, int height, int color) {
    while (height--) {
        for (int i = 0; i < width; i++) {
            pixel(x + i, y, color);
        }
        y++;
    }
}

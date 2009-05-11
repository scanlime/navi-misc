/* -*- Mode: C++; c-basic-offset: 4 -*-
 *
 * Utilities for text rendering, and drawing to our "text" background
 * layer, a bitmap layer that holds proportionally-spaced text and
 * backgrounds/borders for that text.
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
#include <stdarg.h>
#include <string.h>
#include "uiText.h"
#include "videoConvert.h"
#include "font_table.h"


//********************************************************** UIFont

int UIFont::getGlyphEscapement(char c) {
    if (c < FONT_FIRST_CHAR || c > FONT_LAST_CHAR) {
        return 0;
    }

    /*
     * Each glyph has a 1-byte header, with the escapement.
     */
    return font_table[(c - FONT_FIRST_CHAR) << FONT_CHAR_SHIFT];
}

uint8_t *UIFont::getGlyphBitmap(char c) {
    if (c < FONT_FIRST_CHAR || c > FONT_LAST_CHAR) {
        return NULL;
    }

    /*
     * Bitmap data is just after the 1-byte header.
     */
    return font_table + 1 + ((c - FONT_FIRST_CHAR) << FONT_CHAR_SHIFT);
}


//********************************************************** UITextLayer


UITextLayer::UITextLayer() {
    /*
     * Background 2.
     *
     * Use 3 banks of memory in VRAM C, starting at bank 3.
     * (We use the first 3 banks for background 3.)
     */
    bg = bgInitSub(2, BgType_Bmp8, BgSize_B8_256x256, 3, 0);

    clear();
    setScroll(0, 0);
}

void UITextLayer::clear() {
    /*
     * Clear memory, and set default parameters. Does not blit the
     * cleared buffer to the screen. After this call, the entire
     * screen will be marked as dirty.
     */

    setTextColor(RGB8(0xFF, 0xFF, 0xFF));
    setBorderColor(RGB8(0x00, 0x00, 0x00));

    setBackgroundColor(RGB8(0x30, 0x30, 0x30));
    setBackgroundOpaque(false);
    setFrameColor(RGB8(0x55, 0xFF, 0xFF));
    setFrameThickness(8);
    setWrapWidth(SCREEN_WIDTH);
    setAlignment(LEFT);

    moveTo(font.borderWidth, font.borderWidth);

    dirty.clear();
    drawRect(Rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT), 0);
}

void UITextLayer::drawFrame(Rect r) {
    /*
     * Draw a frame around the specified rectangle. The rectangle
     * will be filled with the background color, and a frame will be
     * drawn around its edges.
     */

    drawRect(r, bgPaletteIndex);
    drawBorderRects(r, framePaletteIndex, frameThickness);
    drawBorderRects(r.expand(frameThickness), borderPaletteIndex, font.borderWidth);
}

void UITextLayer::setTextColor(uint16_t color) {
    BG_PALETTE_SUB[textPaletteIndex] = color;
}

void UITextLayer::setBorderColor(uint16_t color) {
    BG_PALETTE_SUB[borderPaletteIndex] = color;
}

void UITextLayer::setBackgroundColor(uint16_t color) {
    BG_PALETTE_SUB[bgPaletteIndex] = color;
}

void UITextLayer::setBackgroundOpaque(bool opaque) {
    bgOpaque = opaque;
}

void UITextLayer::setFrameColor(uint16_t color) {
    BG_PALETTE_SUB[framePaletteIndex] = color;
}

void UITextLayer::setFrameThickness(int thickness) {
    frameThickness = thickness;
}

void UITextLayer::setWrapWidth(int width) {
    wrapWidth = width;
}

void UITextLayer::setAlignment(Alignment a) {
    this->align = a;
}

void UITextLayer::moveTo(int x, int y) {
    this->x = x;
    this->y = y;
}

void UITextLayer::printf(const char *format, ...) {
    va_list v;
    static char buffer[1024];

    va_start(v, format);
    vsnprintf(buffer, sizeof buffer, format, v);
    va_end(v);

    draw(buffer);
}

void UITextLayer::draw(const char *text) {
    int offset;
    int lineX = 0;
    int lineClearedTo = 0;
    char c;

    if (align == CENTER) {
        offset = -measureWidth(text) / 2;
    } else if (align == RIGHT) {
        offset = -measureWidth(text);
    } else {
        offset = 0;
    }

    while ((c = *text)) {

        if (c == ' ') {
            /* See if we want to wrap at this space. */
            if (lineX + measureNextWordWidth(text + 1) > wrapWidth) {
                c = '\n';
            }
        }

        if (c == '\n') {
            lineX = 0;
            lineClearedTo = 0;
            y += font.lineHeight;
            text++;
            continue;
        }


        /*
         * Clear behind the next chunk of text.  Glyphs can overlap,
         * so it's likely that the previous glyph needed to clear
         * behind part of this glyph.  'lineClearedTo' keeps track of
         * how far along this line we've cleared so far.
         *
         * Note that we don't need to add explicit dirty rects for the
         * font glyphs, since the backing rectangle will add dirty
         * rects.
         */

        int escapement = font.getGlyphEscapement(c);
        int clearEnd = lineX + escapement + font.borderWidth * 2;

        drawRect(Rect(x + lineClearedTo + offset - font.borderWidth,
                      y - font.borderWidth,
                      clearEnd - lineClearedTo,
                      font.lineHeight),
                 bgOpaque ? bgPaletteIndex : 0);

        lineClearedTo = clearEnd;

        /*
         * Render a normal glyph.
         */

        VideoConvert::unpackFontGlyph(font.getGlyphBitmap(c),
                                      backbuffer, x + lineX + offset, y);
        lineX += escapement;
        text++;
    }
}

int UITextLayer::measureWidth(const char *text) {
    int lineWidth = 0;
    int maxWidth = 0;
    char c;

    /*
     * This may be a multi-line string. Measure the longest line.
     */
    while ((c = *text)) {
        if (c == '\n') {
            lineWidth = 0;
        } else {
            lineWidth += font.getGlyphEscapement(c);
            if (lineWidth > maxWidth) {
                maxWidth = lineWidth;
            }
        }
        text++;
    }

    /*
     * Each character has a pixel of padding built-in. Remove
     * the padding from the last character in the line.
     */
    return maxWidth - 1;
}

int UITextLayer::measureNextWordWidth(const char *text) {
    /*
     * Measure the width, in pixels, of the characters starting
     * at 'text' and ending at the next whitespace or at the end
     * of the string.
     */

    char c;
    int width = 0;

    while ((c = *text)) {
        if (c == ' ' || c == '\n') {
            break;
        }
        width += font.getGlyphEscapement(c);
        text++;
    }

    return width;
}

void UITextLayer::setScroll(int x, int y) {
    bgSetScroll(bg, x, y);
}

void UITextLayer::blit() {
    /*
     * Draw all dirty rectangles to the screen.
     */

    std::vector<Rect>::iterator i;

    for (i = dirty.rects.begin(); i != dirty.rects.end(); i++) {
        blitRect(*i);
    }
    dirty.rects.clear();
}

void UITextLayer::drawRect(Rect r, uint8_t paletteIndex) {
    uint32_t *line = backbuffer + (r.top << 6);
    int height = r.getHeight();
    uint32_t fillWord = (paletteIndex | (paletteIndex << 8) |
                         (paletteIndex << 16) | (paletteIndex << 24));

    while (height--) {
        if (r.left & 3) {
            int leftRemainder = 4 - (r.left & 3);
            memset(r.left + (uint8_t*)line, paletteIndex, leftRemainder);
        }

        int leftWord = (r.left + 3) >> 2;
        int rightWord = r.right >> 2;
        swiFastCopy(&fillWord, line + leftWord,
                    COPY_MODE_FILL | (rightWord - leftWord));

        if (r.right & 3) {
            memset((r.right & ~3) + (uint8_t*)line, paletteIndex, r.right & 3);
        }

        line += 256/4;
    }

    addAlignedDirtyRect(r);
}

void UITextLayer::drawBorderRects(Rect r, uint8_t paletteIndex, int thickness) {
    // Sides
    drawRect(r.adjacentAbove(thickness), paletteIndex);
    drawRect(r.adjacentBelow(thickness), paletteIndex);
    drawRect(r.adjacentLeft(thickness), paletteIndex);
    drawRect(r.adjacentRight(thickness), paletteIndex);

    // Corners
    drawRect(r.adjacentAbove(thickness).adjacentLeft(thickness), paletteIndex);
    drawRect(r.adjacentAbove(thickness).adjacentRight(thickness), paletteIndex);
    drawRect(r.adjacentBelow(thickness).adjacentLeft(thickness), paletteIndex);
    drawRect(r.adjacentBelow(thickness).adjacentRight(thickness), paletteIndex);
}

void UITextLayer::addAlignedDirtyRect(Rect r) {
    dirty.add(r.align(4, 2));
}

void UITextLayer::blitRect(Rect r) {
    sassert(r.isAligned(4, 1), "blitRect rectangle not aligned");
    sassert(!r.isEmpty(), "blitRect rectangle empty");

    uint32_t *fbSrc = backbuffer;
    uint32_t *fbDest = (uint32_t*) bgGetGfxPtr(bg);

    fbSrc += r.top << 6;
    fbDest += r.top << 6;

    int leftWord = r.left >> 2;
    int widthWords = r.getWidth() >> 2;
    uint32_t copyFlags = COPY_MODE_WORD | COPY_MODE_COPY | widthWords;

    int height = r.getHeight();
    while (height--) {
        swiFastCopy(fbSrc + leftWord, fbDest + leftWord, copyFlags);
        fbSrc += 256/4;
        fbDest += 256/4;
    }
}

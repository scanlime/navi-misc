/* -*- Mode: C; c-basic-offset: 4 -*-
 *
 * TextRenderer draws text on the sub screen using Background 2,
 * configured as a 256x320 scrollable 8-bit graphics plane.
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
#include "textRenderer.h"
#include "videoConvert.h"
#include "font_table.h"


void TextRenderer::init() {
    /*
     * Background 2.
     *
     * Use 5 banks of memory in VRAM C, starting at bank 3.
     * (We use the first 3 banks for background 3.)
     */
    bg = bgInitSub(2, BgType_Bmp8, BgSize_B8_256x256, 3, 0);

    /*
     * Clear the video memory.
     */
    dmaFillWords(0, bgGetGfxPtr(bg), width * height);

    setColor(RGB8(0xFF,0xFF,0xFF));
    setBorderColor(RGB8(0,0,0));

    setScroll(0, 0);
    moveTo(0, 0);
    setAlignment(LEFT);
}

void TextRenderer::setScroll(int x, int y) {
    bgSetScroll(bg, x, y);
}

void TextRenderer::setColor(uint16_t color) {
    BG_PALETTE_SUB[fgPaletteIndex] = color;
}

void TextRenderer::setBorderColor(uint16_t color) {
    BG_PALETTE_SUB[borderPaletteIndex] = color;
}

void TextRenderer::moveTo(int x, int y) {
    this->x = x;
    this->y = y;
}

void TextRenderer::setAlignment(Alignment a) {
    this->align = a;
}

void TextRenderer::printf(const char *format, ...) {
    va_list v;
    static char buffer[1024];

    va_start(v, format);
    vsnprintf(buffer, sizeof buffer, format, v);
    va_end(v);

    draw(buffer);
}

void TextRenderer::draw(const char *text) {
    int offset;

    if (align == CENTER) {
        offset = -measureWidth(text) / 2;
    } else if (align == RIGHT) {
        offset = -measureWidth(text);
    } else {
        offset = 0;
    }

    char c;
    const char *lineStart = text;
    int lineChars = 0;

    /*
     * Break up the text into lines, so we can draw (and
     * double-buffer) each line separately.
     */

    while ((c = *text)) {
        if (c == '\n') {
            if (lineChars) {
                drawLine(lineStart, lineChars, x + offset, y);
            }
            lineChars = 0;
            lineStart = text + 1;
            y += lineHeight;
        } else {
            lineChars++;
        }
        text++;
    }
    if (lineChars) {
        drawLine(lineStart, lineChars, x + offset, y);
    }
}

int TextRenderer::measureWidth(const char *text) {
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
            lineWidth += getGlyphEscapement(c);
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

int TextRenderer::getGlyphEscapement(char c) {
    if (c < FONT_FIRST_CHAR || c > FONT_LAST_CHAR) {
        return 0;
    }

    /*
     * Each glyph has a 1-byte header, with the escapement.
     */
    return font_table[(c - FONT_FIRST_CHAR) << FONT_CHAR_SHIFT];
}

uint8_t *TextRenderer::getGlyphBitmap(char c) {
    if (c < FONT_FIRST_CHAR || c > FONT_LAST_CHAR) {
        return NULL;
    }

    /*
     * Bitmap data is just after the 1-byte header.
     */
    return font_table + 1 + ((c - FONT_FIRST_CHAR) << FONT_CHAR_SHIFT);
}

void TextRenderer::drawLine(const char *lineStart, int lineChars, int x, int y) {
    /*
     * Draw one double-buffered line. We want new text to overwrite
     * old text, without flickering, but we can't double-buffer
     * individual characters or words because glyphs overlap each
     * other. Instead, we double-buffer one line at a time, drawing
     * the text into DTCM memory that we just cleared, then DMA'ing
     * that to VRAM.
     */

    const int backBufY = borderWidth;
    const int height = 15 + borderWidth * 2;

    /*
     * Match the sub-word alignment of the destination.
     */
    int backBufX = borderWidth + (x & 3);
    static DTCM_DATA uint32_t backBuf[256 * height / 4];

    /*
     * Initialize the backbuffer to fully transparent.
     */

    uint32_t zero = 0;
    swiFastCopy(&zero, backBuf, COPY_MODE_FILL | (sizeof backBuf / sizeof backBuf[0]));

    /*
     * Render each glyph. This is a read-modify-write operation, since
     * glyphs can overlap each other and borders will appear behind
     * glyph data itself.  (We blend borders and glyphs using an OR
     * operation, which in this case lets the glyph itself always come
     * out on top.)
     */

    while (lineChars) {
        char c = *lineStart;
        int escapement = getGlyphEscapement(c);
        VideoConvert::unpackFontGlyph(getGlyphBitmap(c),
                                      backBuf, backBufX, backBufY);

        backBufX += escapement;
        lineStart++;
        lineChars--;
    }

    /*
     * Blit just the portion of it we used.
     */

    uint32 *srcPtr = backBuf;
    uint32 *destPtr = (uint32*)bgGetGfxPtr(bg) + (x >> 2) + (y << 6);
    int width = backBufX + borderWidth;
    int widthInWords = (width + 3) >> 2;

    for (int line = height; line; line--) {
        swiFastCopy(srcPtr, destPtr, widthInWords);
        srcPtr += 256 / 4;
        destPtr += 256 / 4;
    }
}

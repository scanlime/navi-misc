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

#ifndef _UITEXT_H_
#define _UITEXT_H_

#include <nds.h>
#include <stdarg.h>
#include "rect.h"


class UIFont
{
public:
    /* Metrics */
    static const int borderWidth = 1;
    static const int glyphHeight = 15;
    static const int lineSpacing = 14;

    static const int baselineY = 11;
    static const int ascenderY = 3;
    static const int centerY = (baselineY + ascenderY) / 2;

    /* A small vertical space (For the '\v' escape code.) */
    static const int vSpace = 5;

    static int getGlyphEscapement(char c);
    static uint8_t *getGlyphBitmap(char c);
};


class UITextLayer
{
 public:
    UITextLayer();
    ~UITextLayer();

    UIFont font;
    enum Alignment { LEFT, CENTER, RIGHT };

    void setTextColor(uint16_t color);
    void setBorderColor(uint16_t color);
    void setBackgroundColor(uint16_t color);
    void setBackgroundOpaque(bool opaque);
    void setFrameColor(uint16_t color);
    void setFrameThickness(int thickness);
    void setWrapWidth(int width);
    void setAlignment(Alignment align);
    void moveTo(int x, int y);

    void clear();
    void drawFrame(Rect r);

    void printf(const char *format, ...);
    void vprintf(const char *format, va_list v);
    void draw(const char *text);
    int measureWidth(const char *text);

    /*
     * These commands affect the screen immediately.
     */
    void setScroll(int x, int y);
    void blit();

 private:
    static const int textPaletteIndex = 0xFF;
    static const int borderPaletteIndex = 0xFE;
    static const int framePaletteIndex = 0xFD;
    static const int bgPaletteIndex = 0xFC;      // Must be even

    void drawRect(Rect r, uint8_t paletteIndex);
    void drawBorderRects(Rect r, uint8_t paletteIndex, int thickness);
    int measureNextWordWidth(const char *text);
    void addAlignedDirtyRect(Rect r);
    void blitRect(Rect r);

    uint32_t backbuffer[SCREEN_WIDTH * SCREEN_HEIGHT / sizeof(uint32_t)];
    DirtyRectTracker dirty;

    int bg;
    int x, y;
    Alignment align;
    int frameThickness;
    bool bgOpaque;
    int wrapWidth;
};

#endif // _UITEXT_H_

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


/*
 * Color scheme for drawing text
 */
class TextColors
{
public:
    TextColors(uint8_t _fg = 0xFF,
               uint8_t _bg = 0xFE,
               uint8_t _border = 0xFD,
               uint8_t _frame = 0xFC)
        : fg(_fg), bg(_bg), border(_border), frame(_frame) {}

    void setBgTransparent() {
        bg = 0;
    }

    void setFgPalette(uint16_t color = RGB8(0xFF, 0xFF, 0xFF)) {
        BG_PALETTE_SUB[fg] = color;
    }

    void setBgPalette(uint16_t color = RGB8(0x30, 0x30, 0x30)) {
        BG_PALETTE_SUB[bg] = color;
    }

    void setBorderPalette(uint16_t color = RGB8(0x00, 0x00, 0x00)) {
        BG_PALETTE_SUB[border] = color;
    }

    void setFramePalette(uint16_t color = RGB8(0x55, 0xFF, 0xFF)) {
        BG_PALETTE_SUB[frame] = color;
    }

    void setDefaultPalette() {
        setFgPalette();
        setBgPalette();
        setBorderPalette();
        setFramePalette();
    }

    uint8_t fg;
    uint8_t bg;
    uint8_t border;
    uint8_t frame;
};


/*
 * One glyph image from a font.
 */
class Glyph
{
public:
    Glyph(int _escapement, int _height, uint8_t *_bitmap)
        : escapement(_escapement),
          height(_height),
          bitmap(_bitmap) {}

    int getEscapement() {
        return escapement;
    }

    int getHeight() {
        // Account for the border
        return height + 2;
    }

    int getWidth() {
        // Account for the border
        return 8 + 2;
    }

    uint8_t getRow(int y) {
        if (y >= 0 && y < height) {
            return bitmap[y];
        } else {
            return 0;
        }
    }

    void drawRow(int y, uint8_t *dest, TextColors colors);

private:
    int escapement;
    int height;
    uint8_t *bitmap;
};


/*
 * Abstract base class for a proportional bitmap font.
 */
class Font
{
public:
    virtual int getLineSpacing() = 0;
    virtual Glyph getGlyph(char c) = 0;
};


/*
 * Default font.
 */
class DefaultFont : public Font
{
public:
    virtual int getLineSpacing();
    virtual Glyph getGlyph(char c);
};


/*
 * A hardware layer (BG2, in 8-bit mode) with double-buffering
 * and practically unbounded vertical scrolling. When we scroll,
 * the necessary parts of the screen are redrawn by a subclass
 * who overrides the draw() method.
 *
 * Supports low-level rendering with vertical clipping and wrapping.
 */
class VScrollLayer {
public:
    VScrollLayer();
    virtual ~VScrollLayer();

    static const int width = 256;
    static const int height = 256;

    void scrollTo(int y);

    int getScroll() {
        return yScroll;
    }

protected:
    void blit();
    void clear();
    void drawRect(Rect r, uint8_t paletteIndex);
    void addDirtyRect(Rect r);
    virtual void paint();

    struct Clip {
        int y1, y2;
    };

    Clip clip;

    void resetClip() {
        clip.y1 = 0;
        clip.y2 = height;
    }

    bool inClip(int y) {
        return y >= clip.y1 && y < clip.y2;
    }

    uint8_t *linePtr(int y) {
        return (uint8_t*)backbuffer + ((y & 0xFF) << 8);
    }

    uint32_t *linePtr32(int y) {
        return (uint32_t*) linePtr(y);
    }

private:
    void blitRect(Rect r);

    uint32_t *destLinePtr32(int y) {
        uint32_t *ptr = (uint32_t*) bgGetGfxPtr(bg);
        ptr += (y & 0xFF) << 6;
        return ptr;
    }

    int bg;
    uint32_t backbuffer[width * height / sizeof(uint32_t)];
    DirtyRectTracker dirty;
    int yScroll;
};


class UITextLayer : public VScrollLayer
{
 public:
    UITextLayer();

    enum Alignment { LEFT, CENTER, RIGHT };

    void setWrapWidth(int _wrapWidth) {
        wrapWidth = _wrapWidth;
    }

    void setAlignment(Alignment _align) {
        align = _align;
    }

    void setAutoclear(bool _autoClear) {
        autoClear = _autoClear;
    }

    void moveTo(int _x, int _y) {
        x = _x;
        y = _y;
    }

    void drawFrame(Rect r, int thickness=8);

    void printf(const char *format, ...);
    void vprintf(const char *format, va_list v);
    void draw(const char *text);
    int measureWidth(const char *text);

    TextColors colors;

private:
    void drawBorderRects(Rect r, uint8_t paletteIndex, int thickness);
    int measureNextWordWidth(const char *text);

    DefaultFont font;

    int x, y;
    Alignment align;
    int wrapWidth;
    bool autoClear;
};


#endif // _UITEXT_H_

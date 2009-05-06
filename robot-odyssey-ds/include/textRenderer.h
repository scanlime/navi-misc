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

#ifndef _TEXTRENDERER_H_
#define _TEXTRENDERER_H_

class TextRenderer
{
 public:
    void init();

    static const int width = 256;
    static const int height = 320;
    static const int lineHeight = 16;
    static const int borderWidth = 1;
    static const int fgPaletteIndex = 0xFF;
    static const int borderPaletteIndex = 0xFE;

    enum Alignment { LEFT, CENTER, RIGHT };

    void setScroll(int x, int y);
    void setColor(uint16_t color);
    void setBorderColor(uint16_t color);

    void moveTo(int x, int y);
    void setAlignment(Alignment align);

    void printf(const char *format, ...);
    void draw(const char *text);
    int measureWidth(const char *text);

 private:
    int getGlyphEscapement(char c);
    uint8_t *getGlyphBitmap(char c);
    void drawLine(const char *lineStart, int lineChars, int x, int y);

    int bg;
    int x, y;
    Alignment align;
};

#endif // _TEXTRENDERER_H_

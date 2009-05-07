/* -*- Mode: C; c-basic-offset: 4 -*-
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

#ifndef _UIEFFECTS_H_
#define _UIEFFECTS_H_

class SpriteDraw;

/*
 * An animated 32x32 marquee, to indicate a button which is actively
 * running or doing something.
 */

class EffectMarquee32
{
 public:
    void init(OamState *oam);
    void free();

    static const int NUM_FRAMES = 16;   // Must be a power of two
    static const int SPRITE_SIZE = 32;

    uint16_t *getFrameGfx(int frame);

 private:
    static const int DOT_WIDTH = 4;
    static const int DOT_COLOR = 2;
    static const int BORDER_COLOR = 1;
    static const int BG_COLOR = 0;

    void drawDot(SpriteDraw *draw, int x, int y);

    OamState *oam;
    uint16_t *frames[NUM_FRAMES];
};


#endif // _UIEFFECTS_H_

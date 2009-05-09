/* -*- Mode: C++; c-basic-offset: 4 -*-
 *
 * Fast video conversion routines. These run out of ITCM RAM, and are
 * compiled with 32-bit ARM instructions.
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

#ifndef _VIDEOCONVERT_H_
#define _VIDEOCONVERT_H_

#include <stdint.h>
#include <nds.h>

namespace VideoConvert
{
    void scaleCGAto256(uint8_t *cgaBuffer, uint16_t *fb16);
    void scaleCGAPlaneTo256(uint8_t *cgaBuffer, uint16_t *fb16);

    void CGAto16ColorTiles(uint8_t *cgaBuffer, uint16_t *spr,
                           uint32_t x, uint32_t y,
                           uint32_t width, uint32_t height);

    void CGAWideto16ColorTiles(uint8_t *cgaBuffer, uint16_t *spr,
                               uint32_t x, uint32_t y,
                               uint32_t width, uint32_t height);

    void CGAclear(uint8_t *cgaBuffer,
                  uint32_t x, uint32_t y,
                  uint32_t width, uint32_t height);

    void unpackFontGlyph(uint8_t *bitmap, uint32_t *fb,
                         uint32_t x, uint32_t y);

    extern const uint16_t palette[16];
}

#endif // _VIDEOCONVERT_H_

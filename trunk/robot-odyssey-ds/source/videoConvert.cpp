/* -*- Mode: C; c-basic-offset: 4 -*-
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

#include "videoConvert.h"


/*
 * Scale a CGA framebuffer to 256x192 16bpp
 */
void ITCM_CODE VideoConvert::scaleCGAto256(uint8_t *in, uint16_t *out)
{
    scaleCGAPlaneTo256(in, out);
    scaleCGAPlaneTo256(in + 0x2000, out + 256);
}


/*
 * Scale one plane of a CGA framebuffer, writing every other line of a
 * 256x192 16bpp output buffer.
 */
void ITCM_CODE VideoConvert::scaleCGAPlaneTo256(uint8_t *in, uint16_t *out)
{
    /*
     * Palette tables for an antialiased scaler which squishes every 5
     * input pixels into 4 output pixels. See gen-scaler-table.py for
     * the general algorithm these were generated with.
     */
    static const DTCM_DATA uint16_t pal00_pal31[] = { 0x8000, 0xe728, 0xe519, 0xe739 };
    static const DTCM_DATA uint16_t pal01_pal30[] = { 0x0000, 0x18c2, 0x1846, 0x18c6 };
    static const DTCM_DATA uint16_t pal10_pal21[] = { 0x8000, 0xce66, 0xccd3, 0xce73 };
    static const DTCM_DATA uint16_t pal11_pal20[] = { 0x0000, 0x3184, 0x308c, 0x318c };

    int x, y;

    for (y = 96; y; y--) {
        uint16_t color;
        uint8_t byte;

        /*
         * Our scaling pattern is 5 input pixels / 4 output pixels
         * wide. Loop it 16 times to cover 256 output pixels.
         */

        for (x = 16; x; x--) {
            byte      = *(in++);
            color     = pal00_pal31[byte >> 6];
            byte    <<= 2;
            color    += pal01_pal30[byte >> 6];
            *(out++)  = color;
            color     = pal10_pal21[byte >> 6];
            byte    <<= 2;
            color    += pal11_pal20[byte >> 6];
            *(out++)  = color;
            color     = pal11_pal20[byte >> 6];
            byte    <<= 2;
            color    += pal10_pal21[byte >> 6];
            *(out++)  = color;
            color     = pal01_pal30[byte >> 6];
            byte      = *(in++);
            color    += pal00_pal31[byte >> 6];
            *(out++)  = color;

            byte    <<= 2;
            color     = pal00_pal31[byte >> 6];
            byte    <<= 2;
            color    += pal01_pal30[byte >> 6];
            *(out++)  = color;
            color     = pal10_pal21[byte >> 6];
            byte    <<= 2;
            color    += pal11_pal20[byte >> 6];
            *(out++)  = color;
            color     = pal11_pal20[byte >> 6];
            byte      = *(in++);
            color    += pal10_pal21[byte >> 6];
            *(out++)  = color;
            color     = pal01_pal30[byte >> 6];
            byte    <<= 2;
            color    += pal00_pal31[byte >> 6];
            *(out++)  = color;

            byte    <<= 2;
            color     = pal00_pal31[byte >> 6];
            byte    <<= 2;
            color    += pal01_pal30[byte >> 6];
            *(out++)  = color;
            color     = pal10_pal21[byte >> 6];
            byte      = *(in++);
            color    += pal11_pal20[byte >> 6];
            *(out++)  = color;
            color     = pal11_pal20[byte >> 6];
            byte    <<= 2;
            color    += pal10_pal21[byte >> 6];
            *(out++)  = color;
            color     = pal01_pal30[byte >> 6];
            byte    <<= 2;
            color    += pal00_pal31[byte >> 6];
            *(out++)  = color;

            byte    <<= 2;
            color     = pal00_pal31[byte >> 6];
            byte      = *(in++);
            color    += pal01_pal30[byte >> 6];
            *(out++)  = color;
            color     = pal10_pal21[byte >> 6];
            byte    <<= 2;
            color    += pal11_pal20[byte >> 6];
            *(out++)  = color;
            color     = pal11_pal20[byte >> 6];
            byte    <<= 2;
            color    += pal10_pal21[byte >> 6];
            *(out++)  = color;
            color     = pal01_pal30[byte >> 6];
            byte    <<= 2;
            color    += pal00_pal31[byte >> 6];
            *(out++)  = color;
        }

        /* Skip every other line (CGA interlacing) */
        out += 256;
    }
}

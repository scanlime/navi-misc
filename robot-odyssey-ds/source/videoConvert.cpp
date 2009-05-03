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

#include <nds.h>
#include "videoConvert.h"


/*
 * Palette for 16-color sprites.
 *
 * This begins with the normal CGA palette, followed by the six colors
 * that result from blending two CGA colors together.
 */

const uint16_t VideoConvert::palette[16] = {
    0x8000,                            // 0
    0x8000 | RGB8(0x55, 0xFF, 0xFF),   // 1
    0x8000 | RGB8(0xFF, 0x55, 0xFF),   // 2
    0x8000 | RGB8(0xFF, 0xFF, 0xFF),   // 3
    0x8000 | RGB8(0x2A, 0x80, 0x80),   // 4 = 0+1
    0x8000 | RGB8(0x80, 0x2A, 0x80),   // 5 = 0+2
    0x8000 | RGB8(0x80, 0x80, 0x80),   // 6 = 0+3
    0x8000 | RGB8(0xAA, 0xAA, 0xFF),   // 7 = 1+2
    0x8000 | RGB8(0xAA, 0xFF, 0xFF),   // 8 = 1+3
    0x8000 | RGB8(0xFF, 0xAA, 0xFF),   // 9 = 2+3
};


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


/*
 * Expand and swizzle 4 pixels from 2bpp to 4bpp
 */
static inline uint16_t expand2to4(uint8_t byte) {
    return (0x3000 & (byte << 12)) |
           (0x0300 & (byte <<  6)) |
           (0x0030 & (byte      )) |
           (0x0003 & (byte >>  6));
}


/*
 * Convert a subrectangle of a CGA framebuffer into a 16-color sprite
 * compatible with Nintendo DS hardware. Only colors 0-3 are used.
 *
 * 'x' must be a multiple of 4. 'y' must be a multiple of 2. width and
 * height must be multiples of 8. (The sprite format consists of a
 * grid of 8x8 tiles.)
 */
void ITCM_CODE VideoConvert::CGAto16ColorTiles(uint8_t *cgaBuffer,
                                               uint16_t *fb4,
                                               uint32_t x,
                                               uint32_t y,
                                               uint32_t width,
                                               uint32_t height)
{
    sassert((x & 3) == 0, "x must be a mulitple of 4");
    sassert((y & 1) == 0, "y must be a mulitple of 2");
    sassert((width & 7) == 0, "width must be a mulitple of 8");
    sassert((height & 7) == 0, "height must be a mulitple of 8");

    uint32_t tileWidth = width >> 3;
    uint32_t tileHeight = height >> 3;

    cgaBuffer += (x >> 2) + (y >> 1) * 80;

    while (tileHeight--) {
        uint8_t *line = cgaBuffer;

        cgaBuffer += 80 * 4;

        uint32_t i = tileWidth;
        while (i--) {
            /*
             * Generate one 8x8 tile.
             * This is 16 words, in a 2x4 grid, with 4 pixels per word.
             */

            *(fb4++) = expand2to4(line[0 + 80*0]);
            *(fb4++) = expand2to4(line[1 + 80*0]);
            *(fb4++) = expand2to4(line[0 + 80*0 + 0x2000]);
            *(fb4++) = expand2to4(line[1 + 80*0 + 0x2000]);
            *(fb4++) = expand2to4(line[0 + 80*1]);
            *(fb4++) = expand2to4(line[1 + 80*1]);
            *(fb4++) = expand2to4(line[0 + 80*1 + 0x2000]);
            *(fb4++) = expand2to4(line[1 + 80*1 + 0x2000]);
            *(fb4++) = expand2to4(line[0 + 80*2]);
            *(fb4++) = expand2to4(line[1 + 80*2]);
            *(fb4++) = expand2to4(line[0 + 80*2 + 0x2000]);
            *(fb4++) = expand2to4(line[1 + 80*2 + 0x2000]);
            *(fb4++) = expand2to4(line[0 + 80*3]);
            *(fb4++) = expand2to4(line[1 + 80*3]);
            *(fb4++) = expand2to4(line[0 + 80*3 + 0x2000]);
            *(fb4++) = expand2to4(line[1 + 80*3 + 0x2000]);

            line += 2;
        }
    }
}


/*
 * Convert a subrectangle of a CGA framebuffer into a 16-color sprite
 * compatible with Nintendo DS hardware. Each 2x1 pixel "Widepixel" in
 * the original framebuffer is converted to one output pixel. Uses a
 * 10-color palette which contains all color combinations that can
 * result by blending two CGA colors.
 *
 * 'x' and 'width' are specified in widepixel units.
 *
 * 'x' must be a multiple of 2. 'y' must be a multiple of 2. width and
 * height must be multiples of 8. (The sprite format consists of a
 * grid of 8x8 tiles.)
 */
void ITCM_CODE VideoConvert::CGAWideto16ColorTiles(uint8_t *cgaBuffer,
                                                   uint16_t *fb4,
                                                   uint32_t x,
                                                   uint32_t y,
                                                   uint32_t width,
                                                   uint32_t height)
{
    sassert((x & 3) == 0, "x must be a mulitple of 4");
    sassert((y & 1) == 0, "y must be a mulitple of 2");
    sassert((width & 7) == 0, "width must be a mulitple of 8");
    sassert((height & 7) == 0, "height must be a mulitple of 8");

    /*
     * Lookup table for unpacking one CGA byte into one 16-color byte.
     * Generated by gen-cgawide-table.py. This operates on two
     * widepixels at a time.
     */
    static const DTCM_DATA uint8_t lut8[] = {
        0x00, 0x40, 0x50, 0x60, 0x40, 0x10, 0x70, 0x80,
        0x50, 0x70, 0x20, 0x90, 0x60, 0x80, 0x90, 0x30,
        0x04, 0x44, 0x54, 0x64, 0x44, 0x14, 0x74, 0x84,
        0x54, 0x74, 0x24, 0x94, 0x64, 0x84, 0x94, 0x34,
        0x05, 0x45, 0x55, 0x65, 0x45, 0x15, 0x75, 0x85,
        0x55, 0x75, 0x25, 0x95, 0x65, 0x85, 0x95, 0x35,
        0x06, 0x46, 0x56, 0x66, 0x46, 0x16, 0x76, 0x86,
        0x56, 0x76, 0x26, 0x96, 0x66, 0x86, 0x96, 0x36,
        0x04, 0x44, 0x54, 0x64, 0x44, 0x14, 0x74, 0x84,
        0x54, 0x74, 0x24, 0x94, 0x64, 0x84, 0x94, 0x34,
        0x01, 0x41, 0x51, 0x61, 0x41, 0x11, 0x71, 0x81,
        0x51, 0x71, 0x21, 0x91, 0x61, 0x81, 0x91, 0x31,
        0x07, 0x47, 0x57, 0x67, 0x47, 0x17, 0x77, 0x87,
        0x57, 0x77, 0x27, 0x97, 0x67, 0x87, 0x97, 0x37,
        0x08, 0x48, 0x58, 0x68, 0x48, 0x18, 0x78, 0x88,
        0x58, 0x78, 0x28, 0x98, 0x68, 0x88, 0x98, 0x38,
        0x05, 0x45, 0x55, 0x65, 0x45, 0x15, 0x75, 0x85,
        0x55, 0x75, 0x25, 0x95, 0x65, 0x85, 0x95, 0x35,
        0x07, 0x47, 0x57, 0x67, 0x47, 0x17, 0x77, 0x87,
        0x57, 0x77, 0x27, 0x97, 0x67, 0x87, 0x97, 0x37,
        0x02, 0x42, 0x52, 0x62, 0x42, 0x12, 0x72, 0x82,
        0x52, 0x72, 0x22, 0x92, 0x62, 0x82, 0x92, 0x32,
        0x09, 0x49, 0x59, 0x69, 0x49, 0x19, 0x79, 0x89,
        0x59, 0x79, 0x29, 0x99, 0x69, 0x89, 0x99, 0x39,
        0x06, 0x46, 0x56, 0x66, 0x46, 0x16, 0x76, 0x86,
        0x56, 0x76, 0x26, 0x96, 0x66, 0x86, 0x96, 0x36,
        0x08, 0x48, 0x58, 0x68, 0x48, 0x18, 0x78, 0x88,
        0x58, 0x78, 0x28, 0x98, 0x68, 0x88, 0x98, 0x38,
        0x09, 0x49, 0x59, 0x69, 0x49, 0x19, 0x79, 0x89,
        0x59, 0x79, 0x29, 0x99, 0x69, 0x89, 0x99, 0x39,
        0x03, 0x43, 0x53, 0x63, 0x43, 0x13, 0x73, 0x83,
        0x53, 0x73, 0x23, 0x93, 0x63, 0x83, 0x93, 0x33,
    };

    uint32_t tileWidth = width >> 3;
    uint32_t tileHeight = height >> 3;

    cgaBuffer += (x >> 1) + (y >> 1) * 80;

    while (tileHeight--) {
        uint8_t *line = cgaBuffer;

        cgaBuffer += 80 * 4;

        uint32_t i = tileWidth;
        while (i--) {
            /*
             * Generate one 8x8 tile.
             * This is 16 words, in a 2x4 grid, with 4 pixels per word.
             * The input is 16x8 pixels (4 bytes wide).
             */

            *(fb4++) =  lut8[line[0 + 80*0]] |
                       (lut8[line[1 + 80*0]] <<8);
            *(fb4++) =  lut8[line[2 + 80*0]] |
                       (lut8[line[3 + 80*0]] <<8);
            *(fb4++) =  lut8[line[0 + 80*0 + 0x2000]] |
                       (lut8[line[1 + 80*0 + 0x2000]] <<8);
            *(fb4++) =  lut8[line[2 + 80*0 + 0x2000]] |
                       (lut8[line[3 + 80*0 + 0x2000]] <<8);

            *(fb4++) =  lut8[line[0 + 80*1]] |
                       (lut8[line[1 + 80*1]] <<8);
            *(fb4++) =  lut8[line[2 + 80*1]] |
                       (lut8[line[3 + 80*1]] <<8);
            *(fb4++) =  lut8[line[0 + 80*1 + 0x2000]] |
                       (lut8[line[1 + 80*1 + 0x2000]] <<8);
            *(fb4++) =  lut8[line[2 + 80*1 + 0x2000]] |
                       (lut8[line[3 + 80*1 + 0x2000]] <<8);

            *(fb4++) =  lut8[line[0 + 80*2]] |
                       (lut8[line[1 + 80*2]] <<8);
            *(fb4++) =  lut8[line[2 + 80*2]] |
                       (lut8[line[3 + 80*2]] <<8);
            *(fb4++) =  lut8[line[0 + 80*2 + 0x2000]] |
                       (lut8[line[1 + 80*2 + 0x2000]] <<8);
            *(fb4++) =  lut8[line[2 + 80*2 + 0x2000]] |
                       (lut8[line[3 + 80*2 + 0x2000]] <<8);

            *(fb4++) =  lut8[line[0 + 80*3]] |
                       (lut8[line[1 + 80*3]] <<8);
            *(fb4++) =  lut8[line[2 + 80*3]] |
                       (lut8[line[3 + 80*3]] <<8);
            *(fb4++) =  lut8[line[0 + 80*3 + 0x2000]] |
                       (lut8[line[1 + 80*3 + 0x2000]] <<8);
            *(fb4++) =  lut8[line[2 + 80*3 + 0x2000]] |
                       (lut8[line[3 + 80*3 + 0x2000]] <<8);

            line += 4;
        }
    }
}


/*
 * Quickly clear a subrectangle of the CGA framebuffer.
 *
 * 'x' and 'width' must be multiples of 16. 'y' and 'height' must be
 * multiples of 2. The CGA framebuffer itself must be word aligned.
 */
void ITCM_CODE VideoConvert::CGAclear(uint8_t *cgaBuffer,
                                      uint32_t x,
                                      uint32_t y,
                                      uint32_t width,
                                      uint32_t height)
{
    sassert((x & 15) == 0, "x must be a mulitple of 16");
    sassert((y & 1) == 0, "y must be a mulitple of 2");
    sassert((width & 15) == 0, "width must be a mulitple of 16");
    sassert((height & 1) == 0, "height must be a mulitple of 2");
    sassert(((uintptr_t)cgaBuffer & 3) == 0, "framebuffer must be word aligned");

    uint32_t tileWidth = width >> 4;
    uint32_t tileHeight = height >> 1;
    uint32_t *cga32 = (uint32_t*) (cgaBuffer + (x >> 2) + (y >> 1) * 80);

    while (tileHeight--) {
        uint32_t *line = cga32;

        cga32 += 80 / sizeof *cga32;

        uint32_t i = tileWidth;
        while (i--) {
            line[0] = 0;
            line[0x2000 / sizeof *cga32] = 0;
            line++;
        }
    }
}


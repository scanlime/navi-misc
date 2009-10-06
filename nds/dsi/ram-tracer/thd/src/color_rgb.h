/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-
 *
 * color_rgb.h -- A utility class for storing 32-bit BGRX colors.
 *
 * Copyright (C) 2009 Micah Dowty
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef __COLOR_RGB_H
#define __COLOR_RGB_H

#include <stdint.h>

struct ColorRGB {
    uint32_t value;

    ColorRGB() : value(0) {}
    ColorRGB(uint32_t v) : value(v) {}
    ColorRGB(uint8_t r, uint8_t g, uint8_t b) : value((r << 16) | (g << 8) | b) {}

    uint8_t red() const { return value >> 16; }
    uint8_t green() const { return value >> 8; }
    uint8_t blue() const { return value; }

    operator uint32_t() const { return value; }
    operator int() const { return value; };

    ColorRGB operator +(const ColorRGB &b)
    {
        // Add with saturate

        const uint32_t mask1 = 0x0000FF00;
        const uint32_t mask2 = 0x00FF00FF;
        const uint32_t a1 = value & mask1;
        const uint32_t b1 = b.value & mask1;
        const uint32_t a2 = value & mask2;
        const uint32_t b2 = b.value & mask2;
        const uint32_t sum1 = a1 + b1;
        const uint32_t sum2 = a2 + b2;

        uint32_t sum1m = sum1 & mask1;
        uint32_t sum2m = sum2 & mask2;

        if (sum1 & 0x00010000)
            sum1m |= 0x0000FF00;

        if (sum2 & 0x01000000)
            sum2m |= 0x00FF0000;

        if (sum2 & 0x00000100)
            sum2m |= 0x000000FF;

        return sum1m | sum2m;
    }

    ColorRGB operator -(const ColorRGB &b)
    {
        // Subtract with saturate

        const uint32_t mask1 = 0x0000FF00;
        const uint32_t mask2 = 0x00FF00FF;
        const uint32_t a1 = (value & mask1) | 0x00010000;
        const uint32_t b1 = b.value & mask1;
        const uint32_t a2 = (value & mask2) | 0x01000100;
        const uint32_t b2 = b.value & mask2;

        const uint32_t diff1 = a1 - b1;
        const uint32_t diff2 = a2 - b2;

        uint32_t diff1m = diff1 & mask1;
        uint32_t diff2m = diff2 & mask2;

        if (!(diff1 & 0x00010000))
            diff1m &= 0xFFFF00FF;

        if (!(diff2 & 0x01000000))
            diff2m &= 0xFF00FFFF;

        if (!(diff2 & 0x00000100))
            diff2m &= 0xFFFFFF00;

        return diff1m | diff2m;
    }

    ColorRGB operator *(float b)
    {
        // Floating point multiply

        if (b <= 0)
            return ColorRGB(0);
        if (b >= 255)
            b = 255;

        const uint32_t mask1 = 0x0000FF00;
        const uint32_t mask2 = 0x00FF00FF;
        const uint32_t a1 = value & mask1;
        const uint32_t a2 = value & mask2;

        const uint32_t r1 = a1 * b;
        const uint32_t r2 = a2 * b;

        uint32_t r1m = r1 & mask1;
        uint32_t r2m = r2 & mask2;

        if (r1 & 0xFFFF0000)
            r1m |= 0x0000FF00;

        if (r2 & 0xFF000000)
            r2m |= 0x00FF0000;

        if (r2 & 0x00000100)
            r2m |= 0x000000FF;

        return r1m | r2m;
    }

    ColorRGB operator *(uint8_t b)
    {
        // Integer multiply

        const uint32_t mask1 = 0x0000FF00;
        const uint32_t mask2 = 0x00FF00FF;
        const uint32_t a1 = value & mask1;
        const uint32_t a2 = value & mask2;

        const uint32_t r1 = (a1 * b) >> 8;
        const uint32_t r2 = (a2 * b) >> 8;

        uint32_t r1m = r1 & mask1;
        uint32_t r2m = r2 & mask2;

        if (r1 & 0xFFFF0000)
            r1m |= 0x0000FF00;

        if (r2 & 0xFF000000)
            r2m |= 0x00FF0000;

        if (r2 & 0x00000100)
            r2m |= 0x000000FF;

        return r1m | r2m;
    }

    ColorRGB operator +=(const ColorRGB &b)
    {
        value = (*this + b).value;
        return *this;
    }

    ColorRGB operator -=(const ColorRGB &b)
    {
        value = (*this - b).value;
        return *this;
    }

    ColorRGB operator *=(const float &b)
    {
        value = (*this * b).value;
        return *this;
    }

    ColorRGB operator *=(const uint8_t &b)
    {
        value = (*this * b).value;
        return *this;
    }
};


#endif /* __COLOR_RGB_H */

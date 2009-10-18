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

#include <wx/colour.h>
#include <stdint.h>

struct ColorRGB {
    uint32_t value;

    ColorRGB() : value(0) {}
    ColorRGB(uint32_t v) : value(v) {}
    ColorRGB(uint8_t r, uint8_t g, uint8_t b)
        : value((r << 16) | (g << 8) | b) {}
    ColorRGB(wxColour c)
        : value((c.Red() << 16) | (c.Green() << 8) | c.Blue()) {}

    uint8_t red() const { return value >> 16; }
    uint8_t green() const { return value >> 8; }
    uint8_t blue() const { return value; }

    ColorRGB blend(ColorRGB other, uint8_t alpha)
    {
        uint8_t alphaPrime = 0xff - alpha;
        return *this * alphaPrime + (other * alpha);
    }

    ColorRGB blend(uint32_t argb)
    {
        return blend(ColorRGB(argb), argb >> 24);
    }

    operator uint32_t() const { return value; }
    operator int() const { return value; };

    operator wxColour() const
    {
        return wxColour(red(), green(), blue());
    }

    ColorRGB operator +(const ColorRGB b)
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

    ColorRGB operator -(const ColorRGB b)
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

    ColorRGB operator *(float a)
    {
        // Floating point multiply and saturate

        if (a <= 0)
            return ColorRGB(0);
        if (a >= 255)
            a = 255;

        int r = std::min<int>(255, std::max<int>(0, a * (int)red()));
        int g = std::min<int>(255, std::max<int>(0, a * (int)green()));
        int b = std::min<int>(255, std::max<int>(0, a * (int)blue()));

        return ColorRGB(r, g, b);
    }

    ColorRGB operator *(uint8_t b)
    {
        // Fixed-point integer multiply

        const uint32_t mask1 = 0x0000FF00;
        const uint32_t mask2 = 0x00FF00FF;
        const uint32_t a1 = value & mask1;
        const uint32_t a2 = value & mask2;

        const uint32_t r1 = (a1 * b);
        const uint32_t r2 = (a2 * b);

        uint32_t r1m = r1 & (mask1 << 8);
        uint32_t r2m = r2 & (mask2 << 8);

        return (r1m | r2m) >> 8;
    }

    ColorRGB operator +=(const ColorRGB b)
    {
        value = (*this + b).value;
        return *this;
    }

    ColorRGB operator -=(const ColorRGB b)
    {
        value = (*this - b).value;
        return *this;
    }

    ColorRGB operator *=(const float b)
    {
        value = (*this * b).value;
        return *this;
    }

    ColorRGB operator *=(const uint8_t b)
    {
        value = (*this * b).value;
        return *this;
    }
};


/*
 * A 16-bit-per-channel color accumulator
 */
struct ColorAccumulator {
    int16_t red;
    int16_t green;
    int16_t blue;

    ColorAccumulator()
        : red(0), green(0), blue(0) {}

    ColorAccumulator(int16_t r, int16_t g, int16_t b)
        : red(r), green(g), blue(b) {}

    ColorAccumulator(ColorRGB c)
        : red(c.red()), green(c.green()), blue(c.blue()) {}

    ColorAccumulator operator +=(const ColorRGB b)
    {
        red += (int16_t) b.red();
        green += (int16_t) b.green();
        blue += (int16_t) b.blue();
        return *this;
    }

    ColorAccumulator operator -=(const ColorRGB b)
    {
        red -= (int16_t) b.red();
        green -= (int16_t) b.green();
        blue -= (int16_t) b.blue();
        return *this;
    }

    ColorAccumulator operator >>=(int s)
    {
        red >>= s;
        green >>= s;
        blue >>= s;
        return *this;
    }

    ColorAccumulator operator <<=(int s)
    {
        red <<= s;
        green <<= s;
        blue <<= s;
        return *this;
    }

    operator ColorRGB() const {
        return ColorRGB(red, green, blue);
    }
};


#endif /* __COLOR_RGB_H */

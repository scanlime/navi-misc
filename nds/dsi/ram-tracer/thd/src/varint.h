/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-
 *
 * varint.h -- Utilities for reading and writing variable-width integers
 *             that can store up to 56 bits of data using from 1 to 8 bytes,
 *             using a UTF-8 style encoding.
 *
 * Integer encoding:
 *
 *   The variable length integers are formatted as follows, shown
 *   in binary:
 *
 *     x < 0x80              1xxxxxxx
 *     x < 0x4000            01xxxxxx xxxxxxxx
 *     x < 0x200000          001xxxxx xxxxxxxx xxxxxxxx
 *     x < 0x10000000        0001xxxx xxxxxxxx xxxxxxxx xxxxxxxx
 *     ...
 *     Flag byte             00000000
 *
 *   The largest integer length that can be represented is 56-bits,
 *   which will be prefixed by 0x01. No valid integer may start with a
 *   zero byte. This value is reserved for use as a special 'flag'
 *   byte, which may have an application-specific use as a delimiter
 *   or termination token.
 *
 * Historical note:
 *
 *   This code was originally written for the 'fidtool' graphing
 *   library, which was part of my CIA project. It was then released
 *   under the LGPL, but this version is being rereleased in a more
 *   generic form, "enhanced" for C++, and with an MIT-style license.
 *
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

#ifndef __VARINT_H
#define __VARINT_H

#include <stdint.h>

struct varint {

    typedef uint64_t varint_t;

    // Normal data range
    static const varint_t MIN = 0;
    static const varint_t MAX = (1LL << 56) - 1;

    // Special values
    static const varint_t FLAG = (varint_t) -1;
    static const varint_t FENCE = (varint_t) -2;

    /*
     * Read a sample forward, incrementing 'p' to point just past the end
     * of the sample on a successful read. If any byte in the sample would
     * have been read from 'fence', this does not change p and returns FENCE.
     * Memory will never be read from addresses greater than or equal to 'fence'.
     */
    static varint_t
    read(const uint8_t *&p, const uint8_t *fence)
    {
        if (p >= fence)
            return FENCE;

        uint8_t c = *p;

        if (c & 0x80) {
            p += 1;
            return c & 0x7F;
        }

        if (c & 0x40) {
            if (p + 1 >= fence)
                return FENCE;
            p += 2;
            return ((c & 0x3F) << 8) | p[1];
        }

        if (c == 0)
            return FLAG;

        if (c & 0x20) {
            if (p + 2 >= fence)
                return FENCE;
            p += 3;
            return ((c & 0x1F) << 16) | (p[1] << 8) | p[2];
        }

        if (c & 0x10) {
            if (p + 3 >= fence)
                return FENCE;
            p += 4;
            return ((c & 0x0F) << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
        }

        if (c & 0x08) {
            if (p + 4 >= fence)
                return FENCE;
            p += 5;
            return (((varint_t) c & 0x07) << 32) | (p[1] << 24) |
                (p[2] << 16) | (p[3] << 8) | p[4];
        }

        if (c & 0x04) {
            if (p + 5 >= fence)
                return FENCE;
            p += 6;
            return (((varint_t) c & 0x03) << 40) | (((varint_t) p[1]) << 32) |
                (p[2] << 24) | (p[3] << 16) | (p[4] << 8) | p[5];
        }

        if (c & 0x02) {
            if (p + 6 >= fence)
                return FENCE;
            p += 7;
            return (((varint_t) c & 0x01) << 48) | (((varint_t) p[1]) << 40) |
                (((varint_t) p[2]) << 32) | (p[3] << 24) | (p[4] << 16) |
                (p[5] << 8) | p[6];
        }

        if (p + 7 >= fence)
            return FENCE;
        p += 8;
        return (((varint_t) p[1]) << 48) | (((varint_t) p[2]) << 40) |
            (((varint_t) p[3]) << 32) | (p[4] << 24) | (p[5] << 16) |
            (p[6] << 8) | p[7];
    }

    /*
     * A reversed version of read(), where memory addresses move
     * downward rather than upward.
     */
    static varint_t
    read_r(const uint8_t *&p, const uint8_t *fence)
    {
        if (p <= fence)
            return FENCE;

        uint8_t c = *p;

        if (c & 0x80) {
            p -= 1;
            return c & 0x7F;
        }

        if (c & 0x40) {
            if (p - 1 <= fence)
                return FENCE;
            p -= 2;
            return ((c & 0x3F) << 8) | p[-1];
        }

        if (c == 0)
            return FLAG;

        if (c & 0x20) {
            if (p - 2 <= fence)
                return FENCE;
            p -= 3;
            return ((c & 0x1F) << 16) | (p[-1] << 8) | p[-2];
        }

        if (c & 0x10) {
            if (p - 3 <= fence)
                return FENCE;
            p -= 4;
            return ((c & 0x0F) << 24) | (p[-1] << 16) | (p[-2] << 8) | p[-3];
        }

        if (c & 0x08) {
            if (p - 4 <= fence)
                return FENCE;
            p -= 5;
            return (((varint_t) c & 0x07) << 32) | (p[-1] << 24) |
                (p[-2] << 16) | (p[-3] << 8) | p[-4];
        }

        if (c & 0x04) {
            if (p - 5 <= fence)
                return FENCE;
            p -= 6;
            return (((varint_t) c & 0x03) << 40) | (((varint_t) p[-1]) << 32) |
                (p[-2] << 24) | (p[-3] << 16) | (p[-4] << 8) | p[-5];
        }

        if (c & 0x02) {
            if (p - 6 <= fence)
                return FENCE;
            p -= 7;
            return (((varint_t) c & 0x01) << 48) | (((varint_t) p[-1]) << 40) |
                (((varint_t) p[-2]) << 32) | (p[-3] << 24) | (p[-4] << 16) |
                (p[-5] << 8) | p[-6];
        }

        if (p - 7 <= fence)
            return FENCE;
        p -= 8;
        return (((varint_t) p[-1]) << 48) | (((varint_t) p[-2]) << 40) |
            (((varint_t) p[-3]) << 32) | (p[-4] << 24) | (p[-5] << 16) |
            (p[-6] << 8) | p[-7];
    }

    /*
     * Return the length, in bytes, necessary to store a value.
     * This assumes the value fits in our 56-bit limit.
     */
    static int __const
    len(varint_t s)
    {
        if (s < 0x80) return 1; /* This case also works for FLAG */
        if (s < 0x4000) return 2;
        if (s < 0x200000) return 3;
        if (s < 0x10000000) return 4;
        if (s < 0x0800000000LL) return 5;
        if (s < 0x040000000000LL) return 6;
        if (s < 0x02000000000000LL) return 7;
        return 8;
    }

    /*
     * Write a sample at the provided address. This does not increment
     * the pointer, or perform any boundary checking. The sample may not
     * be FLAG.
     */
    static void
    write(varint_t s, uint8_t *p)
    {
        if (s < 0x80) {
            p[0] = 0x80 | s;
        }
        else if (s < 0x4000) {
            p[0] = 0x40 | (s >> 8);
            p[1] = s;
        }
        else if (s < 0x200000) {
            p[0] = 0x20 | (s >> 16);
            p[1] = s >> 8;
            p[2] = s;
        }
        else if (s < 0x10000000) {
            p[0] = 0x10 | (s >> 24);
            p[1] = s >> 16;
            p[2] = s >> 8;
            p[3] = s;
        }
        else if (s < 0x0800000000LL) {
            p[0] = 0x08 | (s >> 32);
            p[1] = s >> 24;
            p[2] = s >> 16;
            p[3] = s >> 8;
            p[4] = s;
        }
        else if (s < 0x040000000000LL) {
            p[0] = 0x04 | (s >> 40);
            p[1] = s >> 32;
            p[2] = s >> 24;
            p[3] = s >> 16;
            p[3] = s >> 8;
            p[4] = s;
        }
        else if (s < 0x02000000000000LL) {
            p[0] = 0x02 | (s >> 48);
            p[1] = s >> 40;
            p[2] = s >> 32;
            p[3] = s >> 24;
            p[4] = s >> 16;
            p[5] = s >> 8;
            p[6] = s;
        }
        else {
            p[0] = 0x01;
            p[1] = s >> 48;
            p[2] = s >> 40;
            p[3] = s >> 32;
            p[4] = s >> 24;
            p[5] = s >> 16;
            p[6] = s >> 8;
            p[7] = s;
        }
    }

    /* A reversed version of sample_write */
    static void
    write_r(varint_t s, uint8_t *p)
    {
        if (s < 0x80) {
            p[0] = 0x80 | s;
        }
        else if (s < 0x4000) {
            p[0] = 0x40 | (s >> 8);
            p[-1] = s;
        }
        else if (s < 0x200000) {
            p[0] = 0x20 | (s >> 16);
            p[-1] = s >> 8;
            p[-2] = s;
        }
        else if (s < 0x10000000) {
            p[0] = 0x10 | (s >> 24);
            p[-1] = s >> 16;
            p[-2] = s >> 8;
            p[-3] = s;
        }
        else if (s < 0x0800000000LL) {
            p[0] = 0x08 | (s >> 32);
            p[-1] = s >> 24;
            p[-2] = s >> 16;
            p[-3] = s >> 8;
            p[-4] = s;
        }
        else if (s < 0x040000000000LL) {
            p[0] = 0x04 | (s >> 40);
            p[-1] = s >> 32;
            p[-2] = s >> 24;
            p[-3] = s >> 16;
            p[-3] = s >> 8;
            p[-4] = s;
        }
        else if (s < 0x02000000000000LL) {
            p[0] = 0x02 | (s >> 48);
            p[-1] = s >> 40;
            p[-2] = s >> 32;
            p[-3] = s >> 24;
            p[-4] = s >> 16;
            p[-5] = s >> 8;
            p[-6] = s;
        }
        else {
            p[0] = 0x01;
            p[-1] = s >> 48;
            p[-2] = s >> 40;
            p[-3] = s >> 32;
            p[-4] = s >> 24;
            p[-5] = s >> 16;
            p[-6] = s >> 8;
            p[-7] = s;
        }
    }
};


#endif /* __VARINT_H */

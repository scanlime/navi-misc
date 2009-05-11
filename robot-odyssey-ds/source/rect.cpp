/* -*- Mode: C++; c-basic-offset: 4 -*-
 *
 * Rectangles and dirty rectangle lists.
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
#include <algorithm>
#include "rect.h"


//********************************************************** Rect


Rect::Rect(int x, int y, int width, int height) {
    left = x;
    top = y;
    right = x + width;
    bottom = y + height;
}

Rect::Rect() {
    left = top = right = bottom = 0;
}

int Rect::getX() {
    return left;
}

int Rect::getY() {
    return top;
}

bool Rect::isEmpty() {
    return (bottom <= top || right <= left);
}

int Rect::getWidth() {
    if (right > left) {
        return right - left;
    } else {
        return 0;
    }
}

int Rect::getHeight() {
    if (bottom > top) {
        return bottom - top;
    } else {
        return 0;
    }
}

bool Rect::containsPoint(int x, int y) {
    return (x >= left && x < right &&
            y >= top && y < bottom);
}

Rect Rect::intersectWith(Rect &r) {
    Rect result;

    result.left = std::max(left, r.left);
    result.top = std::max(top, r.top);
    result.right = std::min(right, r.right);
    result.bottom = std::min(bottom, r.bottom);

    return result;
}

Rect Rect::unionWith(Rect &r) {
    Rect result;

    result.left = std::min(left, r.left);
    result.top = std::min(top, r.top);
    result.right = std::max(right, r.right);
    result.bottom = std::max(bottom, r.bottom);

    return result;
}

bool Rect::touchesRect(Rect &r) {
    return !intersectWith(r).isEmpty();
}

Rect Rect::align(int mx, int my) {
    /*
     * Round the rectangle outward so that all horizontal edges are a
     * multiple of 'mx' and vertical edges are a multiple of 'my'. The
     * arguments must both be a power of two.
     */

    Rect result;
    int maskX = mx - 1;
    int maskY = my - 1;

    sassert((maskX & mx) == 0 && (maskY & my) == 0,
            "Rect alignment must be a power of 2");

    result.left = left & ~maskX;
    result.top = top & ~maskY;
    result.right = (maskX + right) & ~maskX;
    result.bottom = (maskY + bottom) & ~maskY;

    return result;
}

bool Rect::isAligned(int mx, int my) {
    int maskX = mx - 1;
    int maskY = my - 1;

    sassert((maskX & mx) == 0 && (maskY & my) == 0,
            "Rect alignment must be a power of 2");

    return (left & maskX) == 0 &&
           (right & maskX) == 0 &&
           (top & maskY) == 0 &&
           (bottom & maskY) == 0;
}

Rect Rect::adjacentAbove(int height) {
    Rect result;

    result.left = left;
    result.top = top - height;
    result.right = right;
    result.bottom = top;

    return result;
}

Rect Rect::adjacentBelow(int height) {
    Rect result;

    result.left = left;
    result.top = bottom;
    result.right = right;
    result.bottom = bottom + height;

    return result;
}

Rect Rect::adjacentLeft(int width) {
    Rect result;

    result.left = left - width;
    result.top = top;
    result.right = left;
    result.bottom = bottom;

    return result;
}

Rect Rect::adjacentRight(int width) {
    Rect result;

    result.left = right;
    result.top = top;
    result.right = right + width;
    result.bottom = bottom;

    return result;
}

Rect Rect::expand(int left, int top, int right, int bottom) {
    Rect result;

    result.left = this->left - left;
    result.top = this->top - top;
    result.right = this->right + right;
    result.bottom = this->bottom + bottom;

    return result;
}

Rect Rect::expand(int size) {
    return expand(size, size, size, size);
}


//********************************************************** DirtyRectTracker


void DirtyRectTracker::clear() {
    rects.clear();
}

void DirtyRectTracker::add(Rect r) {
    Rect merged = r;

    while (mergeRects(merged));
    rects.push_back(merged);
}

bool DirtyRectTracker::mergeRects(Rect &r) {
    /*
     * Find rectangles which intersect with 'r'.  If we find a match,
     * union it with 'r' and delete it from 'rects', then return
     * true. If we don't find any matches, return false.
     */

    std::vector<Rect>::iterator i;

    for (i = rects.begin(); i != rects.end(); i++) {
        if (i->touchesRect(r)) {
            r = r.unionWith(*i);
            rects.erase(i);
            return true;
        }
    }
    return false;
}

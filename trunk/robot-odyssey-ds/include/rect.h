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

#ifndef _RECT_H_
#define _RECT_H_

#include <vector>


/*
 * A basic rectangle class, with integer dimensions.
 */
class Rect
{
 public:
    Rect();
    Rect(int x, int y, int width, int height);

    int getX();
    int getY();
    int getWidth();
    int getHeight();

    bool isEmpty();
    bool containsPoint(int x, int y);
    Rect intersectWith(Rect &r);
    Rect unionWith(Rect &r);
    bool touchesRect(Rect &r);
    Rect align(int mx, int my);
    bool isAligned(int mx, int my);

    Rect adjacentAbove(int height);
    Rect adjacentBelow(int height);
    Rect adjacentLeft(int width);
    Rect adjacentRight(int width);
    Rect expand(int left, int top, int right, int bottom);
    Rect expand(int size);

    int left, top, right, bottom;
};


/*
 * A dirty rectangle tracker. This is a set of rectangles which describe
 * a conservative bounding region around an area of the screen. Any pixel
 * on the screen is guaranteed to be described by at most one rectangle
 * in the list.
 */
class DirtyRectTracker
{
public:
    void clear();
    void add(Rect r);

    std::vector<Rect> rects;

private:
    bool mergeRects(Rect &r);
};


#endif // _RECT_H_

/* -*- Mode: C++; c-basic-offset: 4 -*-
 *
 * UI for listing items and selecting one.
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

#ifndef _UILIST_H_
#define _UILIST_H_

#include <vector>
#include "uiText.h"
#include "uiBase.h"

class UIListItem;


/*
 * List renderer. This is a vertically scrolling text layer
 * which asks each UIListItem to draw itself.
 */
class UIListDraw : public UITextLayer
{
public:
    UIListDraw(std::vector<UIListItem*> *_items, int _offsetX = 0, int _offsetY = 0)
        : currentItem(0),
          items(_items),
          offsetX(_offsetX),
          offsetY(_offsetY) {}

    virtual void paint();
    void paint(UIListItem *itemToPaint);

    int currentItem;

    int getOffsetX() {
        return offsetX;
    }

    int getOffsetY() {
        return offsetY;
    }

private:
    std::vector<UIListItem*> *items;
    int offsetX;
    int offsetY;
};


/*
 * A basic list UI. Draws a set of items, and provides controls to
 * select one.
 */

class UIList : public UITransient
{
public:
    UIList(int offsetX = 0, int offsetY = 0);
    virtual ~UIList();

    virtual void handleInput(const UIInputState &input);
    virtual void updateState();

    void append(UIListItem *item);

    int getCurrentIndex() {
        return draw.currentItem;
    }

    UIListItem *getCurrentItem() {
        return items[draw.currentItem];
    }

    void setCurrentIndex(int index);

 private:
    void getCurrentItemY(int &top, int &bottom);

    std::vector<UIListItem*> items;

    MSpriteAllocator sprAlloc;
    UIListDraw draw;
    bool needRepaint;
};


/*
 * Abstract base class for list items.
 */
class UIListItem
{
public:
    virtual void paint(UIListDraw *draw, int x, int y, bool hilight) = 0;
    virtual int getHeight() = 0;
};


/*
 * A list item for files. This is 32 pixels high with enough room for
 * two lines of text, and it has a boxy background.
 */

class UIFileListItem : public UIListItem
{
public:
    enum TextSlot {
        TEXT_TOP_LEFT = 0,
        TEXT_TOP_RIGHT,
        TEXT_BOTTOM_LEFT,
        TEXT_BOTTOM_RIGHT,
        TEXT_CENTER,
        TEXT_NUM_SLOTS  // Must be last
    };

    UIFileListItem();
    void setText(TextSlot slot, const char *format, ...);
    virtual void paint(UIListDraw *draw, int x, int y, bool hilight);
    virtual int getHeight();

private:
    static const int height = 30;
    static const int bufferLen = 80;
    char text[TEXT_NUM_SLOTS][80];
};


#endif // _UILIST_H_

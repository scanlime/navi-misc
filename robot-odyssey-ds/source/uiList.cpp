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

#include <stdio.h>
#include "uiList.h"


//********************************************************** UIList


UIList::UIList(int offsetX, int offsetY)
    : UITransient(),
      sprAlloc(&oamSub),
      draw(&items, offsetX, offsetY)
{
    draw.colors.setDefaultPalette();
    needRepaint = true;
}

UIList::~UIList()
{
    std::vector<UIListItem*>::iterator i;
    for (i = items.begin(); i != items.end(); i++) {
        delete *i;
    }
}

void UIList::handleInput(const UIInputState &input) {
    if (input.keysHeld == KEY_DOWN) {
        draw.scrollTo(draw.getScroll() + 3);
    }

    if (input.keysHeld == KEY_UP) {
        draw.scrollTo(draw.getScroll() - 3);
    }
}

void UIList::updateState() {
    if (needRepaint) {
        draw.paintAll();
        needRepaint = false;
    }

    UITransient::updateState();
}

void UIList::append(UIListItem *item) {
    items.push_back(item);
    needRepaint = true;
}


//********************************************************** UIListDraw


void UIListDraw::paint() {
    std::vector<UIListItem*>::iterator i;
    int x = offsetX;
    int y = offsetY;

    clear();

    for (i = items->begin(); i != items->end(); i++) {
        UIListItem *item = *i;
        int height = item->getHeight();

        if (y + height >= clip.y1 && y <= clip.y2) {
            item->paint(this, x, y);
        }

        y += height;
    }

    blit();
}


//********************************************************** UIFileListItem


void UIFileListItem::setText(TextSlot slot, const char *format, ...) {
    va_list v;

    va_start(v, format);
    vsnprintf(text[slot], sizeof text[slot], format, v);
    va_end(v);
}

void UIFileListItem::paint(UIListDraw *draw, int x, int y) {
    draw->moveTo(x + 5, y + 5);
    draw->printf("Foo! %p", this);
}

int UIFileListItem::getHeight() {
    return height;
}

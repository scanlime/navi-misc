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


UIList::UIList(int offsetX, int offsetY, int _hotkey)
    : UITransient(),
      hotkey(_hotkey),
      draw(&items, offsetX, offsetY),
      needRepaint(true)
{}

UIList::~UIList()
{
    std::vector<UIListItem*>::iterator i;
    for (i = items.begin(); i != items.end(); i++) {
        delete *i;
    }
}

void UIList::handleInput(const UIInputState &input) {
    if (input.keysPressedWithRepeat & KEY_DOWN) {
        setCurrentIndex(getCurrentIndex() + 1);
    }

    if (input.keysPressedWithRepeat & KEY_UP) {
        setCurrentIndex(getCurrentIndex() - 1);
    }

    if (input.keysPressed & hotkey) {
        activate();
    } else if (input.keysPressed & KEY_TOUCH) {
        int index = findTappedItem(input.touchX, input.touchY);

        if (index == getCurrentIndex()) {
            activate();
        } else if (index >= 0) {
            setCurrentIndex(index);
        }
    }
}

void UIList::updateState() {
    if (needRepaint) {
        draw.paintAll();
        needRepaint = false;
    }

    UITransient::updateState();
}

void UIList::animate() {
    /*
     * Make sure the current item is within the safe area.
     * If not, scroll so that it's fully inside.
     */

    const int scroll = draw.getScroll();
    const int safeTop = scroll + 30;
    const int safeBottom = scroll + SCREEN_HEIGHT - 30;

    int top, bottom, diff;
    getCurrentItemY(top, bottom);

    if (top < safeTop) {
        diff = top - safeTop;
    } else if (bottom > safeBottom) {
        diff = bottom - safeBottom;
    } else {
        diff = 0;
    }

    if (diff) {
        /*
         * Go faster when we're farther away, slower when we approach
         * it.  scrollBy() will take care of limiting the speed.
         */
        draw.scrollBy(diff >> 2);
    }

    UITransient::animate();
}

void UIList::activate() {
    /*
     * Select the current item, end the list UI.
     */
    hide();
}

void UIList::append(UIListItem *item) {
    sassert(item, "Append NULL list item");
    sassert(item->getHeight() > 0, "Append list item with no height");
    items.push_back(item);
    needRepaint = true;
}

void UIList::setCurrentIndex(int index) {
    UIListItem *prevItem = getCurrentItem();

    index = std::max(index, 0);
    index = std::min(index, (int)items.size() - 1);
    draw.currentItem = index;

    draw.paint(prevItem);
    draw.paint(getCurrentItem());
}

void UIList::getCurrentItemY(int &top, int &bottom) {
    /*
     * Get the Y coordinates of the current item.
     */

    std::vector<UIListItem*>::iterator i;
    int index = 0;
    int y = draw.getOffsetY();

    for (i = items.begin(); i != items.end(); i++) {
        UIListItem *item = *i;
        int height = item->getHeight();

        if (index == getCurrentIndex()) {
            top = y;
            bottom = y + height;
            return;
        }

        y += height;
        index++;
    }

    sassert(false, "Current item not found");
}

int UIList::findTappedItem(int x, int y) {
    /*
     * Find the index of the item under (x,y) in screen coordinates,
     * if any. If no items match, returns -1.
     */

    std::vector<UIListItem*>::iterator i;
    int index = 0;

    x -= draw.getOffsetX();
    y -= draw.getOffsetY();
    y += draw.getScroll();

    for (i = items.begin(); i != items.end(); i++) {
        UIListItem *item = *i;
        int height = item->getHeight();

        if (item->hitTest(x, y)) {
            return index;
        }

        y -= height;
        index++;
    }

    return -1;
}


//********************************************************** UIListDraw


void UIListDraw::paint() {
    /*
     * Repaint all items.
     */

    std::vector<UIListItem*>::iterator i;
    int index = 0;
    int x = offsetX;
    int y = offsetY;

    clear();

    for (i = items->begin(); i != items->end(); i++) {
        UIListItem *item = *i;
        int height = item->getHeight();

        if (y + height >= clip.y1 && y <= clip.y2) {
            item->paint(this, x, y, currentItem == index);
        }

        y += height;
        index++;
    }

    blit();
}

void UIListDraw::paint(UIListItem *itemToPaint) {
    /*
     * Repaint a single item.
     */

    std::vector<UIListItem*>::iterator i;
    int index = 0;
    int x = offsetX;
    int y = offsetY;

    for (i = items->begin(); i != items->end(); i++) {
        UIListItem *item = *i;
        int height = item->getHeight();

        if (itemToPaint == item) {
            /*
             * Clip to this item and to the screen
             */
            clip.y1 = std::max(getScroll(), y);
            clip.y2 = std::min(getScroll() + SCREEN_HEIGHT, y + height);

            if (clip.y1 < clip.y2) {
                /*
                 * Note that we don't clear behind the item: We expect
                 * the item to fully redraw its own background, and we
                 * don't want to wipe out the overlap from adjacent
                 * items that we aren't painting.
                 */
                item->paint(this, x, y, currentItem == index);
                blit();
            }
            break;
        }

        y += height;
        index++;
    }
}


//********************************************************** UIFileListItem


UIFileListItem::UIFileListItem() {
    for (int slot = 0; slot < TEXT_NUM_SLOTS; slot++) {
        text[slot][0] = '\0';
    }
}

void UIFileListItem::setText(TextSlot slot, const char *format, ...) {
    va_list v;

    va_start(v, format);
    vsnprintf(text[slot], sizeof text[slot], format, v);
    va_end(v);
}

void UIFileListItem::paint(UIListDraw *draw, int x, int y, bool hilight) {
    const int width = SCREEN_WIDTH - x - 2;
    int boxPalette;
    TextColors colors;

    if (hilight) {
        boxPalette = 4;
        draw->colors = TextColors();
    } else {
        boxPalette = 0;
        draw->colors = TextColors(249, 0, 248);
    }

    /*
     * The +1 causes the bottom black border to overlap with the top
     * black border on the next item, so we don't get a double-thick
     * border.
     */
    draw->drawBox(Rect(x, y, width, height+1), boxPalette);

    int x1 = x + 1;
    int x2 = x + width - 1;
    int y1 = y + 1;

    draw->moveTo(x1, y1);
    draw->setAlignment(draw->LEFT);
    draw->draw(text[TEXT_TOP_LEFT]);
    draw->moveTo(x1, y1 + draw->font.getLineSpacing());
    draw->draw(text[TEXT_BOTTOM_LEFT]);

    draw->moveTo(x2, y1);
    draw->setAlignment(draw->RIGHT);
    draw->draw(text[TEXT_TOP_RIGHT]);
    draw->moveTo(x2, y1 + draw->font.getLineSpacing());
    draw->draw(text[TEXT_BOTTOM_RIGHT]);

    draw->moveTo((x1 + x2) / 2, y1 + draw->font.getLineSpacing() / 2);
    draw->setAlignment(draw->CENTER);
    draw->draw(text[TEXT_CENTER]);
}

int UIFileListItem::getHeight() {
    return height;
}

bool UIFileListItem::hitTest(int x, int y) {
    return x > 0 && y > 0 && y < height;
}


//********************************************************** UIFileListItem


UIListWithRobot::UIListWithRobot(RORobotId robotId)
    : UIList(37),
      sprAlloc(&oamSub),
      sprScraper(),
      renderer(&sprScraper),
      roData(&renderer),
      robot(&sprAlloc, &sprScraper, &roData, robotId)
{
    /*
     * Load a world from the filesystem. We need this for the sprite
     * data, at minimum. It has the handy side-effect of putting all
     * the other world data structures in a consistent state.
     *
     * We load sewer.wor here because we need a complete world file
     * and it needs to be from the main game (so we have 4 robots).
     * This is the only option that meets both criteria.
     */
    DOSFilesystem dos;
    int fd = dos.open("sewer.wor");
    dos.read(fd, (void*)roData.world, sizeof *roData.world);

    robot.sprite.moveTo(20,60);
    robot.sprite.show();
    robot.setupRenderer(&roData);

    /*
     * Initialize the rendere by running a couple of frames.
     */
    for (int i = 0; i < 2; i++) {
        while (renderer.run() != SBTHALT_LOAD_ROOM_ID);
        renderer.reg.al = RO_ROOM_RENDERER;
    }
}

void UIListWithRobot::animate() {
    animateRobot();
    UIList::animate();
}

void UIListWithRobot::animateRobot() {
    /*
     * Figure out how much we need to move.
     */
    int robotX = robot.sprite.getX();
    int currentY = robot.sprite.getY();
    int top, bottom;
    getCurrentItemY(top, bottom);
    int destY = (top + bottom) / 2 - draw.getScroll();
    int diff = destY - currentY;

    if (diff > 0) {
        diff = 1;
    } else if (diff < 0) {
        diff = -1;
    }

    /*
     * Move the sprite (at full frame rate)
     */
    robot.sprite.moveTo(robotX, currentY + diff);

    /*
     * Animate the robot and thrusters at a reduced rate.
     */

    if ((frameCount & 3) == 0) {
        RORobot *robotState = &roData.robots.state[robot.getRobotId()];   

        robotState->thrusterEnable(RO_SIDE_TOP, diff > 0);
        robotState->thrusterEnable(RO_SIDE_BOTTOM, diff < 0);
        robotState->animateThrusters();

        while (renderer.run() != SBTHALT_LOAD_ROOM_ID);
        renderer.reg.al = RO_ROOM_RENDERER;
    }
}

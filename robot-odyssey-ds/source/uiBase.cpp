/* -*- Mode: C++; c-basic-offset: 4 -*-
 *
 * Framework for user interface objects, collections of objects, and
 * input events. A UIObject gets a per-frame callback, and it can
 * optionally respond to keyboard or stylus input.
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
#include "uiBase.h"


//********************************************************** UIObject


uint32_t UIObject::frameCount;

void UIObject::animate() {
    /* No-op. */
}

void UIObject::updateState() {
    /* No-op. */
}

void UIObject::handleInput(const UIInputState &input) {
    /* No-op. */
}


//********************************************************** UIObjectList


UIObjectList *UIObjectList::currentList;

void UIObjectList::animate() {
    std::list<UIObject*>::iterator i;
    for (i = objects.begin(); i != objects.end(); i++) {
        (*i)->animate();
    }
}

void UIObjectList::updateState() {
    std::list<UIObject*>::iterator i;
    for (i = objects.begin(); i != objects.end(); i++) {
        (*i)->updateState();
    }
}

void UIObjectList::handleInput(const UIInputState &input) {
    std::list<UIObject*>::iterator i;
    for (i = objects.begin(); i != objects.end(); i++) {
        (*i)->handleInput(input);
    }
}

void UIObjectList::makeCurrent() {
    currentList = this;
    irqSet(IRQ_VBLANK, vblankISR);
    irqEnable(IRQ_VBLANK);
}

void UIObjectList::vblankISR() {
    /*
     * During vertical blank, scan input and update the UI.
     */

    // Local copy of current list, in case a callback changes it.
    UIObjectList *l = currentList;

    UIObject::frameCount++;
    l->animate();

    UIInputState input;
    scanInput(input);
    l->handleInput(input);

    l->updateState();

    // Flush updated sprites to the hardware.
    oamUpdate(&oamMain);
    oamUpdate(&oamSub);
}

void UIObjectList::scanInput(UIInputState &input) {
    touchPosition touch;

    scanKeys();
    input.keysHeld = keysHeld();
    input.keysPressed = keysDown();
    input.keysReleased = keysUp();

    touchRead(&touch);

    if (touch.rawx == 0 || touch.rawy == 0) {
        input.keysHeld &= ~KEY_TOUCH;
    }

    input.touchX = touch.px;
    input.touchY = touch.py;
}


//********************************************************** UISpriteButton


UISpriteButton::UISpriteButton(MSpriteAllocator *sprAlloc,
                               SpriteImages *images,
                               int x, int y,
                               MSpriteRange range,
                               bool autoDoubleSize) : sprite(sprAlloc) {
    this->images = images;
    this->autoDoubleSize = autoDoubleSize;

    /*
     * Allocate the main OBJ, using the format and first iamge from 'images'.
     * This OBJ needs to be double-sized, so we can magnify it without clipping.
     */
    MSpriteOBJ *obj = sprite.newOBJ(range, 0, 0, images->getImage(0),
                                    images->size, images->format);

    obj->entry->palette = OBJ_PALETTE;
    obj->show();

    sprite.moveTo(x, y);
}

void UISpriteButton::setImageIndex(int id) {
    /*
     * Set which of the images in 'images' to display on the primary OBJ.
     */
    sprite.obj[0].setGfx(images->getImage(id));
}

void UISpriteButton::handleInput(const UIInputState &input) {
    if (input.keysPressed & hotkey) {
        activate();
    } else if ((input.keysPressed & KEY_TOUCH) &&
               sprite.hitTest(input.touchX, input.touchY)) {
        activate();
    }
}

void UISpriteButton::activate() {
    /*
     * Grow the button, to show that it's activated.
     *
     * While the button is large, by default we turn on double-size
     * mode so it doesn't clip. We don't want to leave it on all the
     * time, because the NDS hardware will quickly hit its
     * per-scanline sprite limit.
     */

    if (autoDoubleSize) {
        sprite.setDoubleSize(true);
    }
    sprite.setScale(activatedScale, activatedScale);
}

void UISpriteButton::animate() {
    /*
     * If the button is large, shrink down to its normal size.
     */

    int scale;

    sprite.getScale(scale, scale);

    if (scale < normalScale) {
        scale += scaleRate;
    }
    if (scale >= normalScale) {
        if (autoDoubleSize) {
            sprite.setDoubleSize(false);
        }
        scale = normalScale;
    }

    sprite.setScale(scale, scale);
}


//********************************************************** UIAnimationSequence


UIAnimationSequence::UIAnimationSequence(const Item *items) {
    this->items = items;
    start();
}

void UIAnimationSequence::start() {
    currentItem = items;
    remainingFrames = currentItem->numFrames;
}

void UIAnimationSequence::next() {
    remainingFrames--;
    if (remainingFrames <= 0) {
        currentItem++;
        if (currentItem->numFrames == 0) {
            currentItem = items;
        }
        remainingFrames = currentItem->numFrames;
    }
}

int UIAnimationSequence::getIndex() {
    return currentItem->index;
}


/* -*- Mode: C; c-basic-offset: 4 -*-
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

UIObjectList *UIObjectList::currentList;
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
    // Local copy of current list, in case a callback changes it.
    UIObjectList *l = currentList;

    UIObject::frameCount++;
    l->animate();

    UIInputState input;
    scanInput(input);
    l->handleInput(input);

    l->updateState();
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
    };

    input.touchX = touch.px;
    input.touchY = touch.py;
}

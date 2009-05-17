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


UIObjectList* volatile UIObjectList::currentList;
bool UIObjectList::debugLoadMeter;

UIObjectList::~UIObjectList() {
    sassert(currentList != this, "Can't destroy the current UIObjectList");
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

void UIObjectList::activate() {
    sassert(currentList == NULL, "Another UIObjectList is already active");
    currentList = this;
    irqSet(IRQ_VBLANK, vblankISR);
    irqEnable(IRQ_VBLANK);
}

void UIObjectList::deactivate() {
    sassert(currentList == this, "Deactivating a non-active UIObjectList");
    irqDisable(IRQ_VBLANK);
    currentList = NULL;
}

void UIObjectList::vblankISR() {
    /*
     * Debugging: Turn the sub background red, to indicate CPU time
     * being used.  If the ISR runs longer than the vertical blanking
     * period, you'll see red on the screen.
     */
    if (debugLoadMeter) {
        BG_PALETTE_SUB[0] = RGB5(31,0,0);
    }

    /*
     * Flush updated sprites and layers to the hardware.
     * This must occur before we actually exit vblank!
     */

    bgUpdate();
    oamUpdate(&oamMain);
    oamUpdate(&oamSub);

    /*
     * During vertical blank, scan input and update the UI.
     */

    UIObjectList *current = currentList;

    if (!current) {
        return;
    }

    UIObject::frameCount++;

    current->animate();

    UIInputState input;
    scanInput(input);
    current->handleInput(input);

    current->updateState();

    /*
     * Debug meter: End of ISR, turn the background blue again. Blue
     * indicates no CPU usage, but still lets you know the load meter
     * is on.
     *
     * The red portion of the screen will show how much time we're
     * using in the ISR outside of vblank. If the red flickers, we're
     * using more than a whole frame!
     */
    BG_PALETTE_SUB[0] = debugLoadMeter ? RGB5(0,0,15) : RGB5(0,0,0);
}

void UIObjectList::scanInput(UIInputState &input) {
    touchPosition touch;

    scanKeys();
    input.keysHeld = keysHeld();
    input.keysPressed = keysDown();
    input.keysReleased = keysUp();

    /*
     * Debugging: Trigger the system load meter with L+R+START
     */
    if (input.keysHeld == (KEY_L | KEY_R | KEY_START) && input.keysPressed) {
        debugLoadMeter = !debugLoadMeter;
    }

    touchRead(&touch);

    if (touch.rawx == 0 || touch.rawy == 0) {
        input.keysHeld &= ~KEY_TOUCH;
    }

    input.touchX = touch.px;
    input.touchY = touch.py;
}


//********************************************************** UITransient


UITransient::UITransient(Fixed16 speed, bool shown) {
    if (shown) {
        state = SHOWN;
        visibility = FP_ONE;
    } else {
        state = HIDDEN;
        visibility = 0;
    }
    this->speed = speed;
    updateState();
}

void UITransient::show() {
    if (state != SHOWN) {
        state = SHOWING;
    }
}

void UITransient::hide() {
    if (state != HIDDEN) {
        state = HIDING;
    }
}

bool UITransient::isShown() {
    return state == SHOWN;
}

bool UITransient::isHidden() {
    return state == HIDDEN;
}

void UITransient::run() {
    show();
    activate();
    while (!isHidden()) {
        swiWaitForVBlank();
    }
    deactivate();
}

void UITransient::animate() {
    switch (state) {

    case HIDING:
        if (visibility > speed) {
            visibility -= speed;
        } else {
            visibility = 0;
            state = HIDDEN;
        }
        break;

    case SHOWING:
        visibility += speed;
        if (visibility >= FP_ONE) {
            visibility = FP_ONE;
            state = SHOWN;
        }
        break;

    default:
        break;
    }

    UIObjectList::animate();
}


UITransient::Fixed16 UITransient::easeVisibility() {
    /*
     * Apply a nonlinear transform to the current visibility so that
     * it eases its way in and out. This implementation uses a sine
     * curve.
     */
    uint16_t angle = visibility >> 3;
    return sinLerp(angle) << 4;
}


//********************************************************** UIFade


UIFade::UIFade(Screen screen, bool startOutFaded,
               Fixed16 speed, Color color)
    : UITransient(speed, startOutFaded)
{
    uint32_t ctrlValue = (BLEND_SRC_BG0 |
                          BLEND_SRC_BG1 |
                          BLEND_SRC_BG2 |
                          BLEND_SRC_BG3 |
                          BLEND_SRC_SPRITE |
                          BLEND_SRC_BACKDROP);

    switch (color) {

    case BLACK:
        ctrlValue |= BLEND_FADE_BLACK;
        break;

    case WHITE:
        ctrlValue |= BLEND_FADE_WHITE;
        break;

    }

    switch (screen) {

    case MAIN:
        blendReg = &REG_BLDY;
        REG_BLDCNT = ctrlValue;
        break;

    case SUB:
        blendReg = &REG_BLDY_SUB;
        REG_BLDCNT_SUB = ctrlValue;
        break;

    }
}

void UIFade::updateState() {
    *blendReg = 31 * visibility >> FP_SHIFT;
}



//********************************************************** UISpriteButton


UISpriteButton::UISpriteButton(MSpriteAllocator *sprAlloc,
                               SpriteImages *images,
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


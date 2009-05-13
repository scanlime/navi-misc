/* -*- Mode: C++; c-basic-offset: 4 -*-
 *
 * A simple full-screen message box UI.
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

#include "uiMessageBox.h"
#include "gfx_rarrow.h"
#include "gfx_button_ok.h"


//********************************************************** UIMessageBox


UIMessageBox::UIMessageBox(const char *format, ...)
    : UITransient(1200),
      sprAlloc(&oamSub),
      text(),
      button(&sprAlloc, this)
{
    va_list v;

    text.drawFrame(Rect(outerMargin, outerMargin, frameWidth, frameHeight));
    text.setBackgroundOpaque(true);
    text.moveTo(outerMargin + innerMargin,
                outerMargin + innerMargin);
    text.setWrapWidth(frameWidth - innerMargin*2);

    va_start(v, format);
    text.vprintf(format, v);
    va_end(v);

    text.blit();
    objects.push_back(&button);
}

void UIMessageBox::updateState() {
    text.setScroll(0, (FP_ONE - easeVisibility()) * SCREEN_HEIGHT >> FP_SHIFT);
    bgUpdate();

    button.sprite.moveTo(buttonX - ((easeVisibility() * button.width) >> FP_SHIFT),
                         buttonY);

    UITransient::updateState();
}


//********************************************************** UIOKButton


UIOKButton::UIOKButton(MSpriteAllocator *sprAlloc, UITransient *transient)
    : UISpriteButton(sprAlloc, allocButtonImage(sprAlloc)),
      animSequence(animation)
{
    this->transient = transient;
    hotkey = KEY_A;

    arrowImage = new SpriteImages(sprAlloc->oam, gfx_rarrowTiles, LZ77Vram,
                                  SpriteSize_16x16, SpriteColorFormat_16Color);

    MSpriteOBJ *arrow = sprite.newOBJ(MSPRR_FRONT, arrowX, arrowY,
                                      arrowImage->getImage(0),
                                      arrowImage->size,
                                      arrowImage->format);
    arrow->entry->palette = OBJ_PALETTE;
    arrow->show();
}

UIOKButton::~UIOKButton() {
    delete buttonImage;
    delete arrowImage;
}

void UIOKButton::activate() {
    UISpriteButton::activate();
    transient->hide();
}

void UIOKButton::animate() {
    UISpriteButton::animate();
    sprite.obj[1].xOffset = arrowX + animSequence.getIndex();
    animSequence.next();
    sprite.update();
}

SpriteImages *UIOKButton::allocButtonImage(MSpriteAllocator *sprAlloc) {
    return buttonImage = new SpriteImages(sprAlloc->oam, gfx_button_okTiles, LZ77Vram,
                                          SpriteSize_64x32, SpriteColorFormat_16Color);
}

/*
 * Horizontal offsets and frame numbers for the button animation sequence.
 */
UIAnimationSequence::Item UIOKButton::animation[] = {
    {0, 32},
    {1, 4},
    {2, 2},
    {3, 1},
    {2, 2},
    {1, 4},
    {0},
};

/* -*- Mode: C; c-basic-offset: 4 -*-
 *
 * UI objects for the sub screen: For interacting during gameplay.
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
#include "uiSubScreen.h"
#include "gfx_button_remote.h"
#include "gfx_button_toolbox.h"
#include "gfx_button_solder.h"


UIToggleButton::UIToggleButton(MSpriteAllocator *sprAlloc, SpriteImages *images,
                               UIAnimationSequence::Item *animation, int x, int y)
   : UISpriteButton(sprAlloc, images, x, y),
     animSequence(animation) {

   state = false;

   marquee = sprite.newOBJ(MSPRR_FRONT, 0, 0, NULL,
                           SpriteSize_32x32, SpriteColorFormat_16Color);
   marquee->entry->palette = OBJ_PALETTE;
   marquee->enableDoubleSize = true;

   marqueeEffect = EffectMarquee32::getSingleton(sprAlloc->oam);

   sprite.moveTo(x, y);
   sprite.show();
}

void UIToggleButton::animate() {
   UISpriteButton::animate();

   if (state) {
      setImageIndex(animSequence.getIndex());
      animSequence.next();
      marquee->show();
      marquee->setGfx(marqueeEffect->getFrameGfx(frameCount >> 1));
   } else {
      animSequence.start();
      setImageIndex(0);
      marquee->hide();
   }
}

UIRemoteControlButton::UIRemoteControlButton(MSpriteAllocator *sprAlloc,
                                             ROData *roData,
                                             HwCommon *hw, int x, int y)
   : UIToggleButton(sprAlloc, allocImages(sprAlloc), animation, x, y) {
   this->hw = hw;
   this->roData = roData;
   hotkey = KEY_R;
}

UIRemoteControlButton::~UIRemoteControlButton() {
   delete images;
}

void UIRemoteControlButton::activate() {
   UIToggleButton::activate();
   hw->pressKey('R');
}

void UIRemoteControlButton::updateState() {
   UIToggleButton::updateState();
   state = roData->circuit->remoteControlFlag;
}

SpriteImages *UIRemoteControlButton::allocImages(MSpriteAllocator *sprAlloc) {
   return images = new SpriteImages(sprAlloc->oam, gfx_button_remoteTiles, LZ77Vram,
                                    SpriteSize_32x32, SpriteColorFormat_16Color, 3);
}

UIAnimationSequence::Item UIRemoteControlButton::animation[] = {
   {0, 32},  // Expanding radio waves...
   {1, 32},
   {2, 64},  // Linger on the fully expanded waves for two counts.
   {0},
};

UISolderButton::UISolderButton(MSpriteAllocator *sprAlloc,
                               ROData *roData,
                               HwCommon *hw, int x, int y)
   : UIToggleButton(sprAlloc, allocImages(sprAlloc), animation, x, y) {
   this->hw = hw;
   this->roData = roData;
   hotkey = KEY_A;
}

UISolderButton::~UISolderButton() {
   delete images;
}

void UISolderButton::activate() {
   UIToggleButton::activate();
   if (state) {
      hw->pressKey('C');  // Back to normal cursor
   } else {
      hw->pressKey('S');  // To soldering iron
   }
}

void UISolderButton::updateState() {
   UIToggleButton::updateState();
   state = roData->world->objects.spriteId[RO_OBJ_PLAYER] == RO_SPR_SOLDER_IRON;
}

SpriteImages *UISolderButton::allocImages(MSpriteAllocator *sprAlloc) {
   return images = new SpriteImages(sprAlloc->oam, gfx_button_solderTiles, LZ77Vram,
                                    SpriteSize_32x32, SpriteColorFormat_16Color, 2);
}

UIAnimationSequence::Item UISolderButton::animation[] = {
   {0, 32},
   {1, 16},  // Briefly flash the hot tip.
   {0},
};

UIToolboxButton::UIToolboxButton(MSpriteAllocator *sprAlloc,
                                 HwCommon *hw, int x, int y)
   : UISpriteButton(sprAlloc, allocImages(sprAlloc), x, y) {
   this->hw = hw;
   hotkey = KEY_L;
}

UIToolboxButton::~UIToolboxButton() {
   delete images;
}

void UIToolboxButton::activate() {
   UISpriteButton::activate();
   hw->pressKey('T');
}

SpriteImages *UIToolboxButton::allocImages(MSpriteAllocator *sprAlloc) {
   return images = new SpriteImages(sprAlloc->oam, gfx_button_toolboxTiles, LZ77Vram,
                                    SpriteSize_32x32, SpriteColorFormat_16Color);
}

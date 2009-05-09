/* -*- Mode: C++; c-basic-offset: 4 -*-
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
#include "videoConvert.h"
#include "gfx_button_remote.h"
#include "gfx_button_toolbox.h"
#include "gfx_button_solder.h"
#include "gfx_battery.h"
#include "gfx_background.h"


//********************************************************** UIToggleButton


UIToggleButton::UIToggleButton(MSpriteAllocator *sprAlloc, SpriteImages *images,
                               UIAnimationSequence::Item *animation,
                               EffectMarquee32 *marqueeEffect, int x, int y)
    : UISpriteButton(sprAlloc, images, x, y),
      animSequence(animation) {

    state = false;

    marquee = sprite.newOBJ(MSPRR_FRONT, 0, 0, NULL,
                            SpriteSize_32x32, SpriteColorFormat_16Color);
    marquee->entry->palette = OBJ_PALETTE;
    marquee->enableDoubleSize = true;

    this->marqueeEffect = marqueeEffect;
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


//********************************************************** UIRemoteControlButton


UIRemoteControlButton::UIRemoteControlButton(MSpriteAllocator *sprAlloc,
                                             EffectMarquee32 *marqueeEffect,
                                             ROData *roData,
                                             HwCommon *hw, int x, int y)
    : UIToggleButton(sprAlloc, allocImages(sprAlloc), animation, marqueeEffect, x, y) {
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


//********************************************************** UISolderButton


UISolderButton::UISolderButton(MSpriteAllocator *sprAlloc,
                               EffectMarquee32 *marqueeEffect,
                               ROData *roData,
                               HwCommon *hw, int x, int y)
  : UIToggleButton(sprAlloc, allocImages(sprAlloc), animation, marqueeEffect, x, y) {
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


//********************************************************** UIToolboxButton


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


//********************************************************** UIBatteryIcon


UIBatteryIcon::UIBatteryIcon(MSpriteAllocator *sprAlloc, ROData *roData,
                             RORobotId robotId, int x, int y)
  : UISpriteButton(sprAlloc, allocImages(sprAlloc), x, y) {
    this->roData = roData;
    this->robotId = robotId;
}

UIBatteryIcon::~UIBatteryIcon() {
    delete images;
}

void UIBatteryIcon::activate() {
    UISpriteButton::activate();
    /*
     * XXX: Display detailed information about the battery level and discharge rate.
     */
}

void UIBatteryIcon::updateState() {
    /* The game's battery level is 0 to 15. */
    int batteryLevel = roData->robots.state[robotId].batteryLevel;

    /*
     * Use a lookup table to pick one of the 9 sprites.
     * This roughly divides the power level in two, but
     * we want to make sure the battery only looks full
     * or empty when it's actually full or empty.
     */
    static const int spriteTable[] = {
        0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8,
    };

    setImageIndex(spriteTable[batteryLevel & 0xF]);
    sprite.show();
}

SpriteImages *UIBatteryIcon::allocImages(MSpriteAllocator *sprAlloc) {
    return images = new SpriteImages(sprAlloc->oam, gfx_batteryTiles, LZ77Vram,
                                     SpriteSize_32x16, SpriteColorFormat_16Color, 9);
}


//********************************************************** UIRobotIcon


UIRobotIcon::UIRobotIcon(MSpriteAllocator *sprAlloc, HwSpriteScraper *sprScraper,
                         ROData *roData, RORobotId robotId, int x, int y)
    : UISpriteButton(sprAlloc, allocImages(sprAlloc, sprScraper),
                     x, y, MSPRR_FRONT) {
    this->roData = roData;
    this->robotId = robotId;

    // Center the robot sprite, and scale it properly.
    sprite.setIntrinsicScale(scraperRect->scaleX, scraperRect->scaleY);
    sprite.obj[0].center();

    /*
     * Abuse the sprite hardware to create a 1-pixel black border
     * around the robot sprite. This involves creating additional OBJs
     * behind the main robot sprite. These OBJs share the same image
     * data, but they use an all-black palette.
     */

    static const int xOffset[] = { 1, -1,  0,  0 };
    static const int yOffset[] = { 0,  0,  1, -1 };

    for (unsigned int i = 0; i < sizeof xOffset / sizeof xOffset[0]; i++) {
        MSpriteOBJ *obj = sprite.newOBJ(MSPRR_FRONT_BORDER, 0, 0,
                                        scraperRect->buffer,
                                        scraperRect->size,
                                        scraperRect->format);
        obj->entry->palette = OBJ_BORDER_PALETTE;
        obj->enableDoubleSize = true;
        obj->center();
        obj->moveBy(xOffset[i], yOffset[i]);
    }

    sprite.update();
    sprite.show();
}

UIRobotIcon::~UIRobotIcon() {
    delete images;
    sprScraper->freeRect(scraperRect);
}

void UIRobotIcon::activate() {
    UISpriteButton::activate();
    /*
     * XXX: Display the robot's interior.
     */
}

void UIRobotIcon::setupRenderer(ROData *rendererData) {
    /*
     * Move the robot to the center of its SpriteScraper slot.
     */

    ROObjectId objId = roData->robots.state[robotId].getObjectId();
    const int xOffset = -6;
    const int yOffset = -8;

    rendererData->world->setRobotRoom(objId, RO_ROOM_RENDERER);
    rendererData->world->setRobotXY(objId,
                                    scraperRect->centerX() + xOffset,
                                    scraperRect->centerY() + yOffset);
}

SpriteImages *UIRobotIcon::allocImages(MSpriteAllocator *sprAlloc,
                                       HwSpriteScraper *sprScraper) {
    this->sprScraper = sprScraper;
    scraperRect = sprScraper->allocRect(sprAlloc->oam);
    images = new SpriteImages(sprAlloc->oam, scraperRect->size,
                              scraperRect->format, scraperRect->buffer);
    return images;
}


//********************************************************** UISubHardware


UISubHardware::UISubHardware(const void *bgBitmap, const void *bgPalette) {
    videoSetModeSub(MODE_5_2D);
    vramSetBankC(VRAM_C_SUB_BG_0x06200000);
    vramSetBankD(VRAM_D_SUB_SPRITE);

    static const uint32 paletteSize = 16 * sizeof(uint16_t);

    /* Sprite palette 0: VideoConvert */
    oamInit(&oamSub, SpriteMapping_1D_64, false);
    dmaCopy(VideoConvert::palette, SPRITE_PALETTE_SUB, paletteSize);

    /* Sprite palette 1: All black, for robot borders */
    dmaFillHalfWords(0x0000,
                     SPRITE_PALETTE_SUB + UIRobotIcon::OBJ_BORDER_PALETTE * 16,
                     paletteSize);

    /* Sprite palette 2: UI sprites */
    decompress(gfx_button_remotePal,
               SPRITE_PALETTE_SUB + UISpriteButton::OBJ_PALETTE * 16,
               LZ77Vram);

    /* Backdrop image (BG3) */
    int subBg = bgInitSub(3, BgType_Bmp8, BgSize_B8_256x256, 0, 0);
    decompress(bgBitmap, bgGetGfxPtr(subBg), LZ77Vram);
    decompress(bgPalette, BG_PALETTE_SUB, LZ77Vram);
}


//********************************************************** UISubScreen


UISubScreen::UISubScreen(ROData *gameData, HwCommon *hw)
  : UIObjectList(),
    subHw(gfx_backgroundBitmap, gfx_backgroundPal),
    text(),

    spriteScraper(),
    renderer(&spriteScraper),
    rendererData(&renderer),

    sprAlloc(&oamSub),
    marqueeEffect(&oamSub),

    btnRemote(&sprAlloc, &marqueeEffect, gameData, hw,
              SCREEN_WIDTH - btnRemote.width,
              SCREEN_HEIGHT - btnRemote.height),
    btnSolder(&sprAlloc, &marqueeEffect, gameData, hw,
              0, SCREEN_HEIGHT - btnSolder.height),
    btnToolbox(&sprAlloc, hw,
               SCREEN_WIDTH - btnSolder.height, 0),

    xRobot(&sprAlloc, &spriteScraper, gameData, RO_ROBOT_CHECKERS, 128 - 30, 64),
    yRobot(&sprAlloc, &spriteScraper, gameData, RO_ROBOT_SCANNER, 128 + 30, 64)
{
    this->gameData = gameData;

    objects.push_back(&btnRemote);
    objects.push_back(&btnSolder);
    objects.push_back(&btnToolbox);
    objects.push_back(&xRobot);
    objects.push_back(&yRobot);

    makeCurrent();
}

void UISubScreen::renderFrame(void) {
    int haltCode;

    // Copy all game state to the renderer
    rendererData.copyFrom(gameData);

    xRobot.setupRenderer(&rendererData);
    yRobot.setupRenderer(&rendererData);

    /*
     * Run the renderer until it produces another frame.
     * When we hit the LOAD_ROOM_ID hook, patch in our
     * fake room ID.
     */
    do {
        haltCode = renderer.run();
        if (haltCode == SBTHALT_LOAD_ROOM_ID) {
            renderer.reg.al = RO_ROOM_RENDERER;
        }
    } while (haltCode != SBTHALT_FRAME_DRAWN);
}

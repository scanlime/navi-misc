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
#include <string.h>
#include "uiSubScreen.h"
#include "videoConvert.h"
#include "gfx_button_remote.h"
#include "gfx_button_toolbox.h"
#include "gfx_button_solder.h"
#include "gfx_battery.h"


//********************************************************** UIToggleButton


UIToggleButton::UIToggleButton(MSpriteAllocator *sprAlloc, SpriteImages *images,
                               UIAnimationSequence::Item *animation,
                               EffectMarquee32 *marqueeEffect)
    : UISpriteButton(sprAlloc, images),
      animSequence(animation) {

    state = false;

    marquee = sprite.newOBJ(MSPRR_FRONT, 0, 0, NULL,
                            SpriteSize_32x32, SpriteColorFormat_16Color);
    marquee->entry->palette = OBJ_PALETTE;

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
                                             HwCommon *hw)
    : UIToggleButton(sprAlloc, allocImages(sprAlloc), animation, marqueeEffect) {
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
    state = roData->world->objects.spriteId[RO_OBJ_ANTENNA] == RO_SPR_REMOTE_CONTROL;
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
                               HwCommon *hw)
  : UIToggleButton(sprAlloc, allocImages(sprAlloc), animation, marqueeEffect) {
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
                                 HwCommon *hw)
  : UISpriteButton(sprAlloc, allocImages(sprAlloc)) {
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
                             RORobotId robotId)
    : UISpriteButton(sprAlloc, allocImages(sprAlloc), MSPRR_UI, false) {
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
                         ROData *roData, RORobotId robotId)

    // Note that we disable autoDoubleSize. The robot already has plenty
    // of padding around the edges so it won't clip, and double-size
    // here will quickly exhaust the NDS's limit on sprites per scanline.
    : UISpriteButton(sprAlloc, allocImages(sprAlloc, sprScraper),
                     MSPRR_FRONT, false)
{    
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
        obj->center();
        obj->moveBy(xOffset[i], yOffset[i]);
    }
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

    return images = new SpriteImages(sprAlloc->oam, scraperRect->size,
                                     scraperRect->format, scraperRect->buffer);
}


//********************************************************** UIRobotStatus


UIRobotStatus::UIRobotStatus(MSpriteAllocator *sprAlloc,
                             HwSpriteScraper *sprScraper,
                             ROData *roData,
                             RORobotId robotId)
    : icon(sprAlloc, sprScraper, roData, robotId),
      battery(sprAlloc, roData, robotId)
{
    objects.push_back(&icon);
    objects.push_back(&battery);
}

void UIRobotStatus::setupRenderer(ROData *rendererData) {
    icon.setupRenderer(rendererData);
}

void UIRobotStatus::moveTo(int x, int y) {
    icon.sprite.moveTo(x, y);
    icon.sprite.show();

    battery.sprite.moveTo(x - battery.width / 2,
                          y + 16);
    battery.sprite.show();
}



//********************************************************** UISubScreen


UISubScreen::UISubScreen(ROData *gameData, HwCommon *hw)
  : UIObjectList(),
    text(),

    spriteScraper(),
    renderer(&spriteScraper),
    rendererData(&renderer),

    sprAlloc(&oamSub),
    marqueeEffect(&oamSub),

    btnRemote(&sprAlloc, &marqueeEffect, gameData, hw),
    btnSolder(&sprAlloc, &marqueeEffect, gameData, hw),
    btnToolbox(&sprAlloc, hw),

    xRobot(&sprAlloc, &spriteScraper, gameData, RO_ROBOT_CHECKERS),
    yRobot(&sprAlloc, &spriteScraper, gameData, RO_ROBOT_SCANNER)
{
    this->gameData = gameData;

    btnRemote.sprite.moveTo(SCREEN_WIDTH - btnRemote.width,
                     SCREEN_HEIGHT - btnRemote.height);
    btnRemote.sprite.show();
    objects.push_back(&btnRemote);

    btnSolder.sprite.moveTo(0, SCREEN_HEIGHT - btnSolder.height);
    btnSolder.sprite.show();
    objects.push_back(&btnSolder);

    btnToolbox.sprite.moveTo(SCREEN_WIDTH - btnToolbox.height, 0);
    btnToolbox.sprite.show();
    objects.push_back(&btnToolbox);

    xRobot.moveTo(128 - 30, 64);
    objects.push_back(&xRobot);

    yRobot.moveTo(128 + 30, 64);
    objects.push_back(&yRobot);
}

void UISubScreen::renderFrame(void) {
    // Copy all game state to the renderer
    rendererData.copyFrom(gameData);

    xRobot.setupRenderer(&rendererData);
    yRobot.setupRenderer(&rendererData);

    /*
     * Clear the room data for whatever room the player is in.  The
     * fake room ID below doesn't seem to take effect until the second
     * frame that's rendered, so we could get a single frame flash of
     * whatever room the player is in. Work around this by deleting
     * all of the walls in that room.
     */

    RORoomId playerRoom = (RORoomId) rendererData.world->objects.room[RO_OBJ_PLAYER];
    memset(rendererData.world->rooms.tiles[playerRoom], 0x00, sizeof(RORoomTiles));

    /*
     * Run the renderer until it reaches the top of the main loop,
     * where we need to patch in the fake room ID. Normally this will
     * produce one frame of video.
     */

    while (renderer.run() != SBTHALT_LOAD_ROOM_ID);
    renderer.reg.al = RO_ROOM_RENDERER;
}

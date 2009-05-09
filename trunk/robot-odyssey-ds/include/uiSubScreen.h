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

#ifndef _UISUBSCREEN_H_
#define _UISUBSCREEN_H_

#include "uiBase.h"
#include "uiEffects.h"
#include "mSprite.h"
#include "roData.h"
#include "hwCommon.h"
#include "hwSpriteScraper.h"
#include "roData.h"
#include "textRenderer.h"
#include "sbt86.h"

SBT_DECL_PROCESS(RendererEXE);


/*
 * A toggleable sprite button which displays a marquee and animates
 * when in its active state. This is an abstract base class. Subclasses
 * must provide a way to set and read the current button state.
 */
class UIToggleButton : public UISpriteButton
{
 public:
    UIToggleButton(MSpriteAllocator *sprAlloc, SpriteImages *images,
                   UIAnimationSequence::Item *animation,
                   EffectMarquee32 *marqueeEffect, int x, int y);

    virtual void animate();

    static const uint32_t width = 32;
    static const uint32_t height = 32;

 protected:
    bool state;

 private:
    UIAnimationSequence animSequence;
    MSpriteOBJ *marquee;
    EffectMarquee32 *marqueeEffect;
};


/*
 * A button to toggle the remote control on and off.
 */
class UIRemoteControlButton : public UIToggleButton
{
 public:
    UIRemoteControlButton(MSpriteAllocator *sprAlloc, EffectMarquee32 *marqueeEffect,
                          ROData *roData, HwCommon *hw, int x, int y);

    ~UIRemoteControlButton();

    virtual void activate();
    virtual void updateState();

 private:
    SpriteImages *allocImages(MSpriteAllocator *sprAlloc);

    static UIAnimationSequence::Item animation[];

    SpriteImages *images;
    HwCommon *hw;
    ROData *roData;
};


/*
 * A button to toggle the soldering iron on and off.
 */
class UISolderButton : public UIToggleButton
{
 public:
    UISolderButton(MSpriteAllocator *sprAlloc, EffectMarquee32 *marqueeEffect,
                   ROData *roData, HwCommon *hw, int x, int y);

    ~UISolderButton();

    virtual void activate();
    virtual void updateState();

 private:
    SpriteImages *allocImages(MSpriteAllocator *sprAlloc);

    static UIAnimationSequence::Item animation[];

    SpriteImages *images;
    HwCommon *hw;
    ROData *roData;
};


/*
 * A button to display the toolbox.
 */
class UIToolboxButton : public UISpriteButton
{
 public:
    UIToolboxButton(MSpriteAllocator *sprAlloc, HwCommon *hw, int x, int y);
    ~UIToolboxButton();

    virtual void activate();

    static const uint32_t width = 32;
    static const uint32_t height = 32;

 private:
    SpriteImages *allocImages(MSpriteAllocator *sprAlloc);

    SpriteImages *images;
    HwCommon *hw;
};


/*
 * Robot battery level icon
 */
class UIBatteryIcon : public UISpriteButton
{
 public:
    UIBatteryIcon(MSpriteAllocator *sprAlloc, ROData *roData,
                  RORobotId robotId, int x, int y);
    ~UIBatteryIcon();

    virtual void activate();
    virtual void updateState();

    static const uint32_t width = 32;
    static const uint32_t height = 16;

 private:
    SpriteImages *allocImages(MSpriteAllocator *sprAlloc);

    SpriteImages *images;
    ROData *roData;
    RORobotId robotId;
};


/*
 * Robot icon. This shows a live picture of one robot. Touching it
 * will view the robot's interior.
 */
class UIRobotIcon : public UISpriteButton
{
 public:
    UIRobotIcon(MSpriteAllocator *sprAlloc, HwSpriteScraper *sprScraper,
                ROData *roData, RORobotId robotId, int x, int y);
    ~UIRobotIcon();

    virtual void activate();

    void setupRenderer(ROData *rendererData);

    static const int OBJ_BORDER_PALETTE = 1;

 private:
    SpriteImages *allocImages(MSpriteAllocator *sprAlloc,
                              HwSpriteScraper *sprScraper);

    SpriteImages *images;
    ROData *roData;
    RORobotId robotId;
    HwSpriteScraper *sprScraper;
    SpriteScraperRect *scraperRect;
};


/*
 * Low-level hardware initialization for the sub-screen,
 * as well as loading of the backdrop image for layer 3.
 */
class UISubHardware
{
 public:
    UISubHardware(const void *bgBitmap, const void *bgPalette);

 private:
    int subBg;
};


/*
 * The entire sub-screen UI.
 */
class UISubScreen : public UIObjectList
{
public:
    UISubScreen(ROData *gameData, HwCommon *hw);

    /*
     * Render a frame based on the current game data state.
     * Should be called from the main program, not an ISR.
     */
    void renderFrame(void);

private:
    UISubHardware subHw;

public:
    // Text is public, to make debugging easier.
    TextRenderer text;

private:
    ROData *gameData;

    HwSpriteScraper spriteScraper;
    RendererEXE renderer;
    ROData rendererData;

    MSpriteAllocator sprAlloc;
    EffectMarquee32 marqueeEffect;

    UIRemoteControlButton btnRemote;
    UISolderButton btnSolder;
    UIToolboxButton btnToolbox;

    UIRobotIcon xRobot;
    UIRobotIcon yRobot;
};


#endif // _UISUBSCREEN_H_

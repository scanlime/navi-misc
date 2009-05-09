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

#ifndef _UISUBSCREEN_H_
#define _UISUBSCREEN_H_

#include "uiBase.h"
#include "uiEffects.h"
#include "mSprite.h"
#include "roData.h"
#include "hwCommon.h"
#include "roData.h"


/*
 * A toggleable sprite button which displays a marquee and animates
 * when in its active state. This is an abstract base class. Subclasses
 * must provide a way to set and read the current button state.
 */
class UIToggleButton : public UISpriteButton
{
 public:
    UIToggleButton(MSpriteAllocator *sprAlloc, SpriteImages *images,
                   UIAnimationSequence::Item *animation, int x, int y);

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
    UIRemoteControlButton(MSpriteAllocator *sprAlloc, ROData *roData,
                          HwCommon *hw, int x, int y);

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
    UISolderButton(MSpriteAllocator *sprAlloc, ROData *roData,
                   HwCommon *hw, int x, int y);

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
 * The entire sub-screen UI.
 */
class UISubScreen : public UIObjectList
{
 public:
    UISubScreen();
};


#endif // _UISUBSCREEN_H_

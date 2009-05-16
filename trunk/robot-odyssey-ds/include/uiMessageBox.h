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

#ifndef _UIMESSAGEBOX_H_
#define _UIMESSAGEBOX_H_

#include "uiText.h"
#include "uiBase.h"


class UIOKButton : public UISpriteButton
{
 public:
    UIOKButton(MSpriteAllocator *sprAlloc, UITransient *transient);
    ~UIOKButton();

    virtual void activate();
    virtual void animate();

    static const uint32_t width = 64;
    static const uint32_t height = 32;

 private:
    SpriteImages *allocButtonImage(MSpriteAllocator *sprAlloc);

    static const int arrowX = 35;
    static const int arrowY = 10;

    static UIAnimationSequence::Item animation[];
    UIAnimationSequence animSequence;

    SpriteImages *buttonImage;
    SpriteImages *arrowImage;
    UITransient *transient;
};


class UIMessageBoxText : public UITextLayer
{
public:
    void init(const char *format, va_list v);

protected:
    virtual void paint();
    char buffer[1024];
};


class UIMessageBox : public UITransient
{
 public:
    UIMessageBox(const char *format, ...);

    virtual void updateState();

 private:
    friend class UIMessageBoxText;

    static const int frameWidth = 192;
    static const int frameHeight = 100;
    static const int outerMargin = (SCREEN_WIDTH - frameWidth) / 2;
    static const int innerMargin = 5;
    static const int buttonX = SCREEN_WIDTH + 3;
    static const int buttonY = SCREEN_HEIGHT - 40;

    MSpriteAllocator sprAlloc;
    UIMessageBoxText text;
    UIOKButton button;
};


#endif // _UIMESSAGEBOX_H_

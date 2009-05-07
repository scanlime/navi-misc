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

#ifndef _UIOBJECT_H_
#define _UIOBJECT_H_

#include <list>


/*
 * The current state of all input devices.
 * Passed to each UIObject when it's polled every frame.
 */
class UIInputState
{
 public:
    uint32_t keysHeld;
    uint32_t keysPressed;
    uint32_t keysReleased;
    int touchX;
    int touchY;
};


/*
 * Base class for a UI object. This is a trivial base class that does
 * nothing. It isn't abstract, since there are no mandatory functions
 * for subclasses.
 */
class UIObject
{
 public:
    virtual void animate(void);
    virtual void updateState(void);
    virtual void handleInput(const UIInputState &input);

    /*
     * There is a global frame counter, updated at the beginning of
     * the UIObjectList interrupt handler.
     */
    static uint32_t frameCount;
};


/*
 * A collection of UIObjects, itself also a UIObject.
 *
 * Any UIObjectList can be 'made current', which means that a vblank
 * interrupt handler is installed which updates all UIObjects on that
 * list during the ISR.
 *
 * If the UIObjectList is current, the 'objects' list must not be
 * modified by the main program, only by callbacks that run in the
 * ISR.
 */
class UIObjectList : public UIObject
{
 public:
    std::list<UIObject*> objects;

    virtual void animate();
    virtual void updateState();
    virtual void handleInput(const UIInputState &input);

    void makeCurrent();

 private:
    static UIObjectList *currentList;
    static void vblankISR();
    static void scanInput(UIInputState &input);
};


#endif // _UIOBJECT_H_

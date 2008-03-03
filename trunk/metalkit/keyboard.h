/* -*- Mode: C; c-basic-offset: 3 -*-
 *
 * keyboard.h - Simple PC keyboard driver. Translates scancodes
 *              to a superset of ASCII.
 *
 * Copyright (c) 2008 Micah Dowty
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include "types.h"

/*
 * Symbols for all keys that don't map directly to ASCII
 */

#define KEY_NONE        0x00
#define KEY_ESCAPE      0x1B
#define KEY_BACKSPACE   '\b'
#define KEY_ENTER       '\n'
#define KEY_LCTRL       0x80
#define KEY_LSHIFT      0x81
#define KEY_RCTRL       0x82
#define KEY_RSHIFT      0x83
#define KEY_CAPSLOCK    0x84
#define KEY_NUMLOCK     0x85
#define KEY_SCROLLLOCK  0x86
#define KEY_F1          0x87
#define KEY_F2          0x88
#define KEY_F3          0x89
#define KEY_F4          0x8A
#define KEY_F5          0x8B
#define KEY_F6          0x8C
#define KEY_F7          0x8D
#define KEY_F8          0x8E
#define KEY_F9          0x8F
#define KEY_F10         0x90
#define KEY_F11         0x91
#define KEY_F12         0x92
#define KEY_HOME        0x93
#define KEY_END         0x94
#define KEY_PGUP        0x95
#define KEY_PGDOWN      0x96
#define KEY_UP          0x97
#define KEY_DOWN        0x98
#define KEY_LEFT        0x99
#define KEY_RIGHT       0x9A
#define KEY_INSERT      0x9B
#define KEY_DELETE      0x9C

typedef uint8 Keycode;

typedef struct KeyEvent {
   Keycode key;
   Bool    pressed;
} KeyEvent;

typedef void (KeyboardIRQHandler*)(KeyEvent *event);

void Keyboard_Init(void);
Bool Keyboard_IsKeyPressed(Keycode key);
void Keyboard_SetHandler(KeyboardIRQHandler handler);

#endif /* __VGA_TEXT_H__ */

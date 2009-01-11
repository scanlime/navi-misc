/* -*- Mode: C; c-basic-offset: 3 -*-
 *
 * keyboard.c - Simple PC keyboard driver. Translates scancodes
 *              to a superset of ASCII.
 *
 * This file is part of Metalkit, a simple collection of modules for
 * writing software that runs on the bare metal. Get the latest code
 * at http://svn.navi.cx/misc/trunk/metalkit/
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

#include "keyboard.h"
#include "io.h"
#include "intr.h"

/*
 * Keyboard hardware definitions
 */

#define KB_IRQ          1
#define KB_BUFFER_PORT  0x60
#define KB_CMD_PORT     0x64       // Write for command
#define KB_STATUS_PORT  0x64       // Read for status
#define KB_STATUS_IBF   (1 << 0)   // Input buffer full
#define KB_STATUS_OBF   (1 << 1)   // Output buffer full   
#define KB_CMD_RCB      0x20       // Read command byte
#define KB_CMD_WCB      0x60       // Write command byte
#define KB_CB_INT       (1 << 0)   // IBF Interrupt enabled

/*
 * Global keyboard state
 */

static struct {
   KeyboardIRQHandler handler;
   uint32 keyDown[roundup(KEY_MAX, 32)];
   uint32 scState;
} gKeyboard;


/*
 * KeyboardWrite --
 *
 *    Blocking write to the keyboard controller's buffer.
 *    This can be used to send data to the keyboard itself,
 *    or to send a command parameter.
 */

static void
KeyboardWrite(uint8 byte)
{
   while (IO_In8(KB_STATUS_PORT) & KB_STATUS_OBF);
   IO_Out8(KB_BUFFER_PORT, byte);
}


/*
 * KeyboardRead --
 *
 *    Blocking read from the keyboard controller's buffer.
 *    This can be used to read data from the keyboard itself,
 *    or to read a command parameter.
 */

static uint8
KeyboardRead(void)
{
   while (!(IO_In8(KB_STATUS_PORT) & KB_STATUS_IBF));
   return IO_In8(KB_BUFFER_PORT);
}


/*
 * KeyboardWriteCB --
 *
 *    Blocking write to the keyboard controller's command byte.
 */

static void
KeyboardWriteCB(uint8 byte)
{
   while (IO_In8(KB_STATUS_PORT) & KB_STATUS_OBF);
   IO_Out8(KB_CMD_PORT, KB_CMD_WCB);
   KeyboardWrite(byte);
}   


/*
 * KeyboardReadCB --
 *
 *    Blocking read from the keyboard controller's command byte.
 */

static uint8
KeyboardReadCB(void)
{
   while (IO_In8(KB_STATUS_PORT) & KB_STATUS_OBF);
   IO_Out8(KB_CMD_PORT, KB_CMD_RCB);
   return KeyboardRead();
}


/*
 * KeyboardSetKeyPressed --
 *
 *    Set a key's up/down state.
 */

static void
KeyboardSetKeyPressed(Keycode k, Bool down)
{
   uint32 mask = 1 << (k & 0x1F);
   if (down) {
      gKeyboard.keyDown[k >> 5] |= mask; 
   } else {
      gKeyboard.keyDown[k >> 5] &= ~mask;
   }
}


/*
 * KeyboardTranslate --
 *
 *    Translate scancodes to keycodes when possible, and update
 *    internal state: the scancode state machine, and the up/down
 *    state of all keys.
 */ 

static void
KeyboardTranslate(KeyEvent *event)
{
   /*
    * XXX: We hardcode a US-Ascii QWERTY layout.
    */
   static const Keycode normalMap[] = {
      /* 00 */  KEY_NONE,
      /* 01 */  KEY_ESCAPE,
      /* 02 */  '1',
      /* 03 */  '2',
      /* 04 */  '3',
      /* 05 */  '4',
      /* 06 */  '5',
      /* 07 */  '6',
      /* 08 */  '7',
      /* 09 */  '8',
      /* 0a */  '9',
      /* 0b */  '0',
      /* 0c */  '-',
      /* 0d */  '=',
      /* 0e */  KEY_BACKSPACE,
      /* 0f */  KEY_TAB,
      /* 10 */  'q',
      /* 11 */  'w',
      /* 12 */  'e',
      /* 13 */  'r',
      /* 14 */  't',
      /* 15 */  'y',
      /* 16 */  'u',
      /* 17 */  'i',
      /* 18 */  'o',
      /* 19 */  'p',
      /* 1a */  '[',
      /* 1b */  ']',
      /* 1c */  KEY_ENTER,
      /* 1d */  KEY_LCTRL,
      /* 1e */  'a',
      /* 1f */  's',
      /* 20 */  'd',
      /* 21 */  'f',
      /* 22 */  'g',
      /* 23 */  'h',
      /* 24 */  'j',
      /* 25 */  'k',
      /* 26 */  'l',
      /* 27 */  ';',
      /* 28 */  '\'',
      /* 29 */  '`',
      /* 2a */  KEY_LSHIFT,
      /* 2b */  '\\',
      /* 2c */  'z',
      /* 2d */  'x',
      /* 2e */  'c',
      /* 2f */  'v',
      /* 30 */  'b',
      /* 31 */  'n',
      /* 32 */  'm',
      /* 33 */  ',',
      /* 34 */  '.',
      /* 35 */  '/',
      /* 36 */  KEY_RSHIFT,
      /* 37 */  '*',
      /* 38 */  KEY_LALT,
      /* 39 */  ' ',
      /* 3a */  KEY_CAPSLOCK,
      /* 3b */  KEY_F1,
      /* 3c */  KEY_F2,
      /* 3d */  KEY_F3,
      /* 3e */  KEY_F4,
      /* 3f */  KEY_F5,
      /* 40 */  KEY_F6,
      /* 41 */  KEY_F7,
      /* 42 */  KEY_F8,
      /* 43 */  KEY_F9,
      /* 44 */  KEY_F10,
      /* 45 */  KEY_NUMLOCK,
      /* 46 */  KEY_SCROLLLOCK,
      /* 47 */  '7',  // Numpad
      /* 48 */  '8',
      /* 49 */  '9',
      /* 4a */  '+',
      /* 4b */  '4',
      /* 4c */  '5',
      /* 4d */  '6',
      /* 4e */  '+',
      /* 4f */  '1',
      /* 50 */  '2',
      /* 51 */  '3',
      /* 52 */  '0',
      /* 53 */  '.',
   };

   switch (gKeyboard.scState) {

   case 0:   // 0 - Default state
      
      if (event->scancode == 0xe0) {
	 /* Escape */
	 gKeyboard.scState = 1;

      } else {
	 uint8 code = event->scancode;

	 if (code & 0x80) {
	    code &= 0x7F;
	 } else {
	    event->pressed = 1;
	 }
	    
	 event->key = code < arraysize(normalMap) ? normalMap[code] : KEY_NONE;

	 KeyboardSetKeyPressed(event->key, event->pressed);
      }
      break;

   case 1:   // 1 - Escaped state
      gKeyboard.scState = 0;
      break;

   }
}


/*
 * KeyboardHandlerInternal --
 *
 *    This is the low-level keyboard interrupt handler.  We convert
 *    the incoming key into a Keycode, modify our key state table, and
 *    pass it on to any registered KeyboardIRQHandler.
 */ 

static void
KeyboardHandlerInternal(int vector)
{
   KeyEvent event;

   event.scancode = KeyboardRead();
   event.key = 0;
   event.pressed = 0;

   KeyboardTranslate(&event);

   if (gKeyboard.handler) {
      gKeyboard.handler(&event);
   }
}


/*
 * Keyboard_Init --
 *
 *    Set up the keyboard driver. This installs our default IRQ
 *    handler, and initializes the key table. The IRQ module must be
 *    initialized before this is called.
 *
 *    As a side-effect, this will unmask the keyboard IRQ and install
 *    a handler.
 */

fastcall void
Keyboard_Init(void)
{
   /*
    * Enable the keyboard IRQ
    */
   KeyboardWriteCB(KeyboardReadCB() | KB_CB_INT);

   Intr_SetMask(KB_IRQ, TRUE);
   Intr_SetHandler(IRQ_VECTOR(KB_IRQ), KeyboardHandlerInternal);
}


/*
 * Keyboard_IsKeyPressed --
 *
 *    Check whether a key, identified by Keycode, is down.
 */

fastcall Bool
Keyboard_IsKeyPressed(Keycode k)
{
   if (k < KEY_MAX) {
      return (gKeyboard.keyDown[k >> 5] >> (k & 0x1F)) != 0;
   }
   return FALSE;
}


/*
 * Keyboard_SetHandler --
 *
 *    Set a handler that will receive translated keys and scancodes.
 *    This handler is run within the IRQ handler, so it must complete
 *    quickly and use minimal stack space.
 *
 *    The handler will be called once per scancode byte, regardless of
 *    whether that byte ended a key event or not. If event->key is
 *    zero, the event can be ignored unless you're interested in
 *    seeing the raw scancodes.
 */

fastcall void
Keyboard_SetHandler(KeyboardIRQHandler handler)
{
   gKeyboard.handler = handler;
}

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
   Bool escape;
   KeyboardIRQHandler handler;
   uint32 keyDown[roundup(KEY_MAX, 32)];
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
   enum {
      S_NORMAL = 0,
      S_SHIFTED,
      S_ESCAPED,
   };

   /*
    * XXX: We hardcode a US-Ascii QWERTY layout.
    */
   static const Keycode kbmap[][3] = {
      /*          S_NORMAL        S_SHIFTED       S_ESCAPED */
      /* 00 */  { KEY_NONE,       KEY_NONE,       KEY_NONE },
      /* 01 */  { KEY_ESCAPE,     KEY_ESCAPE,     KEY_NONE },
      /* 02 */  { '1',            '!',            KEY_NONE },
      /* 03 */  { '2',            '@',            KEY_NONE },
      /* 04 */  { '3',            '#',            KEY_NONE },
      /* 05 */  { '4',            '$',            KEY_NONE },
      /* 06 */  { '5',            '%',            KEY_NONE },
      /* 07 */  { '6',            '^',            KEY_NONE },
      /* 08 */  { '7',            '&',            KEY_NONE },
      /* 09 */  { '8',            '*',            KEY_NONE },
      /* 0a */  { '9',            '(',            KEY_NONE },
      /* 0b */  { '0',            ')',            KEY_NONE },
      /* 0c */  { '-',            '_',            KEY_NONE },
      /* 0d */  { '=',            '+',            KEY_NONE },
      /* 0e */  { KEY_BACKSPACE,  KEY_BACKSPACE,  KEY_NONE },
      /* 0f */  { KEY_TAB,        KEY_TAB,        KEY_NONE },
      /* 10 */  { 'q',            'Q',            KEY_NONE },
      /* 11 */  { 'w',            'W',            KEY_NONE },
      /* 12 */  { 'e',            'E',            KEY_NONE },
      /* 13 */  { 'r',            'R',            KEY_NONE },
      /* 14 */  { 't',            'T',            KEY_NONE },
      /* 15 */  { 'y',            'Y',            KEY_NONE },
      /* 16 */  { 'u',            'U',            KEY_NONE },
      /* 17 */  { 'i',            'I',            KEY_NONE },
      /* 18 */  { 'o',            'O',            KEY_NONE },
      /* 19 */  { 'p',            'P',            KEY_NONE },
      /* 1a */  { '[',            '{',            KEY_NONE },
      /* 1b */  { ']',            '}',            KEY_NONE },
      /* 1c */  { KEY_ENTER,      KEY_ENTER,      KEY_ENTER },
      /* 1d */  { KEY_LCTRL,      KEY_LCTRL,      KEY_RCTRL },
      /* 1e */  { 'a',            'A',            KEY_NONE },
      /* 1f */  { 's',            'S',            KEY_NONE },
      /* 20 */  { 'd',            'D',            KEY_NONE },
      /* 21 */  { 'f',            'F',            KEY_NONE },
      /* 22 */  { 'g',            'G',            KEY_NONE },
      /* 23 */  { 'h',            'H',            KEY_NONE },
      /* 24 */  { 'j',            'J',            KEY_NONE },
      /* 25 */  { 'k',            'K',            KEY_NONE },
      /* 26 */  { 'l',            'L',            KEY_NONE },
      /* 27 */  { ';',            ':',            KEY_NONE },
      /* 28 */  { '\'',           '"',            KEY_NONE },
      /* 29 */  { '`',            '~',            KEY_NONE },
      /* 2a */  { KEY_LSHIFT,     KEY_LSHIFT,     KEY_NONE },
      /* 2b */  { '\\',           '|',            KEY_NONE },
      /* 2c */  { 'z',            'Z',            KEY_NONE },
      /* 2d */  { 'x',            'X',            KEY_NONE },
      /* 2e */  { 'c',            'C',            KEY_NONE },
      /* 2f */  { 'v',            'V',            KEY_NONE },
      /* 30 */  { 'b',            'B',            KEY_NONE },
      /* 31 */  { 'n',            'N',            KEY_NONE },
      /* 32 */  { 'm',            'M',            KEY_NONE },
      /* 33 */  { ',',            '<',            KEY_NONE },
      /* 34 */  { '.',            '>',            KEY_NONE },
      /* 35 */  { '/',            '?',            '/' },
      /* 36 */  { KEY_RSHIFT,     KEY_RSHIFT,     KEY_NONE },
      /* 37 */  { '*',            '*',            KEY_CTRL_PRTSCN },
      /* 38 */  { KEY_LALT,       KEY_LALT,       KEY_RALT },
      /* 39 */  { ' ',            ' ',            KEY_NONE },
      /* 3a */  { KEY_CAPSLOCK,   KEY_CAPSLOCK,   KEY_NONE },
      /* 3b */  { KEY_F1,         KEY_F1,         KEY_NONE },
      /* 3c */  { KEY_F2,         KEY_F2,         KEY_NONE },
      /* 3d */  { KEY_F3,         KEY_F3,         KEY_NONE },
      /* 3e */  { KEY_F4,         KEY_F4,         KEY_NONE },
      /* 3f */  { KEY_F5,         KEY_F5,         KEY_NONE },
      /* 40 */  { KEY_F6,         KEY_F6,         KEY_NONE },
      /* 41 */  { KEY_F7,         KEY_F6,         KEY_NONE },
      /* 42 */  { KEY_F8,         KEY_F7,         KEY_NONE },
      /* 43 */  { KEY_F9,         KEY_F8,         KEY_NONE },
      /* 44 */  { KEY_F10,        KEY_F9,         KEY_NONE },
      /* 45 */  { KEY_NUMLOCK,    KEY_NUMLOCK,    KEY_NONE },
      /* 46 */  { KEY_SCROLLLOCK, KEY_SCROLLLOCK, KEY_CTRL_BREAK },
      /* 47 */  { '7',            '7',            KEY_HOME },
      /* 48 */  { '8',            '8',            KEY_UP },
      /* 49 */  { '9',            '9',            KEY_PGUP },
      /* 4a */  { '-',            '-',            KEY_NONE },
      /* 4b */  { '4',            '4',            KEY_LEFT },
      /* 4c */  { '5',            '5',            KEY_NONE },
      /* 4d */  { '6',            '6',            KEY_RIGHT },
      /* 4e */  { '+',            '+',            KEY_NONE },
      /* 4f */  { '1',            '1',            KEY_END },
      /* 50 */  { '2',            '2',            KEY_DOWN },
      /* 51 */  { '3',            '3',            KEY_PGDOWN },
      /* 52 */  { '0',            '0',            KEY_INSERT },
      /* 53 */  { '.',            '.',            KEY_DELETE },
   };

   uint8 scancode = event->scancode & 0x7F;
   event->pressed = (event->scancode & 0x80) == 0;

   if (event->scancode == 0xe0) {
      /*
       * Begin an escape sequence.
       */

      gKeyboard.escape = TRUE;

   } else if (scancode >= KEY_MAX) {
      /*
       * Unsupported scancode.
       */

   } else if (gKeyboard.escape) {
      /*
       * Escaped key.
       */
      gKeyboard.escape = FALSE;
      event->rawKey = kbmap[scancode][S_ESCAPED];
      event->key = event->rawKey;

   } else {
      /*
       * Non-escaped key.
       */

      event->rawKey = kbmap[scancode][S_NORMAL];

      if (Keyboard_IsKeyPressed(KEY_LSHIFT) ||
          Keyboard_IsKeyPressed(KEY_RSHIFT)) {

         event->key = kbmap[scancode][S_SHIFTED];
      } else {
         event->key = event->rawKey;
      }
   }

   KeyboardSetKeyPressed(event->rawKey, event->pressed);
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
   KeyEvent event = { KeyboardRead() };

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

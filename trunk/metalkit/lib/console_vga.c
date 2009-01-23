/* -*- Mode: C; c-basic-offset: 3 -*-
 *
 * console_vga.c - Console driver for VGA text mode.
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

#include "types.h"
#include "console_vga.h"
#include "io.h"
#include "intr.h"

#define VGA_TEXT_FRAMEBUFFER     ((uint8*)0xB8000)

#define VGA_CRTCREG_CURSOR_LOC_HIGH  0x0E
#define VGA_CRTCREG_CURSOR_LOC_LOW   0x0F

typedef struct {
   uint16 crtc_iobase;
   struct {
      int8 x, y;
   } cursor;
   int8 attr;
} ConsoleVGAObject;

ConsoleVGAObject gConsoleVGA[1];


/*
 * ConsoleVGAWriteCRTC --
 *
 *    Write to a VGA CRT Control register.
 */

static fastcall void
ConsoleVGAWriteCRTC(uint8 addr, uint8 value)
{
   ConsoleVGAObject *self = gConsoleVGA;
   IO_Out8(self->crtc_iobase, addr);
   IO_Out8(self->crtc_iobase + 1, value);
}


/*
 * ConsoleVGAMoveHardwareCursor --
 *
 *    Set the hardware cursor to the current cursor position.
 */

static fastcall void
ConsoleVGAMoveHardwareCursor(void)
{
   ConsoleVGAObject *self = gConsoleVGA;
   uint16 loc = self->cursor.x + self->cursor.y * VGA_TEXT_WIDTH;

   ConsoleVGAWriteCRTC(VGA_CRTCREG_CURSOR_LOC_LOW, loc & 0xFF);
   ConsoleVGAWriteCRTC(VGA_CRTCREG_CURSOR_LOC_HIGH, loc >> 8);
}


/*
 * ConsoleVGAMoveTo --
 *
 *    Set the text insertion point. This will move the hardware cursor
 *    at the next Console_Flush(). 
 */

static fastcall void
ConsoleVGAMoveTo(int x, int y)
{
   ConsoleVGAObject *self = gConsoleVGA;

   self->cursor.x = x;
   self->cursor.y = y;
}


/*
 * ConsoleVGA_Clear --
 *
 *    Clear the screen and move the cursor to the home position.
 */

static fastcall void
ConsoleVGAClear(void)
{
   ConsoleVGAObject *self = gConsoleVGA;
   uint8 *fb = VGA_TEXT_FRAMEBUFFER;
   int i, j;

   ConsoleVGAMoveTo(0, 0);

   for (j = 0; j < VGA_TEXT_HEIGHT; j++) {
      for (i = 0; i < VGA_TEXT_WIDTH; i++) {
         fb[0] = ' ';
         fb[1] = self->attr;
         fb += 2;
      }
   }
}


/*
 * ConsoleVGA_SetColor --
 *
 *    Set the text foreground color.
 */

fastcall void
ConsoleVGA_SetColor(int8 fgColor)
{
   ConsoleVGAObject *self = gConsoleVGA;

   self->attr &= 0xF0;
   self->attr |= fgColor;
}


/*
 * ConsoleVGA_SetColor --
 *
 *    Set the text background color.
 */

fastcall void
ConsoleVGA_SetBgColor(int8 bgColor)
{
   ConsoleVGAObject *self = gConsoleVGA;

   self->attr &= 0x0F;
   self->attr |= bgColor << 4;
}


/*
 * ConsoleVGAWriteChar --
 *
 *    Write one character, TTY-style. Interprets \n characters.
 */

static fastcall void
ConsoleVGAWriteChar(char c)
{
   ConsoleVGAObject *self = gConsoleVGA;
   uint8 *fb = VGA_TEXT_FRAMEBUFFER;

   if (c == '\n') {
      self->cursor.y++;
      self->cursor.x = 0;

   } else if (c == '\t') {
      while (self->cursor.x & 7) {
         ConsoleVGAWriteChar(' ');
      }

   } else if (c == '\b') {
      if (self->cursor.x > 0) {
         self->cursor.x--;
         ConsoleVGAWriteChar(' ');
         self->cursor.x--;
      }

   } else {
      fb += self->cursor.x * 2 + self->cursor.y * VGA_TEXT_WIDTH * 2;
      fb[0] = c;
      fb[1] = self->attr;
      self->cursor.x++;
   }

   if (self->cursor.x >= VGA_TEXT_WIDTH) {
      self->cursor.x = 0;
      self->cursor.y++;
   }

   if (self->cursor.y >= VGA_TEXT_HEIGHT) {
      int i;
      uint8 *fb = VGA_TEXT_FRAMEBUFFER;
      const uint32 scrollSize = VGA_TEXT_WIDTH * 2 * (VGA_TEXT_HEIGHT - 1);

      self->cursor.y = VGA_TEXT_HEIGHT - 1;

      memcpy(fb, fb + VGA_TEXT_WIDTH * 2, scrollSize);
      fb += scrollSize;
      for (i = 0; i < VGA_TEXT_WIDTH; i++) {
         fb[0] = ' ';
         fb[1] = self->attr;
         fb += 2;
      }
   }
}


/*
 * ConsoleVGABeginPanic --
 *
 *    Prepare for a panic in VGA mode: Set up the panic colors,
 *    and clear the screen.
 */

static fastcall void
ConsoleVGABeginPanic(void)
{
   ConsoleVGA_SetColor(VGA_COLOR_WHITE);
   ConsoleVGA_SetBgColor(VGA_COLOR_RED);
   ConsoleVGAClear();
   ConsoleVGAMoveHardwareCursor();
}


/*
 * ConsoleVGA_Init --
 *
 *    Perform first-time initialization for VGA text mode,
 *    set VGA as the current console driver, and clear the
 *    screen with a default color.
 */

fastcall void
ConsoleVGA_Init(void)
{
   ConsoleVGAObject *self = gConsoleVGA;

   /*
    * Read the I/O address select bit, to determine where the CRTC
    * registers are.
    */
   if (IO_In8(0x3CC) & 1) {
      self->crtc_iobase = 0x3D4;
   } else {
      self->crtc_iobase = 0x3B4;
   }

   gConsole.beginPanic = ConsoleVGABeginPanic;
   gConsole.clear = ConsoleVGAClear;
   gConsole.moveTo = ConsoleVGAMoveTo;
   gConsole.writeChar = ConsoleVGAWriteChar;
   gConsole.flush = ConsoleVGAMoveHardwareCursor;

   ConsoleVGA_SetColor(VGA_COLOR_WHITE);
   ConsoleVGA_SetBgColor(VGA_COLOR_BLUE);

   ConsoleVGAClear();
   ConsoleVGAMoveHardwareCursor();
}

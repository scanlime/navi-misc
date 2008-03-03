/* -*- Mode: C; c-basic-offset: 3 -*-
 *
 * vgatext.c - Simple VGA text mode driver
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
#include "vgatext.h"
#include "io.h"

#define VGA_TEXT_FRAMEBUFFER     ((uint8*)0xB8000)

#define VGA_CRTCREG_CURSOR_LOC_HIGH  0x0E
#define VGA_CRTCREG_CURSOR_LOC_LOW   0x0F

struct {
   uint16 crtc_iobase;
   struct {
      int8 x, y;
   } cursor;
   int8 attr;
} vgatext;


/*
 * VGATextWriteCRTC --
 *
 *    Write to a VGA CRT Control register.
 */

static void
VGATextWriteCRTC(uint8 addr, uint8 value)
{
   IO_Out8(vgatext.crtc_iobase, addr);
   IO_Out8(vgatext.crtc_iobase + 1, value);
}


/*
 * VGATextMoveHardwareCursor --
 *
 *    Set the hardware cursor to the current cursor position.
 */

static void
VGATextMoveHardwareCursor(void)
{
   uint16 loc = vgatext.cursor.x + vgatext.cursor.y * VGA_TEXT_WIDTH;

   VGATextWriteCRTC(VGA_CRTCREG_CURSOR_LOC_LOW, loc & 0xFF);
   VGATextWriteCRTC(VGA_CRTCREG_CURSOR_LOC_HIGH, loc >> 8);
}


/*
 * VGAText_Init --
 *
 *    Perform first-time initialization for VGA text mode,
 *    and clear the screen with a default color.
 */

void
VGAText_Init(void)
{
   /*
    * Read the I/O address select bit, to determine where the CRTC
    * registers are.
    */
   if (IO_In8(0x3CC) & 1) {
      vgatext.crtc_iobase = 0x3D4;
   } else {
      vgatext.crtc_iobase = 0x3B4;
   }

   VGAText_Clear(VGA_COLOR_WHITE, VGA_COLOR_BLUE);
}


/*
 * VGAText_Clear --
 *
 *    Clear the screen, change the foreground and background color,
 *    and move the cursor to the home position.
 */

void
VGAText_Clear(int8 fgColor, int8 bgColor)
{
   int i, j;
   uint8 *fb = VGA_TEXT_FRAMEBUFFER;

   VGAText_SetColor(fgColor);
   VGAText_SetBgColor(bgColor);
   VGAText_MoveTo(0, 0);

   for (j = 0; j < VGA_TEXT_HEIGHT; j++) {
      for (i = 0; i < VGA_TEXT_WIDTH; i++) {
	 fb[0] = ' ';
	 fb[1] = vgatext.attr;
	 fb += 2;
      }
   }
}


/*
 * VGAText_SetColor --
 *
 *    Set the text foreground color.
 */

void
VGAText_SetColor(int8 fgColor)
{
   vgatext.attr &= 0xF0;
   vgatext.attr |= fgColor;
}


/*
 * VGAText_SetColor --
 *
 *    Set the text background color.
 */

void
VGAText_SetBgColor(int8 bgColor)
{
   vgatext.attr &= 0x0F;
   vgatext.attr |= bgColor << 4;
}


/*
 * VGAText_MoveTo --
 *
 *    Move the hardware cursor and the text insertion point.
 */

void
VGAText_MoveTo(int x, int y)
{
   vgatext.cursor.x = x;
   vgatext.cursor.y = y;
   VGATextMoveHardwareCursor();
}


/*
 * VGATextWriteChar --
 *
 *    This is an internal version of VGAText_WriteChar which does
 *    not call VGATextMoveHardwareCursor. We try not to move the
 *    cursor after each character write, since it involves slow
 *    I/O port writes.
 */

static void
VGATextWriteChar(char c)
{
   uint8 *fb = VGA_TEXT_FRAMEBUFFER;

   switch (c) {

   case '\n':
      vgatext.cursor.y++;
      vgatext.cursor.x = 0;
      break;

   case '\r':
      break;

   case '\t':
      vgatext.cursor.x = (vgatext.cursor.x & ~7) + 8;
      break;

   default:
      fb += vgatext.cursor.x * 2 + vgatext.cursor.y * VGA_TEXT_WIDTH * 2;
      fb[0] = c;
      fb[1] = vgatext.attr;
      vgatext.cursor.x++;
      break;
   }

   if (vgatext.cursor.x >= VGA_TEXT_WIDTH) {
      vgatext.cursor.x = 0;
      vgatext.cursor.y++;
   }

   if (vgatext.cursor.y >= VGA_TEXT_HEIGHT) {
      int i, j;
      uint8 *fb = VGA_TEXT_FRAMEBUFFER;

      vgatext.cursor.y = VGA_TEXT_HEIGHT - 1;

      for (j = 0; j < VGA_TEXT_HEIGHT - 1; j++) {
	 for (i = 0; i < VGA_TEXT_WIDTH; i++) {
	    fb[0] = fb[VGA_TEXT_WIDTH * 2];
	    fb[1] = fb[VGA_TEXT_WIDTH * 2 + 1];
	    fb += 2;
	 }
      }
      for (i = 0; i < VGA_TEXT_WIDTH; i++) {
	 fb[0] = ' ';
	 fb[1] = vgatext.attr;
	 fb += 2;
      }
   }
}


/*
 * VGAText_WriteChar --
 *
 *    Write one character, advancing the cursor and scrolling
 *    as necessary. Interprets control characters.
 */

void
VGAText_WriteChar(char c)
{
   VGATextWriteChar(c);
   VGATextMoveHardwareCursor();
}


/*
 * VGAText_WriteString --
 *
 *    Write a NUL-terminated character string.
 */

void
VGAText_WriteString(char *str)
{
   while (*str) {
      VGAText_WriteChar(*str);
      str++;
   }
   VGATextMoveHardwareCursor();
}


/*
 * VGAText_WriteHex --
 *
 *    Write a variable-length hexadecimal integer.
 */

void
VGAText_WriteHex(int num, int digits)
{
   static const char lookup[] = "0123456789ABCDEF";
   int shift = (digits - 1) << 2;

   while (digits > 0) {
      int digit = (num >> shift) & 0x0F;
      VGATextWriteChar(lookup[digit]);
      digits--;
      shift -= 4;
   }
   VGATextMoveHardwareCursor();
}

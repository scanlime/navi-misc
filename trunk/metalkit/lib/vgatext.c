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
} VGATextObject;

VGATextObject *gVGAText;


/*
 * VGATextWriteCRTC --
 *
 *    Write to a VGA CRT Control register.
 */

static void
VGATextWriteCRTC(uint8 addr, uint8 value)
{
   VGATextObject *self = gVGAText;
   IO_Out8(self->crtc_iobase, addr);
   IO_Out8(self->crtc_iobase + 1, value);
}


/*
 * VGATextMoveHardwareCursor --
 *
 *    Set the hardware cursor to the current cursor position.
 */

static void
VGATextMoveHardwareCursor(void)
{
   VGATextObject *self = gVGAText;
   uint16 loc = self->cursor.x + self->cursor.y * VGA_TEXT_WIDTH;

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
   VGATextObject *self = gVGAText;

   /*
    * Read the I/O address select bit, to determine where the CRTC
    * registers are.
    */
   if (IO_In8(0x3CC) & 1) {
      self->crtc_iobase = 0x3D4;
   } else {
      self->crtc_iobase = 0x3B4;
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
   VGATextObject *self = gVGAText;
   int i, j;
   uint8 *fb = VGA_TEXT_FRAMEBUFFER;

   VGAText_SetColor(fgColor);
   VGAText_SetBgColor(bgColor);
   VGAText_MoveTo(0, 0);

   for (j = 0; j < VGA_TEXT_HEIGHT; j++) {
      for (i = 0; i < VGA_TEXT_WIDTH; i++) {
         fb[0] = ' ';
         fb[1] = self->attr;
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
   VGATextObject *self = gVGAText;

   self->attr &= 0xF0;
   self->attr |= fgColor;
}


/*
 * VGAText_SetColor --
 *
 *    Set the text background color.
 */

void
VGAText_SetBgColor(int8 bgColor)
{
   VGATextObject *self = gVGAText;

   self->attr &= 0x0F;
   self->attr |= bgColor << 4;
}


/*
 * VGAText_MoveTo --
 *
 *    Move the hardware cursor and the text insertion point.
 */

void
VGAText_MoveTo(int x, int y)
{
   VGATextObject *self = gVGAText;

   self->cursor.x = x;
   self->cursor.y = y;
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
   VGATextObject *self = gVGAText;
   uint8 *fb = VGA_TEXT_FRAMEBUFFER;

   if (c == '\n') {
      self->cursor.y++;
      self->cursor.x = 0;
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
      int i, j;
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
VGAText_WriteString(const char *str)
{
   char c;
   while ((c = *(str++))) {
      VGAText_WriteChar(c);
   }
   VGATextMoveHardwareCursor();
}


/*
 * VGAText_WriteChars --
 *
 *    Write a fixed number of characters.
 */

void
VGAText_WriteChars(const char *chars, int count)
{
   while (count--) {
      VGAText_WriteChar(*(chars++));
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


/*
 * VGAText_Format --
 *
 *    Write a formatted string. This is a very tiny subset of printf().
 */

void
VGAText_Format(const char *fmt, ...)
{
   int *arg = (int*)&fmt;
   char c;

   while ((c = *(fmt++))) {
      int width = 0;

      if (c != '%') {
         VGATextWriteChar(c);
         continue;
      }

      while ((c = *(fmt++))) {
         if (c >= '0' && c <= '9') {
            width = c - '0';
            continue;
         }
         if (c == 's') {
	    VGAText_WriteString((char*) *(++arg));
	    break;
         }
         if (c == 'x') {
            VGAText_WriteHex(*(++arg), width);
            break;
         }
         if (c == 'c') {
            VGAText_WriteChar(*(++arg));
	    break;
	 }
         if (c == 'C') {
	    VGAText_WriteChars((char*) *(++arg), width);
	    break;
         }
      }
   }
   VGATextMoveHardwareCursor();
}


/*
 * VGAText_HexDump --
 *
 *    Write a 32-bit hex dump to the console, labelling each
 *    line with addresses starting at 'startAddr'.
 */

void
VGAText_HexDump(uint32 *data, uint32 startAddr, uint32 numWords)
{
   while (numWords) {
      int32 lineWords = 4;
      VGAText_WriteHex(startAddr, 8);
      VGAText_WriteChar(':');
      while (numWords && lineWords) {
         VGAText_WriteChar(' ');
         VGAText_WriteHex(*data, 8);
         data++;
         startAddr += 4;
         numWords--;
         lineWords--;
      }
      VGAText_WriteChar('\n');
   }
}


/*
 * VGAText_DefaultFaultHandler --
 *
 *    Default fault handler for use in text mode. Prints a
 *    message and halts the machine.
 */

void
VGAText_DefaultFaultHandler(int vector)
{
   IntrContext *ctx = Intr_GetContext(vector);

   /*
    * Using a regular inline string constant, the linker can't
    * optimize out this string when the function isn't used.
    */
   static const char faultFmt[] =
      "Fatal error:\n"
      "Unhandled fault %2x at %4x:%8x\n"
      "\n"
      "eax=%8x ebx=%8x ecx=%8x edx=%8x\n"
      "esi=%8x edi=%8x esp=%8x ebp=%8x eflags=%8x\n"
      "\n";

   VGAText_Init();

   /*
    * IntrContext's stack pointer includes the three values that were
    * pushed by the hardware interrupt. Advance past these, so the
    * stack trace shows the state of execution at the time of the
    * fault rather than at the time our interrupt trampoline was
    * invoked.
    */
   ctx->esp += 3 * sizeof(int);

   VGAText_Format(faultFmt,
                  vector, ctx->cs, ctx->eip,
                  ctx->eax, ctx->ebx, ctx->ecx, ctx->edx,
                  ctx->esi, ctx->edi, ctx->esp, ctx->ebp,
                  ctx->eflags);

   VGAText_HexDump((void*)ctx->esp, ctx->esp, 64);

   Intr_Disable();
   Intr_Halt();
}


/*
 * VGAText_Panic --
 *
 *    Default panic handler for use in text mode. Prints
 *    a caller-defined message, and halts the machine.
 */

void
VGAText_Panic(const char *str)
{
   /*
    * To keep minimum size down, this function shouldn't depend on
    * VGAText_Format.
    */
   VGAText_Init();
   VGAText_WriteString("Panic:\n");
   VGAText_WriteString(str);
   Intr_Disable();
   Intr_Halt();
}

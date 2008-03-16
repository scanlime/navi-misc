/* -*- Mode: C; c-basic-offset: 3 -*-
 *
 * vgatext.h - Simple VGA text mode driver
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

#ifndef __VGA_TEXT_H__
#define __VGA_TEXT_H__

#include "types.h"

#define VGA_COLOR_BLACK          0
#define VGA_COLOR_BLUE           1
#define VGA_COLOR_GREEN          2
#define VGA_COLOR_CYAN           3
#define VGA_COLOR_RED            4
#define VGA_COLOR_MAGENTA        5
#define VGA_COLOR_BROWN          6
#define VGA_COLOR_LIGHT_GRAY     7
#define VGA_COLOR_DARK_GRAY      8
#define VGA_COLOR_LIGHT_BLUE     9
#define VGA_COLOR_LIGHT_GREEN    10
#define VGA_COLOR_LIGHT_CYAN     11
#define VGA_COLOR_LIGHT_RED      12
#define VGA_COLOR_LIGHT_MAGENTA  13
#define VGA_COLOR_YELLOW         14
#define VGA_COLOR_WHITE          15

#define VGA_TEXT_WIDTH           80
#define VGA_TEXT_HEIGHT          25

void VGAText_Init(void);

void VGAText_Clear(int8 fgColor, int8 bgColor);
void VGAText_SetColor(int8 fgColor);
void VGAText_SetBgColor(int8 bgColor);
void VGAText_MoveTo(int x, int y);

void VGAText_WriteChar(char c);
void VGAText_WriteChars(const char *chars, int count);
void VGAText_WriteString(const char *str);
void VGAText_WriteHex(int num, int digits);
void VGAText_Format(const char *fmt, ...);
void VGAText_HexDump(uint32 *data, uint32 startAddr, uint32 numWords);

void VGAText_Panic(const char *str);
void VGAText_DefaultFaultHandler(int number);

#endif /* __VGA_TEXT_H__ */

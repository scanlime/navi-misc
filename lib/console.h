/* -*- Mode: C; c-basic-offset: 3 -*-
 *
 * console.h - Abstract text console
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

#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include "types.h"

typedef struct {
   fastcall void (*beginPanic)(void);       // Initialize the console for a Panic message
   fastcall void (*clear)(void);            // Clear the screen, home the cursor
   fastcall void (*moveTo)(int x, int y);   // Move the cursor
   fastcall void (*writeChar)(char c);      // Write one character, with support for control codes
   fastcall void (*flush)(void);            // Finish writing a string of characters
} ConsoleInterface;

extern ConsoleInterface gConsole;

#define Console_BeginPanic()   gConsole.beginPanic()
#define Console_Clear()        gConsole.clear()
#define Console_MoveTo(x, y)   gConsole.moveTo(x, y)
#define Console_WriteChar(c)   gConsole.writeChar(c)
#define Console_Flush()        gConsole.flush()

fastcall void Console_WriteString(const char *str);
fastcall void Console_WriteUInt32(uint32 num, int digits, char padding, int base, Bool suppressZero);
fastcall void Console_FormatV(const char **args);
fastcall void Console_HexDump(uint32 *data, uint32 startAddr, uint32 numWords);

void Console_Format(const char *fmt, ...);
void Console_Panic(const char *str, ...);
void Console_UnhandledFault(int number);

#endif /* __CONSOLE_H__ */

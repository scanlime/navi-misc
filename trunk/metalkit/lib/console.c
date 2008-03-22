/* -*- Mode: C; c-basic-offset: 3 -*-
 *
 * console.c - Abstract text console
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
#include "console.h"
#include "intr.h"

ConsoleInterface gConsole;


/*
 * Console_WriteString --
 *
 *    Write a NUL-terminated string.
 */

void
Console_WriteString(const char *str)
{
   char c;
   while ((c = *(str++))) {
      Console_WriteChar(c);
   }
}


/*
 * Console_WriteUInt32 --
 *
 *    Write a positive 32-bit integer with arbitrary base from 2 to
 *    16, up to 'digits' characters long. If 'padding' is non-NUL,
 *    this character is used for leading digits that would be zero.
 *    If padding is NUL, leading digits are suppressed entierly.
 */

void
Console_WriteUInt32(uint32 num, int digits, char padding, int base, Bool suppressZero)
{
   if (digits == 0) {
      return;
   }

   Console_WriteUInt32(num / base, digits - 1, padding, base, TRUE);

   if (num == 0 && suppressZero) {
      if (padding) {
	 Console_WriteChar(padding);
      }
   } else {
      uint8 digit = num % base;
      Console_WriteChar(digit >= 10 ? digit - 10 + 'A' : digit + '0');
   }
}


/*
 * Console_Format --
 * Console_FormatV --
 *
 *    Write a formatted string. This is for the most part a tiny
 *    subset of printf(). Supports the standard %c, %s, %d, and %X
 *    specifiers.
 *
 *    Deviates from a standard printf() in a few ways, in the interest
 *    of low-level utility and small code size:
 *
 *     - Adds a nonstandard %b specifier, for binary numbers.
 *     - Width specifiers set an exact width, not a minimum width.
 *     - %x is treated as %X.
 */

void
Console_Format(const char *fmt, ...)
{
   Console_FormatV(&fmt);
}

void
Console_FormatV(const char **args)
{
   char c;
   const char *fmt = *(args++);

   while ((c = *(fmt++))) {
      int width = 0;
      char padding = '\0';

      if (c != '%') {
         Console_WriteChar(c);
         continue;
      }

      while ((c = *(fmt++))) {
	 if (c == '0' && width == 0) {
	    /* If we get a leading 0 in the width specifier, turn on zero-padding */
	    padding = '0';
	    continue;
	 }
         if (c >= '0' && c <= '9') {
	    /* Add another digit to the width specifier */
            width = (width * 10) + (c - '0');
	    if (padding == '\0') {
	       padding = ' ';
	    }
            continue;
         }

	 /*
	  * Any other character means the width specifier has
	  * ended. If it's still zero, set the defaults.
	  */
	 if (width == 0) {
	    width = 32;
	 }

	 /*
	  * Non-integer format specifiers
	  */

         if (c == 's') {
	    Console_WriteString((char*) *(args++));
	    break;
         }
         if (c == 'c') {
            Console_WriteChar((char)(uint32) *(args++));
	    break;
	 }

	 /*
	  * Integers of different bases
	  */
	 int base = 0;

	 if (c == 'X' || c == 'x') {
	    base = 16;
	 } else if (c == 'd') {
	    base = 10;
	 } else if (c == 'b') {
	    base = 2;
	 }

	 if (base) {
	    Console_WriteUInt32((uint32) *(args++), width, padding, base, FALSE);
	 } else {
	    Console_WriteChar(c);
	 }
	 break;
      }
   }
}


/*
 * Console_HexDump --
 *
 *    Write a 32-bit hex dump to the console, labelling each
 *    line with addresses starting at 'startAddr'.
 */

void
Console_HexDump(uint32 *data, uint32 startAddr, uint32 numWords)
{
   while (numWords) {
      int32 lineWords = 4;
      Console_Format("%08x:", startAddr);
      while (numWords && lineWords) {
         Console_Format(" %08x", *data);
         data++;
         startAddr += 4;
         numWords--;
         lineWords--;
      }
      Console_WriteChar('\n');
   }
}


/*
 * Console_UnhandledFault --
 *
 *    Display a fatal error message with register and stack trace when
 *    an unhandled fault occurs. This fault handler must be installed
 *    using the Intr module.
 */

void
Console_UnhandledFault(int vector)
{
   IntrContext *ctx = Intr_GetContext(vector);

   /*
    * Using a regular inline string constant, the linker can't
    * optimize out this string when the function isn't used.
    */
   static const char faultFmt[] =
      "Fatal error:\n"
      "Unhandled fault %d at %04x:%08x\n"
      "\n"
      "eax=%08x ebx=%08x ecx=%08x edx=%08x\n"
      "esi=%08x edi=%08x esp=%08x ebp=%08x\n"
      "eflags=%032b\n"
      "\n";

   Console_BeginPanic();

   /*
    * IntrContext's stack pointer includes the three values that were
    * pushed by the hardware interrupt. Advance past these, so the
    * stack trace shows the state of execution at the time of the
    * fault rather than at the time our interrupt trampoline was
    * invoked.
    */
   ctx->esp += 3 * sizeof(int);

   Console_Format(faultFmt,
                  vector, ctx->cs, ctx->eip,
                  ctx->eax, ctx->ebx, ctx->ecx, ctx->edx,
                  ctx->esi, ctx->edi, ctx->esp, ctx->ebp,
                  ctx->eflags);

   Console_HexDump((void*)ctx->esp, ctx->esp, 64);

   Console_Flush();
   Intr_Disable();
   Intr_Halt();
}


/*
 * Console_Panic --
 *
 *    Default panic handler. Prints a caller-defined message, and
 *    halts the machine.
 */

void
Console_Panic(const char *fmt, ...)
{
   Console_BeginPanic();
   Console_WriteString("Panic:\n");
   Console_FormatV(&fmt);
   Console_Flush();
   Intr_Disable();
   Intr_Halt();
}

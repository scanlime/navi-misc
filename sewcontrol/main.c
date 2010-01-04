/*
 * main.c -- Main program for the sewing machine speed controller.
 *           See hardware.txt for information about the modified AVR
 *           Butterfly board this runs on.
 *
 * Copyright (c) 2010 Micah Dowty <micah@navi.cx>
 * See end of file for license terms. (BSD style)
 */

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <stdio.h>

#include "LCD_Driver.h"
#include "pedal.h"
#include "sensors.h"
#include "triac.h"

const char FooText[] PROGMEM = "Foobar";
int debug;

void
Beep() {
   int i;

   DDRB |= 1 << 5;
   for (i = 0; i < 100; i++) {
      volatile int j;

      wdt_reset();
      PORTB ^= 1 << 5;
      for (j = 0; j < 100; j++);
   }
}

static void
Clock_Init()
{
   // Run at full speed (8 MHz)
   CLKPR = 1 << CLKPCE;
   CLKPR = 0;
}

int
main() {
   wdt_enable(0);
   Control_Init();
   Clock_Init();
   Triac_Init();
   Pedal_Init();
   Sensors_Init();
   LCD_Init();
   sei();

   Beep();

   while (1) {
      wdt_reset();

      static char buffer[32];

      sprintf(buffer, "%d", debug);
      LCD_PutStr(buffer);
   }
}

/*****************************************************************/

/*
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

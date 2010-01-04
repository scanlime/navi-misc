/*
 * triac.h -- Triac control module for the sewing machine speed controller.
 *
 * Copyright (c) 2010 Micah Dowty <micah@navi.cx>
 * See end of file for license terms. (BSD style)
 */

#ifndef _PEDAL_H
#define _PEDAL_H

#include <avr/io.h>

static inline void
Pedal_Init()
{
   ADMUX = (1 << REFS1) | 1;    // ADC reading pedal position, with AVCC reference
   ADCSRB = 0;                  // Free-running mode
   DIDR0 = (1 << 1);            // Disable digital input from pedal pin

   // Start ADC, 1/128 prescaler
   ADCSRA = (1 << ADEN) | (1 << ADSC) | (1 << ADATE) | 7;
}

static inline short
Pedal_Read()
{
   unsigned char low = ADCL;
   unsigned char high = ADCH;
   return low | (high << 8);
}

#endif // _PEDAL_H

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

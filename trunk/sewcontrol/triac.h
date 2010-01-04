/*
 * triac.h -- Triac control module for the sewing machine speed controller.
 *
 * Copyright (c) 2010 Micah Dowty <micah@navi.cx>
 * See end of file for license terms. (BSD style)
 */

#ifndef _TRIAC_H
#define _TRIAC_H

#include <avr/io.h>
#include <stdint.h>

#define MAINS_HZ         60

#define TRIAC_TICK_HZ    (8000000 / 8)                     // Timer 1, 1/8 prescaler
#define TRIAC_PERIOD     (TRIAC_TICK_HZ / (2 * MAINS_HZ))  // One half-cycle
#define TRIAC_PULSE_LEN  (TRIAC_TICK_HZ / 5000)            // Trigger pulse length

#define TRIAC_PORT    PORTB
#define TRIAC_DDR     DDRB
#define TRIAC_MASK    (1 << 0)

void Triac_Init();
void Triac_Trigger(uint16_t delay);

#endif // _TRIAC_H

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

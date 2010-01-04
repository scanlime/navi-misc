/*
 * triac.c -- Triac control module for the sewing machine speed controller.
 *
 * Copyright (c) 2010 Micah Dowty <micah@navi.cx>
 * See end of file for license terms. (BSD style)
 */

#include <avr/interrupt.h>
#include <avr/io.h>

#include "triac.h"

enum {
   S_TRIGGERED = 0,
   S_PULSING,
};

static uint8_t state;

ISR(TIMER1_OVF_vect, ISR_BLOCK)
{
   if (state == S_PULSING) {
      // End the pulse
      TRIAC_PORT &= ~TRIAC_MASK;
      TIMSK1 = 0;
   } else {
      // Begin the pulse
      TRIAC_PORT |= TRIAC_MASK;
      TCNT1 = -TRIAC_PULSE_LEN;
      state = S_PULSING;
   }
}

void
Triac_Init()
{
   // Initialize port
   TRIAC_PORT &= ~TRIAC_MASK;
   TRIAC_DDR |= TRIAC_MASK;

   // Set up Timer 1 to tick at clkIO/8
   TCCR1A = 0;
   TCCR1B = 1 << CS11;

   // Until we need it, keep the interrupt disabled.
   TIMSK1 = 0;
}

void
Triac_Trigger(uint16_t delay)
{
   /*
    * To safely reprogram the timer, we need to first disable it, then
    * set a long timeout that won't occur before we're finished, set
    * up the interrupt and output port, then reset the timer to its
    * proper value.
    */

   TIMSK1 = 0;            // Disable interrupt
   TCNT1 = 0;             // Reset timer
   TIMSK1 = 1 << TOIE1;   // Enable interrupt

   // If we were already in the middle of a pulse, end it
   TRIAC_PORT &= ~TRIAC_MASK;

   // Keep delay from wrapping around
   if (delay <= 0)
      delay = 1;

   // Asynchronously trigger the triac, in 'delay' ticks.
   state = S_TRIGGERED;
   TCNT1 = -delay;
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

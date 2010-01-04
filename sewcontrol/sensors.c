/*
 * sensors.h -- Sensor input module for the sewing machine speed controller.
 *
 * Copyright (c) 2010 Micah Dowty <micah@navi.cx>
 * See end of file for license terms. (BSD style)
 */

#include <avr/interrupt.h>
#include <avr/io.h>

#include "sensors.h"
#include "triac.h"
#include "pedal.h"

#define PEDAL_MAX 420

static void
Sensor_ZeroCross()
{
   int pedal = Pedal_Read();
   if (pedal > PEDAL_MAX)
      pedal = PEDAL_MAX;

   Triac_Trigger((uint32_t)TRIAC_PERIOD * (uint32_t)pedal / PEDAL_MAX);
}

static void
Sensor_TachPulse()
{

}

static void
Sensor_NeedleUp()
{

}

static void
Sensor_NeedleDown()
{

}

ISR(PCINT0_vect, ISR_BLOCK)
{
   static uint8_t prev;
   uint8_t current = PINE;
   uint8_t diff = prev ^ current;

   /*
    * Zero crossing, negative edge detector
    *
    * The hardware keeps this pretty close to true 50% duty cycle,
    * so we'll use any edge to detect a zero crossing. We don't
    * care whether it's positive or negative.
    */
   if (diff & SENSOR_ZEROCROSS) {
      Sensor_ZeroCross();
   }

   /*
    * Tachometer, positive edge detector
    *
    * The tachometer is a clean square wave, with an arbitrary duty
    * cycle. It doesn't matter which edge we pick, as long as it's
    * consistent.
    */
   if ((diff & SENSOR_TACHOMETER) && (current & SENSOR_TACHOMETER)) {
      Sensor_TachPulse();
   }

   /*
    * Photo-interrupter, positive and negative edge detector
    *
    * The two photo-interrupter edges each signify a different spot in
    * the machine's cycle. A rising edge (sensor blocked) means the
    * needle is up, and has just started moving down. A falling edge
    * (sensor cleared) means the needle is down and is about to return
    * up.
    */
   if (diff & SENSOR_INTERRUPTER) {
      if (current & SENSOR_INTERRUPTER)
         Sensor_NeedleUp();
      else
         Sensor_NeedleDown();
   }

   prev = current;
}

void
Sensors_Init()
{
   /*
    * Set up the pin change interrupt to signal high and low edges on
    * the zero-crossing, photointerrupter, and tachometer sensors.
    */
   PCMSK0 = SENSOR_ALL;
   PCMSK1 = 0;
   EIMSK = 1 << PCIE0;
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

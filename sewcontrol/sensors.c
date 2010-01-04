/*
 * sensors.h -- Sensor input module for the sewing machine speed controller.
 *
 * Copyright (c) 2010 Micah Dowty <micah@navi.cx>
 * See end of file for license terms. (BSD style)
 */

#include <avr/interrupt.h>
#include <avr/io.h>
#include <math.h>

#include "sensors.h"
#include "LCD_Driver.h"
#include "triac.h"
#include "control.h"

static int  tachCounter;
static int  nextTriacPhase;
static char nextTriacEnable;


static void
Sensor_ZeroCross()
{
   /*
    * Before anything else, set up the triac trigger.  This should
    * occur as soon after the zero crossing as possible. This means
    * that the motor speed values are delayed by 1/2 cycle.
    */
   if (nextTriacEnable)
      Triac_Trigger(nextTriacPhase);

   /*
    * Tach measurement, in gear teeth per second.
    *
    * There are two zero crossings per mains cycle, and two tach
    * pulses per gear tooth. So we can just multiply by the mains
    * frequency.
    */
   float tach = tachCounter * MAINS_HZ;
   tachCounter = 0;

   /*
    * Run the control loop, and convert its output into a triac phase
    * value.  The control loop's output is a "power" value, normalized
    * to the range [0, 1]. The control loop works best when this value
    * controls speed nearly linearly.
    *
    * We can't account for the mechanical affects here (that's what
    * the control loop tries to correct for) but we can correct for
    * the triac controller's nonlinearity.
    *
    * The triac controls power by switching on the load over only part
    * of the sine wave cycle. If we neglect all of the effects of
    * switching an inductive load (ugh) this can be approximated by
    * integrating over the portion of the sine wave in which the load
    * is receiving power.
    *
    * If our power waveform is sin(t), the integral of power is
    * 1-cos(t), and the inverse of this is acos(1-x). Scaling for
    * power and time both in the range [0, 1], this is:
    *
    *   time = acos(1 - 2*power) / pi
    */

   float power = Control_Loop(tach);

   power = acos(1 - 2*power) / M_PI;

   const float minPower = 0.1f;
   const float maxPower = 0.95f;

   if (power > maxPower)
      power = maxPower;

   if ((nextTriacEnable = power >= minPower)) {
      nextTriacPhase = TRIAC_PERIOD - power * TRIAC_PERIOD;
   }
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
    * Zero crossing
    *
    * The hardware keeps this pretty close to true 50% duty cycle,
    * so we'll use any edge to detect a zero crossing. We don't
    * care whether it's positive or negative.
    */
   if (diff & SENSOR_ZEROCROSS) {
      Sensor_ZeroCross();
   }

   /*
    * Tachometer
    *
    * The tachometer is a clean square wave, with an arbitrary duty
    * cycle. To get the cleanest signal, we'd pick a single edge
    * (either positive or negative) and stick with it. Using either
    * edge introduces some noise- but the extra temporal resolution is
    * handy enough that I don't care. Detect either edge.
    */
   if (diff & SENSOR_TACHOMETER) {
      tachCounter++;
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

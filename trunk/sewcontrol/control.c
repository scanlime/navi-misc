/*
 * control.h - Main speed control loop
 *
 * Copyright (c) 2010 Micah Dowty <micah@navi.cx>
 * See end of file for license terms. (BSD style)
 */

#include "joystick.h"

extern int debug;
static float power;

void
Control_Init()
{
   power = 0;
}

static float
filter(float input)
{
   /*
    * Single pole butterworth low-pass filter, sampling at 120 Hz,
    * with a corner frequency of 10 Hz. Designed with the applet at:
    *   http://www-users.cs.york.ac.uk/~fisher/mkfilter/trad.html
    */

   static float xv0, xv1, yv0, yv1;

   xv0 = xv1;
   xv1 = input / 4.732050808;
   yv0 = yv1;
   yv1 = xv0 + xv1 + 0.5773502692 * yv0;

   return yv1;
}

float
Control_Loop(float curSpeed)
{
   // Target speed
   const float target = 120.0f;

   static int igain = 1;
   float gain = igain * 1e-6;
   debug = igain;

   uint8_t joy = Joystick_Poll();
   if (JOY_LEFT & joy)
      igain++;
   if (JOY_RIGHT & joy)
      igain--;

   float error = target - curSpeed;

   if (target <= 0.0f) {
      power = 0.0f;
   } else {
      power += error * gain;
   }

   return power;
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

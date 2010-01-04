/*
 * joystick.h - Simple joystick header for the AVR Butterfly
 *
 * Copyright (c) 2010 Micah Dowty <micah@navi.cx>
 * See end of file for license terms. (BSD style)
 */

#ifndef _JOYSTICK_H
#define _JOYSTICK_H

#include <avr/io.h>

#define JOY_LEFT                  (1 << 2)
#define JOY_RIGHT                 (1 << 3)
#define JOY_UP                    (1 << 6)
#define JOY_DOWN                  (1 << 7)
#define JOY_PRESS                 (1 << 4)

#define JOY_BMASK                 ((1 << 4) | (1 << 6) | (1 << 7))
#define JOY_EMASK                 ((1 << 2) | (1 << 3))

static inline uint8_t
Joystick_Poll()
{
   PORTB |= JOY_BMASK;
   PORTE |= JOY_EMASK;
   return (~PINB & JOY_BMASK) | (~PINE & JOY_EMASK);
}

#endif // _JOYSTICK_H

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

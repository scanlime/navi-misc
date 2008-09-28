/* -*- Mode: C; c-basic-offset: 4 -*-
 *
 * pll.c --
 *
 *   This file implements a Phase Locked Loop for recovering the
 *   current phase angle of the power mains.
 *
 * Copyright (c) 2008 Micah Dowty <micah@navi.cx>
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

#include <avr/io.h>
#include <stdio.h>

#include "tedrx.h"

/*
 * Currently, we use a sampling rate of 9600 baud (our serial bit
 * rate) and we use 16-bit fixed point math for the PLL counter.
 *
 * In a single nominal mains cycle, we have 9600 / 60 = 160
 * interrupts. At each interrupt, we add 'frq' to 'phase'. The
 * value of 'frq' is adjusted to control the frequency of the
 * PLL, and to maintain phase lock. When the PLL is locked,
 * 'frq' will reflect the current mains frequency.
 *
 * The PLL oscillator has a resolution of 9600 / 0x10000 = 0.1465 Hz.
 */

#define PLL_HZ        OUTPUT_BAUD      /* We're invoked by the serial port's timer interrupt. */
#define MAINS_HZ      60               /* Nominal power line frequency in the US */
#define PLL_FRQ       (0x10000 * MAINS_HZ / PLL_HZ)

#define PLL_GAIN      6                    /* Control loop gain */
#define PHASE_SHIFT   (-0x10000 * 4 / 10)  /* Compensation for PLL phase shift */

volatile static uint16_t pll_phase;

/*
 * pll_update --
 *
 *    Update the PLL. This increments the PLL oscillator, and it
 *    implements an edge-controlled phase detector.
 *
 *    (The ECPD is useful because we don't expect the input signal to
 *    have a 50% duty cycle. We would like to pick a single edge
 *    transition, like high-to-low, and take only that edge into
 *    account.)
 *
 *    References:
 *    "Digital Phase-Locked Loop Design Using SN54/74LS297", Donald G. Troha
 *    http://www3.vdu.lt/if00/LabGrupe/dokai/daugiau%20apie%20SN74.pdf
 */

void
pll_update(void)
{
    static uint8_t prev_mains_sample;
    static uint8_t phase_detector;
    uint8_t mains_sample = (PINB >> MAINS_PIN) & 1;
    uint16_t next_phase = pll_phase + PLL_FRQ;
    
    /*
     * Detect falling edges on the mains pin.
     */
    if (prev_mains_sample && !mains_sample) {
	phase_detector = 1;
    }
    prev_mains_sample = mains_sample;

    /*
     * Detect falling edges (rollover) on our phase counter.
     */
    if (next_phase < pll_phase) {
	phase_detector = 0;
    }
    pll_phase = next_phase;

    /*
     * Apply phase correction.
     */
    if (phase_detector) {
	pll_phase += PLL_GAIN;
    } else {
	pll_phase -= PLL_GAIN;
    }
    
#ifdef DEBUG_PLL
    if ((pll_phase + PHASE_SHIFT) & 0x8000) {
	PORTB |= _BV(DEBUG_PIN);
    } else {
	PORTB &= ~_BV(DEBUG_PIN);
    }
#endif

}

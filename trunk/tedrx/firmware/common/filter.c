/* -*- Mode: C; c-basic-offset: 4 -*-
 *
 * filter.c --
 *
 *   A digital filter and demodulator. Our input is the amplified
 *   output of a 138 kHz bandpass filter, which separates the
 *   modulated signal from most of the background noise (and, of
 *   course, the mains frequency itself).
 *
 *   This module's job is to phovide a very narrow digital bandpass
 *   filter to select the 138 kHz pulses. The result is then low-pass
 *   filtered into a clean baseband signal. We provide a high-level
 *   entry point which receives a packet, using a serial protocol
 *   which is similar to RS-232 at 1200 baud 8-N-2, but with a
 *   different idle state.
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
#include <avr/interrupt.h>
#include <string.h>

#include <filter.h>
#include <tedrx.h>

/* Experimentally determined protocol constants */
#define ASK_FREQ      138000                       /* The TED uses ASK modulation at 138 kHz */
#define BIT_RATE      1200                         /* Data is 1200 baud serial */

#define RDIV(x,y)     (((x) + ((y)/2)) / (y))      /* Integer divide, with rounding. */

#define TMR0_FREQ     (ASK_FREQ * 2)               /* Sample at 2x the ASK frequency */
#define TMR0_DIV      RDIV(F_CPU, TMR0_FREQ)
#define TMR0_ACTUAL   RDIV(F_CPU, TMR0_DIV)        /* Actual TMR0 rate, including rounding error */
#define BIT_DIV       RDIV(TMR0_ACTUAL, BIT_RATE)  /* Number of TMR0 interrupts per bit */

#define PATMATCH_DIV  8                            /* How many TMR0 cycles between pattern matching opportunities? */
#define LPF_SIZE      8                           /* Size of low-pass filter buffer. Must be a power of two. */
#define LPF_THRESHOLD (LPF_SIZE * 3)               /* Low pass filter threshold */
#define LPF_HYST      4                            /* Amount of hysteresis applied to LPF_THREHSOLD */

static volatile uint8_t  baseband;                 /* The latest demodulated bit */
static volatile uint8_t  bit_timer;                /* TMR0-driven counter for bit timing */


/*
 * ask_demodulate --
 *
 *    Demodulate the Amplitude Shift Keyed (ASK) data that our
 *    hardware has filtered and amplified. We take a pattern
 *    matching approach: Sample at twic the modulation frequency,
 *    and look for alternating ones and zeroes, then low-pass
 *    filter the result.
 *
 *    This updates the global 'baseband' with the latest decoded bit.
 *
 *    Call this function any time the USI counter overflows. We don't
 *    use an interrupt, since the demodulator must be lower priority
 *    than Timer 0.
 */

static void
ask_demodulate(void)
{
    /* Clear the overflow flag, seed the bit counter. */
    USISR = _BV(USIOIF) | (16 - PATMATCH_DIV);

    /*
     * The USI buffer register contains a history of the last 8
     * samples from the ASK input pin, sampled at TMR0_FREQ.
     */
    uint8_t shift_reg = USIBR;

    /*
     * Pattern matching:
     *
     * We sample the ASK signal at 2x the modulation frequency. Ideally,
     * we'd see a repeating pattern in our shift register at all times:
     * either 01010101 or 10101010. This lookup table is used to make a
     * fuzzy comparison against either of these templates. For each of
     * the 256 byte values, this table measures the number of bits of
     * difference between the value and either of the two templates.
     */

    static const uint8_t pattern_lut[] = {
        4, 3, 3, 4, 3, 2, 4, 3, 3, 4, 2, 3, 4, 3, 3, 4,
        3, 2, 4, 3, 2, 1, 3, 2, 4, 3, 3, 4, 3, 2, 4, 3,
        3, 4, 2, 3, 4, 3, 3, 4, 2, 3, 1, 2, 3, 4, 2, 3,
        4, 3, 3, 4, 3, 2, 4, 3, 3, 4, 2, 3, 4, 3, 3, 4,
        3, 2, 4, 3, 2, 1, 3, 2, 4, 3, 3, 4, 3, 2, 4, 3,
        2, 1, 3, 2, 1, 0, 2, 1, 3, 2, 4, 3, 2, 1, 3, 2,
        4, 3, 3, 4, 3, 2, 4, 3, 3, 4, 2, 3, 4, 3, 3, 4,
        3, 2, 4, 3, 2, 1, 3, 2, 4, 3, 3, 4, 3, 2, 4, 3,
        3, 4, 2, 3, 4, 3, 3, 4, 2, 3, 1, 2, 3, 4, 2, 3,
        4, 3, 3, 4, 3, 2, 4, 3, 3, 4, 2, 3, 4, 3, 3, 4,
        2, 3, 1, 2, 3, 4, 2, 3, 1, 2, 0, 1, 2, 3, 1, 2,
        3, 4, 2, 3, 4, 3, 3, 4, 2, 3, 1, 2, 3, 4, 2, 3,
        4, 3, 3, 4, 3, 2, 4, 3, 3, 4, 2, 3, 4, 3, 3, 4,
        3, 2, 4, 3, 2, 1, 3, 2, 4, 3, 3, 4, 3, 2, 4, 3,
        3, 4, 2, 3, 4, 3, 3, 4, 2, 3, 1, 2, 3, 4, 2, 3,
        4, 3, 3, 4, 3, 2, 4, 3, 3, 4, 2, 3, 4, 3, 3, 4
    };

    static uint8_t lpf_buf[LPF_SIZE] = { 0 };
    static uint8_t lpf_index = 0;
    static uint8_t lpf_total = 0;

    lpf_total -= lpf_buf[lpf_index];
    lpf_total += (lpf_buf[lpf_index] = pattern_lut[shift_reg]);
    lpf_index = (lpf_index + 1) & (LPF_SIZE - 1);

    /*
     * We apply some digital hysteresis to this value, to avoid
     * noisy edge transitions.
     */
        
    baseband = lpf_total <= (baseband ? LPF_THRESHOLD + LPF_HYST :
                             LPF_THRESHOLD - LPF_HYST);

#if defined(DEBUG_PIN) && defined(DEBUG_BASEBAND)
    if (baseband) {
        PORTB |= _BV(DEBUG_PIN);
    } else {
        PORTB &= ~_BV(DEBUG_PIN);
    }
#endif
}


/*
 * ask_demodulate_pending --
 *
 *    Do we need to call ask_demodulate()?
 */

static inline uint8_t
ask_demodulate_pending(void)
{
    return USISR & _BV(USIOIF);
}


/*
 * ask_demodulate_poll --
 *
 *    Call ask_demodulate() only if we need to. (Only if the USI counter rolled over.)
 */

static inline void
ask_demodulate_poll(void)
{
    if (ask_demodulate_pending()) {
        ask_demodulate();
    }
}


/*
 * Timer 0 overflow vector --
 *
 *    This runs at TMR0_FREQ, naturally. Our job is just to
 *    seed the timer, in order to set its overflow frequency.
 */

ISR(TIM0_OVF_vect)
{
    /* The read-modify-write takes three cycles. */
    TCNT0 += 256 - TMR0_DIV + 3;

    bit_timer++;

#if defined(DEBUG_PIN) && defined(DEBUG_TMR0)
    PORTB ^= _BV(DEBUG_PIN);
#endif
}


/*
 * rx_bit --
 *
 *    Receive one bit, with majority-detect.
 */

static uint8_t
rx_bit(void)
{
    uint16_t high_cnt = 0;
    uint16_t low_cnt = 0;
    
#if defined(DEBUG_PIN) && defined(DEBUG_RX_BIT_TIME)
    PORTB ^= _BV(DEBUG_PIN);
    PORTB ^= _BV(DEBUG_PIN);
#endif

    do {
	ask_demodulate_poll();

	/*
	 * Majority-detection window: Between 1/2 and 2/3 of the bit
	 * width. We bias toward the end of the bit, since the tail
	 * end of each "1" tends to ring a bit.
	 */
	if (bit_timer >= BIT_DIV / 2 && bit_timer <= BIT_DIV * 2 / 3) {
	    if (baseband) {
		high_cnt++;
	    } else {
		low_cnt++;
	    }
	}

    } while (bit_timer < BIT_DIV);

    cli();
    bit_timer -= BIT_DIV;
    sei();

#if defined(DEBUG_PIN) && defined(DEBUG_RX_BIT_TIME)
    PORTB ^= _BV(DEBUG_PIN);
    PORTB ^= _BV(DEBUG_PIN);
    PORTB ^= _BV(DEBUG_PIN);
    PORTB ^= _BV(DEBUG_PIN);
    if (high_cnt > low_cnt) {
	PORTB |= _BV(DEBUG_PIN);
    } else {
	PORTB &= ~_BV(DEBUG_PIN);
    }
#endif

    return high_cnt > low_cnt;
}


/*
 * filter_rx --
 *
 *    Try to receive a packet. Returns the number of bytes received.
 */

uint8_t
filter_rx(uint8_t *buffer, uint8_t length)
{
    uint8_t result = 0;
    uint8_t i;

    /*
     * Prime the ASK demodulation filters, by running
     * through a few dummy cycles.
     */
    for (i = LPF_SIZE; i; i--) {
	while (!ask_demodulate_pending());
	ask_demodulate();
    }

    /* Wait for first start bit. */	
    do {
	do {
	    ask_demodulate_poll();
	} while (!baseband);
	
	bit_timer = 0;
    } while (!rx_bit());

    while (1) {
	uint8_t bit;
	uint8_t byte = 0;

	/* Receive 8 data bits, LSB first. */

#if defined(DEBUG_PIN) && defined(DEBUG_RX_BYTE_FRAMING)
	PORTB |= _BV(DEBUG_PIN);
#endif

	for (bit = 8; bit; bit--) {
	    byte >>= 1;
	    if (rx_bit()) {
		byte |= 0x80;
	    }
	}

#if defined(DEBUG_PIN) && defined(DEBUG_RX_BYTE_FRAMING)
	PORTB ^= _BV(DEBUG_PIN);
	PORTB ^= _BV(DEBUG_PIN);
#endif

	/*
	 * Two stop bits.
	 *
	 * Validate the first one, but use the second one as an error
	 * margin in synchronizing with the next byte's start
	 * bit. This makes us a lot more tolerant to timing errors in
	 * detecting the edge of the first start bit.
	 */

	if (rx_bit()) {
	    goto error;
	}

	/* Store this byte */

	*buffer = byte;
	buffer++;
	length--;
	result++;

	if (!length) {
	    break;
	}

	/* Wait for up to two bit widths, for the next start bit. */
	bit = 2;

	do {
	    ask_demodulate_poll();
	    if (bit_timer > BIT_DIV) {
		bit--;
		bit_timer = 0;
		if (bit == 0) {
		    goto error;
		}
	    }
	} while (!baseband);

#if defined(DEBUG_PIN) && defined(DEBUG_RX_BYTE_FRAMING)
	PORTB &= ~_BV(DEBUG_PIN);
#endif

	bit_timer = 0;
	if (!rx_bit()) {
	    goto error;
	}
    }

 error:
    return result;
}


/*
 * filter_init --
 *
 *    Initialize the digital filter for receiving TED transmissions.
 */

void
filter_init(void)
{
#ifdef DEBUG_PIN
    DDRB |= _BV(DEBUG_PIN);
#endif

    /*
     * Program timer 0. Run it off of the system clock with no prescaler.
     * Enable the overflow interrupt.
     */
    TCCR0B = _BV(CS00);
    TIMSK |= _BV(TOIE0);
        
    /*
     * Set up the USI to sample the ASK input at TMR0_FREQ,
     * a multiple of the ASK modulation frequency.
     *
     * Don't enable the USI interrupt. We can't use it because
     * it's higher priority than the Timer 0 interrupt. We need
     * Timer 0 to be able to interrupt the USI code.
     */
    USICR = _BV(USIWM0) | _BV(USICS0);
}

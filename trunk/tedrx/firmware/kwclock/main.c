/* -*- Mode: C; c-basic-offset: 4 -*-
 *
 * The Kilowatt Clock: This firmware receives packets over the
 * powerline from The Energy Detective, and uses the current
 * whole-house power usage to tick a standard Quartz clock movement at
 * every kilowatt-second.
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
#include <util/delay.h>

#include <tedrx.h>
#include <filter.h>
#include <ted-raw.h>

FUSES = {
    .low = 0xE1,       /* 16 MHz PLL */
    .high = 0xDF,      /* Defaults */
    .extended = 0xFF,  /* Defaults */
};

/*
 * Watchdog timer duration, in TMR1 cycles.
 * If we go this long without receiving a TED packet, stop the clock.
 */
#define WATCHDOG_TICKS  (TMR1_HZ * 30)

uint32_t current_power;
uint32_t watchdog;


int main(void)
{
    uint8_t buf[16];

    DDRB |= _BV(LED_PIN) | _BV(CLOCK_PIN_1) | _BV(CLOCK_PIN_2);
        
    /*
     * Set up Timer 1 to drive our clock interrupt, at TMR1_HZ.
     *
     * When the timer reaches OCR1C, it will be reset. When it
     * reaches OCR1B, we fire an interrupt.
     */
    TCCR1 = _BV(CTC1) | _BV(CS13) | _BV(CS11) |_BV(CS10);
    OCR1B = 1;
    OCR1C = F_CPU / 1024 / TMR1_HZ;
    TIMSK |= _BV(OCIE1B);

    filter_init();
    sei();

    while (1) {
        uint8_t size;
	uint32_t power;
	TEDPacket *packet;
	
        size = filter_rx(buf, sizeof buf);
	packet = ted_packet_validate(buf, size);

	if (!packet) {
	    /* Bad packet */
	    continue;
	}

	/* Atomically store the new power value. */

	power = ted_packet_watts(packet);
	cli();
	current_power = power;
	watchdog = WATCHDOG_TICKS;
	sei();

	/* We got a good packet. Flash the LED. */

	PORTB |= _BV(LED_PIN);
	_delay_ms(50);
	PORTB &= ~_BV(LED_PIN);
    }
}


/*
 * Timer 1 compare match vector --
 *
 *    This fires at TMR1_HZ, and runs our clock accumulator.
 *
 *    We intentionally use COMPB instead of COMPA, because this
 *    interrupt must be lower priority than the Timer 0 overflow. This
 *    interrupt must not block the Timer 0 interrupt, or our ASK
 *    demodulation timebase will be inaccurate.
 */

ISR(TIM1_COMPB_vect, ISR_NOBLOCK)
{
    static uint32_t clock_acc;
    const uint32_t acc_rollover = TMR1_HZ * 1000L;
    uint8_t need_tick;
    static uint8_t state;

    /*
     * Update the watchdog timer.
     */
    if (!watchdog) {
	return;
    }
    watchdog--;

    /*
     * Clock accumulator. Every TMR1_HZ, we accumulate the current_power
     * (in watts). Every time we've stored one kilowatt-second (TMR1_HZ *
     * 1000), we tick the clock mechanism.
     */
    
    clock_acc += current_power;
    need_tick = clock_acc > acc_rollover;
    if (need_tick) {
	clock_acc -= acc_rollover;
    }

    /*
     * Drive the clock ticks themselves using a state machine.
     *
     * The clock motor is connected to two I/O pins, and we drive
     * it somewhat like a stepper motor. Every other tick toggles
     * the pulse polarity. Ticks are short pulses of about 1/30th
     * of a second, or three TMR1 interrupts.
     */

    switch (state & 7) {

    case 0:                        /* Waiting for tick (+) */
	PORTB &= ~_BV(CLOCK_PIN_1);
	PORTB &= ~_BV(CLOCK_PIN_2);
	if (need_tick) {
	    state++;
	}
	break;

    case 1:                        /* Emitting tick (+) */
    case 2:
    case 3:
	PORTB |= _BV(CLOCK_PIN_1);
	PORTB &= ~_BV(CLOCK_PIN_2);
	state++;
	break;

    case 4:                        /* Waiting for tick (-) */
	PORTB |= _BV(CLOCK_PIN_1);
	PORTB |= _BV(CLOCK_PIN_2);
	if (need_tick) {
	    state = 5;
	}
	break;

    case 5:                        /* Emitting tick (-) */
    case 6:
    case 7:
	PORTB &= ~_BV(CLOCK_PIN_1);
	PORTB |= _BV(CLOCK_PIN_2);
	state++;
	break;

    }
}

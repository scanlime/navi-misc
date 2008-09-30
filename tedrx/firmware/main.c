/* -*- Mode: C; c-basic-offset: 4 -*-
 *
 * Work-in-progress: Open source receiver for The Energy Detective
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
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>

#include "tedrx.h"

FUSES = {
    .low = 0xE1,       /* 16 MHz PLL */
    .high = 0xDF,      /* Defaults */
    .extended = 0xFF,  /* Defaults */
};

int main(void)
{
    uint8_t buf[32];

    filter_init();
    serial_init();
    sei();

    while (1) {
        uint8_t i, size;

        size = filter_rx(buf, sizeof buf);

	/*
	 * Packet format notes:
	 *
	 *  11 bytes
	 * 
	 *  0.     Header (0x55)
	 *  1.     House code
	 *  2.     Packet counter
	 *  3-5.   Raw power reading (complement)
	 *  6-8.   Raw voltage?
	 *  9-10.  16-bit checksum?
	 */

        printf("[%02d]", size);
        for (i = 0; i < size; i++) {
            printf(" %02x", buf[i]);
        }
        printf("\r\n");
    }
}

/* -*- Mode: C; c-basic-offset: 4 -*-
 *
 * Main program for an open source receiver unit for The Energy
 * Detective.
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

uint8_t checksum(uint8_t *buf, uint8_t len)
{
    uint8_t sum = 0;

    while (len) {
	sum += *buf;
	buf++;
	len--;
    }
}

int main(void)
{
    uint8_t buf[16];

    filter_init();
    serial_init();
    sei();

    while (1) {
        uint8_t i, size;
	uint32_t power, voltage;

        size = filter_rx(buf, sizeof buf);

	/*
	 * Packet format notes:
	 *
	 *  11 bytes
	 * 
	 *  0.     Header (0x55)
	 *  1.     House code
	 *  2.     Packet counter
	 *  3-5.   Raw power reading
	 *  6-8.   Raw voltage reading
	 *  9.     Unknown
	 *  10.    Checksum
	 *
	 * The raw voltage and power readings can be converted to
	 * kilowatts and volts using these experimentally determined
	 * formulae:
	 *
	 *   kW = (raw_power - 16777177) / -62048
	 *   V  = (raw_voltage - 170350) / 55822
	 *
	 * Byte 9 seems to always hover around 250 in my
	 * measurements. This could be a flag byte, some kind of
	 * parity signal, or maybe even a measurement of the AC line
	 * frequency.
	 *
	 * Byte 10 is a simple checksum. Add all the bytes in the packet,
	 * modulo 256, and this byte ensures that the result is zero.
	 */

	if (size != 11 || buf[0] != 0x55 || checksum(buf, size) != 0) {
	    /* Bad packet */
	    continue;
	}

	power = (uint32_t)buf[3] | (((uint32_t)buf[4]) << 8) | (((uint32_t)buf[5]) << 16);
	voltage = (uint32_t)buf[6] | (((uint32_t)buf[7]) << 8) | (((uint32_t)buf[8]) << 16);

	power = -(power - 16777177L);
	voltage -= 170350L;

	/*
	 * Print a representation of this packet which is designed
	 * to be both human- and machine-readable.
	 */

	printf("HC=%03u KW=%03u.%03u V=%03u.%03u CNT=%03u\r\n",
	       buf[1],
	       (int)(power / 62048L),
	       (int)((power * 100L / 6204L) % 1000),
	       (int)(voltage / 55822L),
	       (int)((voltage * 100L / 5582L) % 1000),
	       buf[2]);
    }
}

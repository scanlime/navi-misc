/* -*- Mode: C; c-basic-offset: 4 -*-
 *
 * Main program for an open source receiver unit for The Energy Detective.
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

#include <tedrx.h>
#include <ted-raw.h>

FUSES = {
    .low = 0xE1,       /* 16 MHz PLL */
    .high = 0xDF,      /* Defaults */
    .extended = 0xFF,  /* Defaults */
};

int main(void)
{
    uint8_t buf[16];

    filter_init();
    serial_init();
    sei();

    while (1) {
        uint8_t size;
	uint32_t power, voltage;
	TEDPacket *packet;
	
        size = filter_rx(buf, sizeof buf);
	packet = ted_packet_validate(buf, size);

	if (!packet) {
	    /* Bad packet */
	    continue;
	}

	power = ted_packet_watts(packet);
	voltage = ted_packet_millivolts(packet);

	/*
	 * Print a representation of this packet which is designed
	 * to be both human- and machine-readable.
	 */

	printf("HC=%03u KW=%03u.%03u V=%03u.%03u CNT=%03u\r\n",
	       packet->houseCode,
	       (int)(power / 1000),
	       (int)(power % 1000),
	       (int)(voltage / 1000),
	       (int)(voltage % 1000),
	       packet->packetCount);
    }
}

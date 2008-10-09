/* -*- Mode: C; c-basic-offset: 4 -*-
 *
 * ted-raw.h --
 *
 *   Definitions for TED's raw packet format, as transmitted along
 *   the power lines at 2400 baud. This information is based on
 *   original black-box reverse engineering work.
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

#ifndef _TED_RAW_H
#define _TED_RAW_H

#ifndef NULL
#define NULL ((void*)0)
#endif

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
 * Byte 10 is a simple checksum. Add all the bytes in the packet
 * except byte 9, modulo 256, and this byte ensures that the result is
 * zero.  Equivalently, we can state that the sum of all bytes in the
 * packet equals byte 9.
 */

typedef struct TEDPacket {
  uint8_t header;
  uint8_t houseCode;
  uint8_t packetCount;
  uint8_t power[3];
  uint8_t voltage[3];
  uint8_t unknown;
  uint8_t checksum;
} TEDPacket;

#define TED_HEADER  0x55

static uint8_t
ted_checksum(uint8_t *buf, uint8_t len)
{
    uint8_t sum = 0;

    while (len) {
	sum += *buf;
	buf++;
	len--;
    }

    return sum;
}

static inline TEDPacket *
ted_packet_validate(uint8_t *buf, uint8_t len)
{
    TEDPacket *packet = (TEDPacket *) buf;

    if (len != sizeof *packet) {
	return NULL;
    }

    if (packet->header != TED_HEADER) {
	return NULL;
    }

    if (ted_checksum(buf, len) != packet->unknown) {
	return NULL;
    }

    return packet;
}

static inline uint32_t
ted_packet_raw_power(TEDPacket *packet)
{
    return (uint32_t)packet->power[0] |
	   (((uint32_t)packet->power[1]) << 8) |
	   (((uint32_t)packet->power[2]) << 16);
}

static inline uint32_t
ted_packet_raw_voltage(TEDPacket *packet)
{
    return (uint32_t)packet->voltage[0] |
	   (((uint32_t)packet->voltage[1]) << 8) |
	   (((uint32_t)packet->voltage[2]) << 16);
}

static inline uint32_t
ted_packet_watts(TEDPacket *packet)
{
    return -(ted_packet_raw_power(packet) - 16777177L) * 100L / 6205L;
}

static inline uint32_t
ted_packet_millivolts(TEDPacket *packet)
{
    return (ted_packet_raw_voltage(packet) - 170350L) * 100L / 5582L;
}

#endif /* _TED_RAW_H */

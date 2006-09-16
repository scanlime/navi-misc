/*
 * receiver.c - Packet receiver for the therm base station. This uses
 *              the built-in UART to receive individual bits as bytes,
 *              then matches those bytes to the ideal patterns for a "1"
 *              and a "0" pulse. These matched bits are buffered, and
 *              any content between flag sequences is checked to see if
 *              it might be a valid packet.
 *
 * Wireless therm system
 * Copyright (C) 2004-2005 Micah Dowty
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#include <uart_driver.h>
#include "therm-rx.h"

static int total_bits;
static int total_noise;
static unsigned char consecutive_ones;
static unsigned char* buffer;
static int buffer_size;
static int buffered_bits;

#define HEADER_SIZE 6

void receiver_init()
{
  uart_init();
  uart_set_speed(UART_BAUD_TO_DIVISOR(9600.0));

  total_bits = 0;
  total_noise = 0;
  buffered_bits = 0;
}


/******************************************************************************/
/************************************************************** Bit buffer ****/
/******************************************************************************/

void receiver_set_buffer(unsigned char *buffer_, int buffer_size_)
{
  buffer = buffer_;
  buffer_size = buffer_size_;
  buffered_bits = 0;
}

static void buffer_append_bit(unsigned char b)
{
  /* Append a bit to our current buffer */

  int offset = HEADER_SIZE + (buffered_bits >> 3);
  int mask = 1 << (buffered_bits & 7);
  if (offset >= buffer_size)
    return;

  if (b)
    buffer[offset] |= mask;
  else
    buffer[offset] &= ~mask;

  buffered_bits++;
}

static unsigned char buffer_get_bit(int bit_offset)
{
  /* Extract a single bit from the buffer */
  int offset = HEADER_SIZE + (bit_offset >> 3);
  int mask = 1 << (bit_offset & 7);
  if (bit_offset >= buffered_bits)
    return 0;
  if (buffer[offset] & mask)
    return 1;
  else
    return 0;
}

static unsigned int buffer_get_int(int bit_offset, int width)
{
  /* Extract an arbitrary-width LSB-first integer */
  unsigned int i = 0;
  unsigned int mask = 1;

  while (width > 0) {
    if (buffer_get_bit(bit_offset))
      i |= mask;

    mask <<= 1;
    width--;
    bit_offset++;
  }
  return i;
}

static unsigned int buffer_get_crc16(int bit_offset, int width)
{
  /* Generate an arbitrary-width CRC-16 */
  unsigned int crc = 0;

  while (width > 0) {
    if (crc & 0x8000) {
      crc = (crc<<1) | buffer_get_bit(bit_offset);
      crc ^= 0x8005;
    }
    else {
      crc = (crc<<1) | buffer_get_bit(bit_offset);
    }
    width--;
    bit_offset++;
  }

  /* Augment the message */
  width = 16;
  while (width > 0) {
    if (crc & 0x8000) {
      crc <<= 1;
      crc ^= 0x8005;
    }
    else {
      crc <<= 1;
    }
    width--;
  }

  return crc;
}


/******************************************************************************/
/********************************************************** Packet Framing ****/
/******************************************************************************/

static int receiver_validate_frame()
{
  /* Look for a valid packet in the current receive buffer. Return the length
   * of the valid buffer data if so, zero if not.
   */

  /* Is it at least long enough for the flag and CRC? */
  if (buffered_bits < 16 + 6)
    return 0;

  /* Chop off the flag */
  buffered_bits -= 6;

  /* Does the CRC match? */
  if (buffer_get_crc16(0, buffered_bits - 16) !=
      buffer_get_int(buffered_bits - 16, 16))
    return 0;

  /* Chop off the CRC */
  buffered_bits -= 16;

  /* Fill in the header */
  ((unsigned short*)buffer)[0] = buffered_bits;
  ((unsigned short*)buffer)[1] = total_noise;
  ((unsigned short*)buffer)[2] = total_bits;

  /* Round up to the nearest byte, add the header length */
  return HEADER_SIZE + ((buffered_bits+7) >> 3);
}

static int receiver_push_bit(unsigned char b, unsigned char noise)
{
  /* We received a raw bit with noise amount- figure out what to do with it */
  int retval = 0;

  total_bits++;
  total_noise += noise;

  if (b) {
    consecutive_ones++;
    buffer_append_bit(b);
  }
  else {
    if (consecutive_ones == 3) {
      /* This is a zero inserted for bit stuffing, ignore it */
    }
    else if (consecutive_ones == 4) {
      /* This is the end of a flag. See if we have a valid packet, then reset everything */
      buffer_append_bit(b);
      retval = receiver_validate_frame();
      total_bits = 0;
      total_noise = 0;
      buffered_bits = 0;
    }
    else {
      buffer_append_bit(b);
    }
    consecutive_ones = 0;
  }

  return retval;
}


/******************************************************************************/
/*********************************************************** Bit Detection ****/
/******************************************************************************/

/* Lookup table to count the number of bits set in a byte */
const static unsigned char count_bits[] = {
  0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
  1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
  1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
  1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
  3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
  4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
};

int receiver_poll()
{
  int byte = uart_nb_read();
  unsigned char noise0, noise1;
  if (byte < 0)
    return 0;

  /* Compare this against the expected pattern for a 1 and for a 0 */
  noise1 = count_bits[byte ^ 0xC0];
  noise0 = count_bits[byte ^ 0xFC];

  if (noise1 < noise0)
    return receiver_push_bit(1, noise1);
  else
    return receiver_push_bit(0, noise0);
}

/* The End */

/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * var-int.h - Inlines for encoding and decoding integers into a
 *             variable length format reminicent of UTF-8 or EBML.
 *             In fact, this is a subset of EBML's size encoding,
 *             limited to values expressable in a 32-bit unsigned long.
 *
 *             For more information on this format, see:
 *             http://ebml.sourceforge.net/specs/
 *
 * Fyre - rendering and interactive exploration of chaotic functions
 * Copyright (C) 2004-2006 David Trowbridge and Micah Dowty
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

#ifndef __VAR_INT_H__
#define __VAR_INT_H__

/* The farthest past 'p' that our read/write functions will try to access */
#define VAR_INT_MAX_SIZE 5

/* Writes an integer at 'p', returns the number of bytes written */
static inline int var_int_write(unsigned char *p, unsigned int i)
{
    if (i < (1<<7)) {
	*p = 0x80 | i;
	return 1;
    }
    else if (i < (1<<14)) {
	*(p++) = 0x40 | (i >> 8);
	*p = i & 0xFF;
	return 2;
    }
    else if (i < (1<<21)) {
	*(p++) = 0x20 | (i >> 16);
	*(p++) = 0xFF & (i >> 8);
	*p = i & 0xFF;
	return 3;
    }
    else if (i < (1<<28)) {
	*(p++) = 0x10 | (i >> 24);
	*(p++) = 0xFF & (i >> 16);
	*(p++) = 0xFF & (i >> 8);
	*p = i & 0xFF;
	return 4;
    }
    else {
	/* Assume the value fits in 32 bits */
	*(p++) = 0x08;
	*(p++) = 0xFF & (i >> 24);
	*(p++) = 0xFF & (i >> 16);
	*(p++) = 0xFF & (i >> 8);
	*p = i & 0xFF;
	return 5;
    }
}

/* Reads an integer at 'p' into i, returns the number of bytes read */
static inline int var_int_read(const unsigned char *p, unsigned int *i)
{
    if (0x80 & *p) {
	*i = p[0] & 0x7F;
	return 1;
    }
    else if (0x40 & *p) {
	*i = ((p[0] & 0x3F) << 8) | p[1];
	return 2;
    }
    else if (0x20 & *p) {
	*i = ((p[0] & 0x1F) << 16) | (p[1] << 8) | p[2];
	return 3;
    }
    else if (0x10 & *p) {
	*i = ((p[0] & 0x0F) << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
	return 4;
    }
    else {
	/* Also assume it's 32-bit data (0x08 & *p) */
	*i = (p[1] << 24) | (p[2] << 16) | (p[3] << 8) | p[4];
	return 5;
    }
}

#endif /* __VAR_INT_H__ */

/* The End */

/*
 * therm-daemon.h - Local definitions for the therm daemon's modules
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

#ifndef __THERM_DAEMON_H
#define __THERM_DAEMON_H

#include <stdarg.h>

struct rx_packet {
  int                buffer_bytes;
  int                buffer_bits;
  float              signal_strength;
  unsigned char*     buffer;
  int                current_bit;
};

void              packet_free     (struct rx_packet* self);

struct rx_packet* packet_dup      (struct rx_packet* self);
int               packet_cmp      (struct rx_packet* a,
				   struct rx_packet* b);

void              packet_rewind   (struct rx_packet* self);
int               packet_read_bit (struct rx_packet* self);
int               packet_read_int (struct rx_packet* self,
				   int               width);

int               packet_read_signed_int (struct rx_packet* self,
					  int               width);


/* If the receiver device can be successfully opened, returns its file descriptor.
 * Otherwise this returns -1. If it finds any devices without firmware,
 * it will download it to them and return -1.
 */
int    receiver_open             ();

/* Start receiving a packet, giving up after the specified timeout
 * in milliseconds. Returns NULL if no packet was received.
 */
struct rx_packet* receiver_read           (int fd, int timeout);

/* Get the local temperature at the receiver. Returns 0 on success, -1 on failure */
int               receiver_get_local_temp (int fd, int *temperature);

char*  strdup_vprintf            (const char* format, va_list ap);
char*  strdup_printf             (const char* format, ...);

#endif /* __THERM_DAEMON_H */

/* The End */

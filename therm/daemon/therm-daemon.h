/*
 * therm-daemon.h - Local definitions for the therm daemon's modules
 *
 * Wireless therm system
 * Copyright (C) 2004 Micah Dowty
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

#include <usb.h>
#include <stdarg.h>
#include <mysql.h>

struct rx_packet {
  int buffer_bytes;
  int buffer_bits;
  float signal_strength;
  unsigned char* buffer;
  int current_bit;
};

void              packet_free     (struct rx_packet* self);

struct rx_packet* packet_dup      (struct rx_packet* self);
int               packet_cmp      (struct rx_packet* a,
				   struct rx_packet* b);

void              packet_rewind   (struct rx_packet* self);
int               packet_read_bit (struct rx_packet* self);
int               packet_read_int (struct rx_packet* self,
				   int               width);

/* If the receiver device can be successfully opened, returns it.
 * Otherwise this returns NULL. If it finds any devices without firmware,
 * it will download it to them and return NULL.
 */
usb_dev_handle*   receiver_open           ();

/* Start receiving a packet, giving up after the specified timeout
 * in milliseconds. Returns NULL if no packet was received.
 */
struct rx_packet* receiver_read           (usb_dev_handle* self, int timeout);

/* Get the local temperature at the receiver. Returns 0 on success, -1 on failure */
int               receiver_get_local_temp (usb_dev_handle* self, int *temperature);

char*  strdup_vprintf            (const char* format, va_list ap);
char*  strdup_printf             (const char* format, ...);

char*  db_strdup_escape          (MYSQL* mysql, const char* string);
int    db_query_printf           (MYSQL* mysql, const char* format, ...);
int    db_find_source            (MYSQL* mysql, const char *medium, int protocol, int station_id);
int    db_packet_new             (MYSQL* mysql, int source_id);
int    db_packet_new_full        (MYSQL* mysql, int source_id, int sequence, float strength);
void   db_packet_mark_duplicate  (MYSQL* mysql, int packet_id);
void   db_packet_add_batt_volts  (MYSQL* mysql, int packet_id, float volts);
void   db_packet_add_temperature (MYSQL* mysql, int packet_id, float average, int num_samples);

#endif /* __THERM_DAEMON_H */

/* The End */

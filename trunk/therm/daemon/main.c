/*
 * main.c - Main loop for the therm daemon. This daemon receives data
 *          from the therm base station and stores it in a remote MySQL
 *          database.
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

#include <mysql.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "therm-daemon.h"

#define LOCAL_MS_PER_SAMPLE      4000    /* Approximate milliseconds per local temperature sample */
#define LOCAL_SAMPLES_PER_STORE  15      /* Number of local samples to average before storing */

void   store_packet              (MYSQL* mysql, struct rx_packet* packet);
void   store_packet_therm0       (MYSQL* mysql, struct rx_packet* packet);
void   store_local_temperature   (MYSQL* mysql, struct fraction* temperature);
void   average_local_temperature (MYSQL* mysql, int temperature);


int main(int argc, char **argv) {
  int fd;
  struct rx_packet* packet;
  MYSQL mysql;
  int t;

  if (argc != 5) {
    printf("usage: %s <hostname> <username> <password> <database>\n",
	   argv[0]);
    return 1;
  }

  mysql_init(&mysql);
  if (!mysql_real_connect(&mysql, argv[1], argv[2], argv[3],
			  argv[4], 0, NULL, 0)) {
    printf("MySQL connection failed: %s\n", mysql_error(&mysql));
    return 1;
  }

  /* Loop between connecting and reading... */
  while (1) {

    fd = receiver_open();
    if (fd >= 0)
      printf("Connected to base station.\n");
    else
      printf("Can't open device. Trying again...\n");

    /* Do something useful as long as we have a connection */
    while (fd >= 0) {
      packet = receiver_read(fd, LOCAL_MS_PER_SAMPLE);
      if (packet) {
	/* We have a packet from the radio. Decode it and
	 * store it somewhere if we can
	 */
	db_begin_transaction(&mysql);
	store_packet(&mysql, packet);
	db_commit_transaction(&mysql);
	packet_free(packet);
      }
      else {
	/* Timed out waiting for a packet. Take this opportunity
	 * to sample the local temperature
	 */
	if (receiver_get_local_temp(fd, &t) >= 0) {
	  db_begin_transaction(&mysql);
	  average_local_temperature(&mysql, t);
	  db_commit_transaction(&mysql);
	}
	else {
	  /* This shouldn't fail- reconnect, there could be something up with our device. */
	  close(fd);
	  fd = -1;
	}
      }
    }

    /* Wait between reconnects */
    sleep(1);
  }

  return 0;
}

void store_packet(MYSQL* mysql, struct rx_packet* packet)
{
  int protocol = packet_read_int(packet, 2);

  switch (protocol) {

  case 0:
    store_packet_therm0(mysql, packet);
    break;

  default:
    printf("Received a packet with unknown protocol %d\n", protocol);

  }
}

void store_packet_therm0(MYSQL* mysql, struct rx_packet* packet)
{
  /* Store a packet using temperature protocol 0 */
  int packet_id, source_id;
  int station_id, sequence, temp_total, temp_count;
  struct fraction voltage;
  static struct rx_packet* previous_packet = NULL;
  static int previous_packet_id = -1;

  /* Is this a duplicate of the last packet? */
  if (!packet_cmp(previous_packet, packet)) {
    db_packet_mark_duplicate(mysql, previous_packet_id);
    return;
  }

  /* Verify length */
  if (packet->buffer_bits != 47) {
    printf("Received a protocol 0 temperature packet with incorrect length (%d bits)\n",
	   packet->buffer_bits);
    return;
  }

  /* Unpack the packet */
  station_id = packet_read_int(packet, 6);
  sequence = packet_read_int(packet, 5);
  voltage.numerator = packet_read_int(packet, 10) * 10;
  voltage.denominator = 1024;
  temp_total = packet_read_signed_int(packet, 16);
  temp_count = packet_read_int(packet, 8);

  /* Look up a source ID for this packet */
  source_id = db_find_source(mysql, "rf", 0, station_id);
  if (source_id < 0)
    return;

  /* Allocate a new packet ID */
  packet_id = db_packet_new_full(mysql, source_id,
				 sequence, &packet->signal_strength);
  if (packet_id < 0) {
    printf("Error allocating a therm0 packet\n");
    return;
  }

  /* We always have a battery voltage, store that */
  db_packet_add_batt_volts(mysql, packet_id, &voltage);

  /* Only store temperatures if we actually have any- if the sensor is having
   * problems or the battery has just been inserted, there won't be any samples.
   */
  if (temp_count > 0) {
    struct fraction f;
    f.numerator = temp_total;
    f.denominator = temp_count;
    db_packet_add_temperature(mysql, packet_id, &f, temp_count);
  }

  /* This is now the previous packet */
  packet_free(previous_packet);
  previous_packet = packet_dup(packet);
  previous_packet_id = packet_id;
}

void store_local_temperature(MYSQL* mysql, struct fraction* temperature)
{
  int source_id, packet_id;

  /* Our source is usb:0:0 */
  source_id = db_find_source(mysql, "usb", 0, 0);
  if (source_id < 0)
    return;

  /* Create a new empty packet. We have no sequence number
   * or signal strength to report
   */
  packet_id = db_packet_new(mysql, source_id);
  if (packet_id < 0)
    return;

  /* Store our lone temperature average */
  db_packet_add_temperature(mysql, packet_id, temperature,
			    LOCAL_SAMPLES_PER_STORE);
}

void average_local_temperature(MYSQL* mysql, int temperature)
{
  static int total = 0;
  static int n_samples = 0;

  total += temperature;
  n_samples++;

  if (n_samples >= LOCAL_SAMPLES_PER_STORE) {
    struct fraction f;
    f.numerator = total;
    f.denominator = n_samples;
    store_local_temperature(mysql, &f);
    total = 0;
    n_samples = 0;
  }
}

/* The End */

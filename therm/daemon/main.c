/*
 * main.c - Main loop for the therm daemon. This daemon receives data
 *          from the therm base station and stores it in a remote MySQL
 *          database.
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

#include <usb.h>
#include <mysql.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include "therm-daemon.h"

#define LOCAL_MS_PER_SAMPLE      4000    /* Approximate milliseconds per local temperature sample */
#define LOCAL_SAMPLES_PER_STORE  15      /* Number of local samples to average before storing */


double timef                     ();
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

void   store_packet              (MYSQL* mysql, struct rx_packet* packet);
void   store_packet_therm0       (MYSQL* mysql, struct rx_packet* packet);
void   store_local_temperature   (MYSQL* mysql, float temperature);
void   average_local_temperature (MYSQL* mysql, int temperature);


int main(int argc, char **argv) {
  usb_dev_handle *rx;
  struct rx_packet* packet;
  MYSQL mysql;

  if (argc != 5) {
    printf("usage: %s <hostname> <username> <password> <database>\n",
	   argv[0]);
    return 1;
  }

  if (!(rx = receiver_open())) {
    printf("Error opening device\n");
    return 1;
  }

  mysql_init(&mysql);
  if (!mysql_real_connect(&mysql, argv[1], argv[2], argv[3],
			  argv[4], 0, NULL, 0)) {
    printf("MySQL connection failed: %s\n", mysql_error(&mysql));
    return 1;
  }

  while (1) {
    packet = receiver_read(rx, LOCAL_MS_PER_SAMPLE);
    if (packet) {
      store_packet(&mysql, packet);
      packet_free(packet);
    }
    else {
      average_local_temperature(&mysql, receiver_get_local_temp(rx));
    }
  }

  return 0;
}

double timef()
{
  /* A floating point version of time() */
  struct timeval tv;
  gettimeofday(&tv);
  return tv.tv_sec + tv.tv_usec / 1000000.0;
}

char* strdup_vprintf(const char* format, va_list ap)
{
  va_list ap2;
  int size;
  char* buffer;

  va_copy(ap2, ap);
  size = vsnprintf(NULL, 0, format, ap2)+1;
  va_end(ap2);

  buffer = malloc(size+1);
  assert(buffer != NULL);

  vsnprintf(buffer, size, format, ap);
  return buffer;
}

char* strdup_printf(const char* format, ...)
{
  char* buffer;
  va_list ap;
  va_start(ap, format);
  buffer = strdup_vprintf(format, ap);
  va_end(ap);
  return buffer;
}

char* db_strdup_escape(MYSQL* mysql, const char* string)
{
  int length = strlen(string);
  char* buffer = malloc(length*2+1);
  assert(buffer != NULL);
  mysql_real_escape_string(mysql, buffer, string, length);
  return buffer;
}

int db_query_printf  (MYSQL* mysql, const char* format, ...)
{
  va_list ap;
  int retval;
  char *buffer;
  va_start(ap, format);
  buffer = strdup_vprintf(format, ap);
  va_end(ap);
  retval = mysql_query(mysql, buffer);
  if (retval)
    printf("MySQL query failed: %s\nThe query was: '%s'\n", mysql_error(mysql), buffer);
  free(buffer);
  return retval;
}

int db_find_source(MYSQL* mysql, const char *medium, int protocol, int station_id)
{
  /* Search for a data source ID, given its medium, protocol number, and station ID.
   * Returns -1 if a matching source can't be found.
   */
  int retval;
  char *escaped_medium;
  MYSQL_RES *result;
  MYSQL_ROW row;

  escaped_medium = db_strdup_escape(mysql, medium);
  retval = db_query_printf(mysql,
			   "SELECT id FROM sources WHERE "
			   "medium = '%s' AND protocol = %d AND "
			   "station_id = %d",
			   escaped_medium, protocol, station_id);
  free(escaped_medium);
  if (!retval) {

    result = mysql_store_result(mysql);
    if (result) {

      row = mysql_fetch_row(result);
      if (row) {
	retval = atoi(row[0]);
	mysql_free_result(result);
	return retval;
      }
    }
  }
  printf("No match found for source %s:%d:%d\n", medium, protocol, station_id);
  return -1;
}

int db_packet_new(MYSQL* mysql, int source_id)
{
  /* In the database, create a new packet. Leave all optional parameters NULL.
   * Returns the new packet ID on success, or -1 on failure.
   */
  if (db_query_printf(mysql,
		      "INSERT INTO packets (time, source) VALUES (%f, %d)",
		      timef(), source_id))
    return  -1;
  return mysql_insert_id(mysql);
}

int db_packet_new_full(MYSQL* mysql, int source_id, int sequence, float strength)
{
  /* Create a new packet in the database, specifying all parameters */
  if (db_query_printf(mysql,
		      "INSERT INTO packets (time, source, sequence, "
		      "signal_strength) VALUES (%f, %d, %d, %f)",
		      timef(), source_id, sequence, strength))
    return  -1;
  return mysql_insert_id(mysql);
}

void db_packet_mark_duplicate(MYSQL* mysql, int packet_id)
{
  /* Increase the num_copies of an existing packet */
  db_query_printf(mysql,
		  "UPDATE packets SET num_copies = num_copies + 1 WHERE id = %d",
		  packet_id);
}

void db_packet_add_batt_volts  (MYSQL* mysql, int packet_id, float volts)
{
  /* Store a battery voltage for an existing packet */
  db_query_printf(mysql,
		  "INSERT INTO battery_voltage (packet, voltage) VALUES(%d, %f)",
		  packet_id, volts);
}

void db_packet_add_temperature (MYSQL* mysql, int packet_id, float average, int num_samples)
{
  /* Store a temperature for an existing packet */
  db_query_printf(mysql,
		  "INSERT INTO temperatures (packet, average, num_samples) VALUES(%d, %f, %d)",
		  packet_id, average, num_samples);
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
  float voltage;
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
  voltage = packet_read_int(packet, 10) / 1024.0 * 10.0;
  temp_total = packet_read_int(packet, 16);
  temp_count = packet_read_int(packet, 8);

  /* Look up a source ID for this packet */
  source_id = db_find_source(mysql, "rf", 0, station_id);
  if (source_id < 0)
    return;

  /* Allocate a new packet ID */
  packet_id = db_packet_new_full(mysql, source_id, sequence, packet->signal_strength);
  if (packet_id < 0) {
    printf("Error allocating a therm0 packet\n");
    return;
  }

  /* We always have a battery voltage, store that */
  db_packet_add_batt_volts(mysql, packet_id, voltage);

  /* Only store temperatures if we actually have any- if the sensor is having
   * problems or the battery has just been inserted, there won't be any samples.
   */
  if (temp_count > 0) {
    db_packet_add_temperature(mysql, packet_id,
			      ((float) temp_total) / temp_count,
			      temp_count);
  }

  /* This is now the previous packet */
  packet_free(previous_packet);
  previous_packet = packet_dup(packet);
  previous_packet_id = packet_id;
}

void store_local_temperature(MYSQL* mysql, float temperature)
{
  int source_id = db_find_source(mysql, "usb", 0, 0);

  if (source_id < 0)
    return;

  printf("Local temperature: %f\n", temperature);

}

void average_local_temperature(MYSQL* mysql, int temperature)
{
  static int total = 0;
  static int n_samples = 0;

  total += temperature;
  n_samples++;

  if (n_samples >= LOCAL_SAMPLES_PER_STORE) {
    store_local_temperature(mysql, ((float) total) / n_samples);
    total = 0;
    n_samples = 0;
  }
}

/* The End */

/*
 * database.c - Database abstraction for the therm daemon
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
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "therm-daemon.h"

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
		      "INSERT INTO packets (time, source) VALUES (CURRENT_TIMESTAMP, %d)",
		      source_id))
    return  -1;
  return mysql_insert_id(mysql);
}

int    db_packet_new_full        (MYSQL*                mysql,
				  int                   source_id,
				  int                   sequence,
                                  struct fraction*      strength)
{
  /* Create a new packet in the database, specifying all parameters */
  if (db_query_printf(mysql,
		      "INSERT INTO packets (time, source, sequence, "
		      "signal_strength) VALUES (CURRENT_TIMESTAMP, "
		      "%d, %d, %d/%d)",
		      source_id, sequence, strength->numerator,
		      strength->denominator))
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

void   db_packet_add_batt_volts  (MYSQL*                mysql,
				  int                   packet_id,
				  struct fraction*      volts)
{
  /* Store a battery voltage for an existing packet */
  db_query_printf(mysql,
		  "INSERT INTO battery_voltage (packet, voltage) "
		  "VALUES(%d, %d/%d)",
		  packet_id, volts->numerator, volts->denominator);
}

void   db_packet_add_temperature (MYSQL*                mysql,
				  int                   packet_id,
				  struct fraction*      average,
				  int                   num_samples)
{
  /* Store a temperature for an existing packet */
  db_query_printf(mysql,
		  "INSERT INTO temperatures (packet, average, num_samples) "
		  "VALUES(%d, %d/%d, %d)",
		  packet_id, average->numerator, average->denominator,
		  num_samples);
}

void db_begin_transaction (MYSQL* mysql)
{
  db_query_printf(mysql, "SET AUTOCOMMIT=0");
  db_query_printf(mysql, "BEGIN WORK");
}

void db_commit_transaction (MYSQL* mysql)
{
  db_query_printf(mysql, "COMMIT");
}

/* The End */

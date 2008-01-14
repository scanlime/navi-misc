/*
 * main.c - Main loop for the therm daemon. This daemon receives data
 *          from the therm base station, and stores each received
 *          sample by calling a provided shell script.
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

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include "therm-daemon.h"

#define LOCAL_MS_PER_SAMPLE      4000    /* Approximate milliseconds per local temperature sample */
#define LOCAL_SAMPLES_PER_STORE  15      /* Number of local samples to average before storing */

struct temperature_info {
  const char*        sensor_type;
  int                sensor_index;
  int                sequence;
  float              battery_voltage;
  float              temperature;
  float              signal_strength;
};

void   store_packet              (const char *dataDir, struct rx_packet* packet);
void   store_packet_therm0       (const char *dataDir, struct rx_packet* packet);
void   average_local_temperature (const char *dataDir, int temperature);
void   store_temperature_info    (const char *dataDir, struct temperature_info *info);

int main(int argc, char **argv) {
  int fd;
  struct rx_packet* packet;
  const char *dataDir;
  int t;

  if (argc != 2) {
    fprintf(stderr, "usage: %s <data directory>\n", argv[0]);
    return 1;
  }
  dataDir = argv[1];

  /* Loop between connecting and reading... */
  while (1) {

    fd = receiver_open();
    if (fd >= 0)
      fprintf(stderr, "Connected to base station.\n");
    else
      fprintf(stderr, "Can't open device. Trying again...\n");

    /* Do something useful as long as we have a connection */
    while (fd >= 0) {
      packet = receiver_read(fd, LOCAL_MS_PER_SAMPLE);
      if (packet) {
	store_packet(dataDir, packet);
	packet_free(packet);
      }
      else {
	/* Timed out waiting for a packet. Take this opportunity
	 * to sample the local temperature
	 */
	if (receiver_get_local_temp(fd, &t) >= 0) {
	  average_local_temperature(dataDir, t);
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

void store_packet(const char *dataDir, struct rx_packet* packet)
{
  int protocol = packet_read_int(packet, 2);

  switch (protocol) {

  case 0:
    store_packet_therm0(dataDir, packet);
    break;

  default:
    fprintf(stderr, "Received a packet with unknown protocol %d\n", protocol);

  }
}

void store_packet_therm0(const char *dataDir, struct rx_packet* packet)
{
  /* Store a packet using temperature protocol 0 */
  struct temperature_info info;
  int source_id, temp_total, temp_count;
  static struct rx_packet* previous_packet = NULL;

  /* Is this a duplicate of the last packet? */
  if (!packet_cmp(previous_packet, packet)) {
    /* Ignore it */
    return;
  }

  /* Verify length */
  if (packet->buffer_bits != 47) {
    fprintf(stderr, "Received a protocol 0 temperature packet with incorrect length (%d bits)\n",
	   packet->buffer_bits);
    return;
  }

  /* Unpack the packet */
  info.sensor_type = "rf";
  info.sensor_index = packet_read_int(packet, 6);
  info.sequence = packet_read_int(packet, 5);
  info.battery_voltage = packet_read_int(packet, 10) / 102.4f;
  temp_total = packet_read_signed_int(packet, 16);
  temp_count = packet_read_int(packet, 8);

  if (temp_count > 0) {
    info.temperature = temp_total / (float)temp_count;
    info.signal_strength = packet->signal_strength;

    store_temperature_info(dataDir, &info);
  }

  /* This is now the previous packet */
  packet_free(previous_packet);
  previous_packet = packet_dup(packet);
}

void average_local_temperature(const char *dataDir, int temperature)
{
  static int total = 0;
  static int n_samples = 0;
  static int sequence = 0;

  total += temperature;
  n_samples++;

  if (n_samples >= LOCAL_SAMPLES_PER_STORE) {
    struct temperature_info info;

    info.sensor_type = "local";
    info.sensor_index = 0;
    info.sequence = sequence++;
    info.battery_voltage = 0;
    info.temperature = total / (float)n_samples;
    info.signal_strength = 1.0f;

    store_temperature_info(dataDir, &info);

    total = 0;
    n_samples = 0;
  }
}

void store_temperature_info(const char *dataDir, struct temperature_info *info)
{
  char tmpFile[1024];
  char latestFile[1024];
  char rrdCommand[1024];
  FILE *f;
  time_t now = time(NULL);

  /*
   * Write the latest temperature data to a per-sensor file.  We write
   * to a separate file and move it into place, to ensure that the
   * data is updated atomically.
   */

  sprintf(tmpFile, "%s/%s%d.tmp", dataDir, info->sensor_type, info->sensor_index);
  sprintf(latestFile, "%s/%s%d.xml", dataDir, info->sensor_type, info->sensor_index);

  f = fopen(tmpFile, "w");
  if (!f) {
    perror(tmpFile);
    return;
  }

  fprintf(f, "<temperature-info>\n");
  fprintf(f, "    <timestamp>%d</timestamp>\n", now);
  fprintf(f, "    <sensor-type>%s</sensor-type>\n", info->sensor_type);
  fprintf(f, "    <sensor-index>%d</sensor-index>\n", info->sensor_index);
  fprintf(f, "    <sequence>%d</sequence>\n", info->sequence);
  fprintf(f, "    <battery-voltage>%f</battery-voltage>\n", info->battery_voltage);
  fprintf(f, "    <temperature>%f</temperature>\n", info->temperature);
  fprintf(f, "    <signal-strength>%f</signal-strength>\n", info->signal_strength);
  fprintf(f, "</temperature-info>\n");
  
  fclose(f);
  if (rename(tmpFile, latestFile) < 0) {
    perror(latestFile);
  }

  /*
   * Update this sensor's RRD files, using rrdtool.
   */
  
  sprintf(rrdCommand, "rrdtool update %s/%s%d-temperature.rrd %d:%f",
	  dataDir, info->sensor_type, info->sensor_index, now, info->temperature);
  system(rrdCommand);

  sprintf(rrdCommand, "rrdtool update %s/%s%d-voltage.rrd %d:%f",
	  dataDir, info->sensor_type, info->sensor_index, now, info->battery_voltage);
  system(rrdCommand);

  sprintf(rrdCommand, "rrdtool update %s/%s%d-signal.rrd %d:%f",
	  dataDir, info->sensor_type, info->sensor_index, now, info->signal_strength);
  system(rrdCommand);

  sprintf(rrdCommand, "rrdtool update %s/%s%d-sequence.rrd %d:%d",
	  dataDir, info->sensor_type, info->sensor_index, now, info->sequence);
  system(rrdCommand);
}

/* The End */

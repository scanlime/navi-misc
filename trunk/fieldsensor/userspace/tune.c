/*
 * calibrate.c - dump time v. sample data for each transmitter/reciever pair
 *
 * Copyright (C) 2004 David Trowbridge
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

#include <stdio.h>
#include <sys/time.h>
#include "field-sensor.h"
#include "../include/efs_protocol.h"

float
gettime ()
{
	static struct timeval base = {0, 0}, tv;
	int secs, usecs;

	if (base.tv_sec == 0)
		gettimeofday (&base, NULL);
	gettimeofday (&tv, NULL);

	secs = tv.tv_sec - base.tv_sec;
	usecs = tv.tv_usec - base.tv_usec;

	return secs + 1e-6f*usecs;
}

int
main (int argc, char *argv[])
{
	int i, j;
	FILE *data[8];
	char filename[] = "datax";
	unsigned char buffer[8];
	float time;
	FieldSensor *sensor;

	g_type_init ();

	sensor = field_sensor_new ();

	for (i = 0; i < 8; i++) {
		int tx = i / 2;

		field_sensor_set_result_index (sensor, i, i);
		field_sensor_set_lc_port_xor  (sensor, i, 0xff);
		field_sensor_set_period       (sensor, i, 29);
		field_sensor_set_phase        (sensor, i, 71);
		field_sensor_set_half_periods (sensor, i, 10);
		field_sensor_set_lc_port      (sensor, i, 0x55);
		if (i % 2) field_sensor_set_adcon_select (sensor, i, FS_RX_1);
		else       field_sensor_set_adcon_select (sensor, i, FS_RX_0);
		field_sensor_set_lc_tristate (sensor, i, (1 << tx));

		filename[4] = i + '0';
		data[i] = fopen (filename, "w");
	}

	for (j = 0; j < 256; j++) {
		for (i = 0; i < 8; i++)
			field_sensor_set_phase (sensor, i, j);
		field_sensor_take_reading (sensor, buffer);
		for (i = 0; i < 8; i++) {
			fprintf (data[i], "%d %d\n", j, (int) buffer[i]);
		}
	}

	for (i = 0; i < 8; i++)
		fclose (data[i]);
}

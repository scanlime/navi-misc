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
	unsigned char buffer[8];
	float time;
	FieldSensor *sensor;
	int range[8][2];
	float average[8];

	guchar phase[]  = {24, 33, 21, 28, 26, 34, 17, 14};
	guchar period[] = {27, 26, 29, 26, 27, 30, 29, 29};

	g_type_init ();

	sensor = field_sensor_new ();

	for (i = 0; i < 8; i++) {
		int tx = i / 2;

		field_sensor_set_result_index (sensor, i, i);
		field_sensor_set_lc_port_xor  (sensor, i, 0xff);
		field_sensor_set_period       (sensor, i, period[i]);
		field_sensor_set_phase        (sensor, i, phase[i]);
		field_sensor_set_half_periods (sensor, i, 10);
		field_sensor_set_lc_port      (sensor, i, 0x55);
		if (i % 2) field_sensor_set_adcon_select (sensor, i, FS_RX_1);
		else       field_sensor_set_adcon_select (sensor, i, FS_RX_0);
		field_sensor_set_lc_tristate (sensor, i, (1 << tx));

		average[i] = 0;
		range[i][0] = -1;
		range[i][1] = -1;
	}

	for (j = 0;; j++) {
		time = gettime ();
		field_sensor_take_reading (sensor, buffer);
		for (i = 0; i < 8; i++) {
			if (range[i][0] == -1)
				range[i][0] = buffer[i];
			if (range[i][1] == -1)
				range[i][1] = buffer[i];

			if (range[i][0] > buffer[i])
				range[i][0] = buffer[i];
			if (range[i][1] < buffer[i])
				range[i][1] = buffer[i];

			average[i] = ((float)j * average[i] + buffer[i]) / (j + 1);
		}

		if (j % 1000 == 0) {
			for (i = 0; i < 8; i++) {
				printf ("%f ", average[i]);
			}
			printf ("\n[%d,%d] [%d,%d] [%d,%d] [%d,%d] [%d,%d] [%d,%d] [%d,%d] [%d,%d]\n\n", range[0][0], range[0][1], range[1][0], range[1][1], range[2][0], range[2][1], range[3][0], range[3][1], range[4][0], range[4][1], range[5][0], range[5][1], range[6][0], range[6][1], range[7][0], range[7][1]);
		}
	}
}

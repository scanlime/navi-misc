/*
 * calibrate.c - dump sample data for each transmitter/reciever pair
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
#include "field-sensor.h"

struct filter{
	unsigned char *buffer;
	int *sums;
	int current;
	int sample_size;
	int num_samples;
} filter;

void filter_init(struct filter* self, int sample_size, int num_samples)
{
	self->buffer = malloc(sample_size * num_samples);
	memset(self->buffer, 0, sample_size * num_samples);
	self->sums = malloc(sizeof(int) * sample_size);
	memset(self->sums, 0, sample_size * sizeof(int));
	self->current = 0;
	self->sample_size = sample_size;
	self->num_samples = num_samples;
}

void filter_append(struct filter* self, unsigned char *sample)
{
	int i;
	for (i=0; i<self->sample_size; i++) {
		self->sums[i] += sample[i];
		self->sums[i] -= self->buffer[self->sample_size *
					      self->current + i];
	}
	memcpy(self->buffer + (self->sample_size * self->current),
	       sample, self->sample_size);
	self->current = (self->current + 1) % self->num_samples;
}

int
main (int argc, char *argv[])
{
	int i, j;
	unsigned char buffer[8];
	struct filter filter;
	FieldSensor *sensor;

	g_type_init ();

	sensor = field_sensor_new ();
	filter_init (&filter, 8, 64);

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
	}

	for (j = 0;; j++) {
		field_sensor_take_reading (sensor, buffer);
		filter_append(&filter, buffer);

		for (i = 0; i < filter.sample_size; i++)
			printf ("%f ", (float)filter.sums[i] / filter.num_samples);
		printf ("\n");
		fflush(stdout);
	}
}

/*
 * theremin.c - digital theremin using an arbitrary transform from
 *              sensor output to MIDI events
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
#include <unistd.h>
#include <alsa/asoundlib.h>
#include "field-sensor.h"

struct filter {
	unsigned char *buffer;
	int *sums;
	int current;
	int sample_size;
	int num_samples;
} filter;

void
filter_init (struct filter *self, int sample_size, int num_samples)
{
	self->buffer = malloc (sample_size * num_samples);
	memset (self->buffer, 0, sample_size * num_samples);
	self->sums = malloc(sizeof(int) * sample_size);
	memset(self->sums, 0, sample_size * sizeof(int));
	self->current = 0;
	self->sample_size = sample_size;
	self->num_samples = num_samples;
}

void
filter_append (struct filter *self, unsigned char *sample)
{
	int i;
	for (i=0; i<self->sample_size; i++) {
		self->sums[i] += sample[i];
		self->sums[i] -= self->buffer[self->sample_size * self->current + i];
	}
	memcpy(self->buffer + (self->sample_size * self->current), sample, self->sample_size);
	self->current = (self->current + 1) % self->num_samples;
}

int
main (int argc, char *argv[])
{
	int i, port;
	snd_seq_t *seq;
	snd_seq_event_t ev;
	unsigned char buffer[8];
	struct filter filter;
	FieldSensor *sensor;

	const int dest_client = 128,
	          dest_port = 0;

	guchar phase[]  = {24, 33, 21, 28, 26, 34, 17, 14};
	guchar period[] = {27, 26, 29, 26, 27, 30, 29, 29};

	g_type_init ();

	sensor = field_sensor_new ();
	filter_init (&filter, 8, 64);

	snd_seq_open (&seq, "default", SND_SEQ_OPEN_DUPLEX, 0);
	snd_seq_set_client_name (seq, "Digital Theremin");
	port = snd_seq_create_simple_port (seq, "Digital Theremin Output",
	                                   SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ,
					   SND_SEQ_PORT_TYPE_APPLICATION);
	snd_seq_connect_to (seq, port, dest_client, dest_port);

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
	}

	while (1) {
		field_sensor_take_reading (sensor, buffer);
		filter_append (&filter, buffer);

		snd_seq_ev_clear (&ev);
		snd_seq_ev_set_direct (&ev);
		snd_seq_ev_set_source (&ev, port);
		snd_seq_ev_set_dest (&ev, dest_client, dest_port);

		snd_seq_ev_set_pitchbend (&ev, 0, (float)filter.sums[0] / filter.num_samples * 64 - 8192);
		snd_seq_event_output (seq, &ev);

		for (i = 1; i < 6; i++)
			snd_seq_ev_set_controller (&ev, 0, i, filter.sums[i] / filter.num_samples - 127);
		snd_seq_event_output (seq, &ev);

		snd_seq_drain_output (seq);

		usleep (1);
	}
}

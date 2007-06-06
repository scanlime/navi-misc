/*
 * Copyright (C) 2007 David Trowbridge and Micah dowty
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
#include <math.h>
#include <fcntl.h>
#include <alsa/asoundlib.h>
#include "field-sensor.h"

guchar phase[]  = {24,     33,     21,     28,     26,     34,     17,     14};
guchar period[] = {27,     26,     29,     26,     27,     30,     29,     29};
float  mean[]   = {179.27, 199.08, 190.07, 178.63, 205.96, 167.80, 251.76, 236.91};
float  lbound[] = {168,    167,    172,    164,    179,    158,    183,    176};
float  ubound[] = {212,    254,    252,    253,    255,    198,    253,    240};

#define max(n,m) ((n > m) ? (n) : (m))

#define N_FRAMES 1024
#define N_VOICES 2

struct filter{
	unsigned char *buffer;
	int *sums;
	int current;
	int sample_size;
	int num_samples;
} filter;

snd_pcm_t *playback_handle;

struct {
   float frequency, magnitude, theta;
} voices[N_VOICES];


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

float
scale (float input, int channel)
{
	static float scale[8];
	static gboolean init = FALSE;
	int i;

	if (!init) {
		for (i = 0; i < 8; i++) {
			scale[i] = max (ubound[i] - mean[i], mean[i] - lbound[i]);
		}
		init = TRUE;
	}

	return (input - mean[channel]) / scale[channel];
}

void
sound_init ()
{
   snd_pcm_hw_params_t *hw_params;
   snd_pcm_sw_params_t *sw_params;
   snd_pcm_sframes_t frames_to_deliver;
   int nfds;
   int rate = 44100;
   int err;

   if ((err = snd_pcm_open (&playback_handle, "hw:0", SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
      fprintf (stderr, "cannot open audio device (%s)\n", 
               snd_strerror (err));
      exit (1);
   }

   if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0) {
      fprintf (stderr, "cannot allocate hardware parameter structure (%s)\n",
               snd_strerror (err));
      exit (1);
   }

   if ((err = snd_pcm_hw_params_any (playback_handle, hw_params)) < 0) {
      fprintf (stderr, "cannot initialize hardware parameter structure (%s)\n",
               snd_strerror (err));
      exit (1);
   }

   if ((err = snd_pcm_hw_params_set_access (playback_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
      fprintf (stderr, "cannot set access type (%s)\n",
               snd_strerror (err));
      exit (1);
   }

   if ((err = snd_pcm_hw_params_set_format (playback_handle, hw_params, SND_PCM_FORMAT_S16_LE)) < 0) {
      fprintf (stderr, "cannot set sample format (%s)\n",
               snd_strerror (err));
      exit (1);
   }

   if ((err = snd_pcm_hw_params_set_rate_near (playback_handle, hw_params, &rate, 0)) < 0) {
      fprintf (stderr, "cannot set sample rate (%s)\n",
               snd_strerror (err));
      exit (1);
   }

   if ((err = snd_pcm_hw_params_set_channels (playback_handle, hw_params, 2)) < 0) {
      fprintf (stderr, "cannot set channel count (%s)\n",
               snd_strerror (err));
      exit (1);
   }

   if ((err = snd_pcm_hw_params (playback_handle, hw_params)) < 0) {
      fprintf (stderr, "cannot set parameters (%s)\n",
               snd_strerror (err));
      exit (1);
   }

   snd_pcm_hw_params_free (hw_params);
}

void
sound_poll()
{
   int frames_to_deliver, err, i;
   struct {
      signed short l, r;
   } buf[N_FRAMES];

   if ((frames_to_deliver = snd_pcm_avail_update (playback_handle)) < 0) {
      if (frames_to_deliver == -EPIPE) {
         fprintf (stderr, "an xrun occured\n");
         return;
      } else {
         fprintf (stderr, "unknown ALSA avail update return value (%d)\n", 
                  frames_to_deliver);
         return;
      }
   }

   frames_to_deliver = frames_to_deliver > N_FRAMES ? N_FRAMES : frames_to_deliver;

   for (i=0; i<frames_to_deliver; i++) {
      int acc = 0;
      int voice;

      for (voice=0; voice<N_VOICES; voice++) {
         voices[voice].theta = fmod(voices[voice].theta +
                                    (voices[voice].frequency / 44100.0 * 2 * M_PI), 2*M_PI);
         acc += sin(voices[voice].theta) * 0x3000 * voices[voice].magnitude;
      }

      if (acc > 0x7fff) acc = 0x7fff;
      if (acc < -0x7fff) acc = -0x7fff;

      buf[i].l = buf[i].r = acc;
   }

   if ((err = snd_pcm_writei (playback_handle, buf, frames_to_deliver)) < 0) {
      fprintf (stderr, "write failed (%s)\n", snd_strerror (err));
   }
}

int
main (int argc, char *argv[])
{
   int i, j;
   unsigned char buffer[8];
   struct filter filter;
   struct filter slow_filter;
   FieldSensor *sensor;

   g_type_init ();

   sensor = field_sensor_new ();
   filter_init (&filter, 8, 2);
   filter_init (&slow_filter, 8, 16);

   sound_init();

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

   fcntl(1, F_SETFL, O_NONBLOCK);

   for (j = 0;; j++) {
      float scaled[8];
      float mag = 0;

      field_sensor_take_reading (sensor, buffer);
      filter_append(&filter, buffer);
      filter_append(&slow_filter, buffer);

      for (i=0; i<8; i++) {
         scaled[i] = scale ((float)filter.sums[i] / filter.num_samples, i);

         float s = scale ((float)slow_filter.sums[i] / slow_filter.num_samples, i);
         mag += s * s;
      }
      mag = max(0, log(mag) - log(0.01)) * 0.5;

      voices[0].frequency = pow(440, 1 + (scaled[4] - scaled[1]) * 0.7);
      voices[0].magnitude = mag;

      voices[1].frequency = voices[0].frequency * (1 - scaled[6]);
      voices[1].magnitude = voices[0].magnitude * 0.8;

      sound_poll();
   }
}


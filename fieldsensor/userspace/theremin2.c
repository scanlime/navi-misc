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
float  mean[]   = {190.050140, 221.423645, 201.651016, 187.882416, 226.742813, 173.381256, 252.022629, 250.073822 
};
float  lbound[] = {168,    167,    172,    164,    179,    158,    183,    176};
float  ubound[] = {212,    254,    252,    253,    255,    198,    253,    240};

#define max(n,m) ((n > m) ? (n) : (m))

#define N_FRAMES 32
#define N_VOICES 2

struct filter{
	unsigned char *buffer;
	int *sums;
	int current;
	int sample_size;
	int num_samples;
} filter;

snd_pcm_t *playback_handle;

volatile struct {
  double frequency, freqLatch;
  double magnitude, theta;
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
sound_init (const char *device)
{
   int err;

   if ((err = snd_pcm_open (&playback_handle, device, SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
      fprintf (stderr, "cannot open audio device (%s)\n", 
               snd_strerror (err));
      exit (1);
   }

   if ((err = snd_pcm_set_params(playback_handle,
				 SND_PCM_FORMAT_S16_LE,
				 SND_PCM_ACCESS_RW_INTERLEAVED,
				 2,
				 44100,
				 1,
				 10000))) {
     printf("Playback open error: %s\n", snd_strerror(err));
     exit(1);
   }
}

void
sound_poll()
{
   int frames, i, voice;
   struct {
     signed short l,r;
   } buffer[N_FRAMES];

   for (voice=0; voice<N_VOICES; voice++) {
     voices[voice].freqLatch = voices[voice].frequency;
   }

   for (i=0; i<N_FRAMES; i++) {
     int acc = 0;

     for (voice=0; voice<N_VOICES; voice++) {
       acc += sin(voices[voice].theta + 
		  (i * voices[voice].freqLatch / 44100.0 * 2*M_PI)) * 0x3000 * voices[voice].magnitude;
     }

     if (acc > 0x7fff) acc = 0x7fff;
     if (acc < -0x7fff) acc = -0x7fff;

     buffer[i].l = buffer[i].r = acc;
   }

   frames = snd_pcm_writei(playback_handle, buffer, N_FRAMES);
   if (frames < 0) {
     int err = snd_pcm_recover(playback_handle, frames, 0);
     if (err < 0) {
       printf("snd_pcm_recover failed: %s\n", snd_strerror(err));
     }
     return;
   }

   for (voice=0; voice<N_VOICES; voice++) {
     voices[voice].theta = fmod(voices[voice].theta +
				(frames * voices[voice].freqLatch / 44100.0 * 2*M_PI), 2*M_PI);
   }

   snd_pcm_start(playback_handle);
}

void 
sound_thread(void *arg)
{
  sound_init((char*) arg);
  while (1) {
    sound_poll();
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
   pthread_t thread;

   if (pthread_create(&thread, NULL, sound_thread, argv[1]) < 0) {
     perror("pthread_create");
   }

   g_type_init ();

   sensor = field_sensor_new ();
   filter_init (&filter, 8, 16);
   filter_init (&slow_filter, 8, 32);

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

      voices[0].frequency = pow(440, 1 + (scaled[1] - scaled[4]) * 0.5);
      voices[0].magnitude = mag;

      printf("%.04f %.04f %f\n", voices[0].frequency, voices[0].magnitude, scaled[6]);

      voices[1].frequency = voices[0].frequency * (1 - scaled[6]);
      voices[1].magnitude = voices[0].magnitude * 0.8;
   }
}


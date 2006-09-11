/*
 * main.c -- PulseAudio MI6K visualizer
 *
 * Copyright (C) 2006 Micah Dowty
 */

#include <string.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <alloca.h>
#include <complex.h>
#include <fftw3.h>
#include <pulse/simple.h>
#include "mi6k.h"

#define MAX(a,b)  ((a) > (b) ? (a) : (b))

#define FFT_NUM_FRAMES    512
#define FFT_OUTPUT_SIZE   (FFT_NUM_FRAMES / 2 + 1)

#define DISPLAY_GAIN      1.5
#define DISPLAY_FADE_RATE 0.7
#define PEAK_FADE_RATE    0.97
#define MIN_PEAK_VALUE    4.0

/*
 * Randomly choose an item, given the probability that
 * each should be chosen. We treat the weights as probabilities
 * that haven't been normalized.
 *
 * This generates a cumulative distribution function, then
 * searches that CDF using a uniform random variable. For example,
 * let's say we have the following weights:
 *   { 0.5, 0.5, 1.0 }
 *
 * This will give us the cumulative distribution:
 *   { 0.5, 1.0, 2.0 }
 *
 * A random number in the range [0, 2.0) is chosen. If it's less than
 * 0.5, we choose the first item. If it's less than 1.0, we choose
 * the second item, etc.
 */
int choose_weighted(float *weights, int num_items)
{
    float *cdf = alloca(num_items * sizeof(float));
    float r;
    int i;

    cdf[0] = weights[0];
    for (i=1; i<num_items; i++) {
	cdf[i] = cdf[i-1] + weights[i];
    }

    r = random() / (double) RAND_MAX;
    r *= cdf[num_items - 1];

    /* 
     * Since our cdf is sorted, we could optimize this
     * with a binary search rather than a linear search.
     */
    for (i=0; i<num_items; i++) {
	if (r < cdf[i]) {
	    break;
	}
    }
    return i;
}

int main(int argc, char *argv[])
{
    pa_simple *s;
    pa_sample_spec ss;
    float fft_frames[FFT_NUM_FRAMES];
    fftwf_plan fft_plan;
    fftwf_complex *fft_output;
    double fft_combined[FFT_OUTPUT_SIZE] = { 0 };
    float columns[MI6K_WIDTH] = { 0 };
    float prev_columns[MI6K_WIDTH] = { 0 };
    float brightness_priority[MI6K_WIDTH] = { 0 };
    float peak = MIN_PEAK_VALUE;
    float next_peak;

    mi6k_init();    

    ss.format = PA_SAMPLE_FLOAT32LE;
    ss.channels = 1;
    ss.rate = 48000;

    fft_output = fftwf_malloc(sizeof(*fft_output) * FFT_OUTPUT_SIZE);
    assert(fft_output);
    fft_plan = fftwf_plan_dft_r2c_1d(FFT_NUM_FRAMES, fft_frames, fft_output, 0);
    assert(fft_plan);

    s = pa_simple_new(NULL, "pulse6k", PA_STREAM_RECORD, "alsa_output.default.monitor",
		      "Spectrum Analyzer", &ss, NULL, NULL, NULL);
    if (!s) {
	fprintf(stderr, "Error connecting to PulseAudio\n");
	return 1;
    }
    fprintf(stderr, "Connected.\n");

    while (1) {
	pa_memchunk frame;
	unsigned char *frame_chars;
	struct mi6k_dim_command *dim_command;
	int rv, i, freq;

	/*
	 * Read enough samples for our fixed FFT window, and perform the FFT
	 */
	rv = pa_simple_read(s, fft_frames, sizeof fft_frames, NULL);
	assert(rv == 0);
	fftwf_execute_dft_r2c(fft_plan, fft_frames, fft_output);
	for (i=0; i < FFT_OUTPUT_SIZE; i++) {
	    fft_combined[i] = MAX(fft_combined[i], cabs(fft_output[i]));
	}

	if (!mi6k_needs_frame()) {
	    continue;
	}

      	freq = 0;
	next_peak = MIN_PEAK_VALUE;
	for (i=0; i<MI6K_WIDTH; i++) {
	    float samples = 0;
	    int tap_width;
	    static const int tap_widths[] = {
		2, 1, 1, 1, 1, 1, 1, 1, 1, 2,
		2, 2, 2, 2, 4, 4, 4, 4, 4, 4
	    };

	    for (tap_width = tap_widths[i]; tap_width; freq++, tap_width--) {
		samples += fft_combined[freq];
	    }

            /*
	     * We exclude the very first column from the AGC calculations,
	     * so as not to let bass totally take over the display.
	     */
	    if (i > 0) {
	        next_peak = MAX(next_peak, samples);
	    }
		
	    columns[i] = MAX(samples / peak * DISPLAY_GAIN,
			     columns[i] * DISPLAY_FADE_RATE);
	}

	/* Update the automatic gain control */
	peak = MAX(next_peak, peak * PEAK_FADE_RATE);

	mi6k_frame_new(&frame, &frame_chars, &dim_command);
	mi6k_draw_bargraph(frame_chars, columns);

	/*
	 * We get to update one column's brightness per frame.
	 * Choose a column randomly, but put more weight on
	 * columns that have changed more since we last updated them.
	 */
	for (i=0; i<MI6K_WIDTH; i++) {
	    float diff = columns[i] - prev_columns[i];
	    brightness_priority[i] += diff * diff;
	    prev_columns[i] = columns[i];
	}
	i = choose_weighted(brightness_priority, MI6K_WIDTH);
	brightness_priority[i] = 0;
	dim_command->column = i;
	dim_command->level = MI6K_BRIGHTNESS(columns[i]);

	mi6k_commit_frame(&frame);

	memset(fft_combined, 0, sizeof fft_combined);
    }

    return 0;
}


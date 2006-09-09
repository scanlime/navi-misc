/*
 * main.c -- PulseAudio MI6K visualizer
 *
 * Copyright (C) 2006 Micah Dowty
 */

#include <string.h>
#include <assert.h>
#include <math.h>
#include <complex.h>
#include <fftw3.h>
#include <pulse/simple.h>
#include "mi6k.h"

#define MAX(a,b)  ((a) > (b) ? (a) : (b))

#define FFT_NUM_FRAMES    512
#define FFT_OUTPUT_SIZE   (FFT_NUM_FRAMES / 2 + 1)

#define DISPLAY_GAIN      1.5
#define DISPLAY_FADE_RATE 0.5
#define PEAK_FADE_RATE    0.95
#define MIN_PEAK_VALUE    4.0

int main(int argc, char *argv[])
{
    pa_simple *s;
    pa_sample_spec ss;
    float fft_frames[FFT_NUM_FRAMES];
    fftwf_plan fft_plan;
    fftwf_complex *fft_output;
    double fft_average[FFT_OUTPUT_SIZE] = { 0 };
    int num_fft_average_samples = 0;
    float columns[MI6K_WIDTH] = { 0 };
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
	int rv, i, freq;

	/*
	 * Read enough samples for our fixed FFT window, and perform the FFT
	 */
	rv = pa_simple_read(s, fft_frames, sizeof fft_frames, NULL);
	assert(rv == 0);
	fftwf_execute_dft_r2c(fft_plan, fft_frames, fft_output);
	
	/*
	 * Keep averaging the FFT results, taking the absolute value
	 * of each complex number.
	 */

	for (i=0; i < FFT_OUTPUT_SIZE; i++) {
	    fft_average[i] += cabs(fft_output[i]);
	}
	num_fft_average_samples++;

	/*
	 * If the display isn't ready for a new frame, go back to waiting for audio
	 */
	if (!mi6k_needs_frame()) {
	    continue;
	}

	/*
	 * Resample the FFT averages. Each column onscreen is going to result from
	 * an average of several FFT samples in time, as well as several frequencies
	 * from the resulting FFT.
	 */
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
		samples += fft_average[freq];
	    }

	    samples /= num_fft_average_samples;
	    next_peak = MAX(next_peak, samples);
	    columns[i] = MAX(samples / peak * DISPLAY_GAIN,
			     columns[i] * DISPLAY_FADE_RATE);
	}

	/* Update the automatic gain control */
	peak = MAX(next_peak, peak * PEAK_FADE_RATE);

	/*
	 * Render the bargraph, and queue it for output
	 */
	mi6k_frame_bargraph(&frame, columns);
	mi6k_commit_frame(&frame);

	/*
	 * Reset the averaging buffer
	 */
	memset(fft_average, 0, sizeof fft_average);
	num_fft_average_samples = 0;
    }

    return 0;
}


/*
 * Proof of concept, NTSC video modulation completely in software!
 *
 * Use the following X modeline:
 *      DotClock 76.50
 *      HTimings 4096 4096 4104 4112
 *      VTimings 308 308 309 310
 *
 * A wide range of timings may work as long as you update the #defines
 * below. This reads from testimage.jpeg (any resolution) and writes
 * ntsc.pgm. Connect the antenna input on your TV directly to one of
 * the RGB channels on your video card (pin 1/2/3) and tune to channel
 * 7. Display the resulting ntsc.pgm full-screen.
 *
 * You should see a color rendition of testimage.jpeg! The white specks
 * are "blind spots" caused by the horizontal sync. Also, the picture
 * is currently not interlaced, so vertical resolution is reduced.
 *
 * Copyright (C) 2006 Micah Dowty <micah@navi.cx>
 */

#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <jpeglib.h>

/* Video mode */
#define MODE_DOTCLOCK 76.5e6
#define MODE_WIDTH    4096
#define MODE_HTOTAL   4112
#define MODE_HEIGHT   308
#define MODE_VTOTAL   310

/* NTSC */
#define CHANNEL_HZ       175.25e6  /* VHF channel 7 */
#define COLORBURST_HZ    3579545.0
#define LINE_TOTAL       63.5e-6
#define LINE_BLANK       10.9e-6
#define LINE_FRONT       1.5e-6
#define LINE_BACK        6.2e-6
#define LINE_BURST_START 5.8e-6
#define LINE_BURST_END   (LINE_BURST_START + 9.0 / COLORBURST_HZ)
#define NTSC_BLACK       10
#define NTSC_LUMA        90
#define NTSC_CHROMA_U    60
#define NTSC_CHROMA_V    60

struct image_info {
    struct jpeg_decompress_struct cinfo;
    unsigned char *buffer;
    int rowStride;
};

void image_open(struct image_info *img, const char *filename)
{
    FILE *f = fopen(filename, "rb");
    struct jpeg_error_mgr jerr;

    if (!f) {
	perror("fopen");
	exit(1);
    }

    memset(&jerr, 0, sizeof(jerr));
    img->cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&img->cinfo);
    jpeg_stdio_src(&img->cinfo, f);
    jpeg_read_header(&img->cinfo, TRUE);
    jpeg_start_decompress(&img->cinfo);

    assert(img->cinfo.output_components == 3);
    img->rowStride = img->cinfo.output_width * img->cinfo.output_components;
    img->buffer = malloc(img->rowStride);
}

void image_sample(struct image_info *img, float x, int line,
		  float *r, float *g, float *b)
{
    int img_x = x * (img->cinfo.output_width - 1);
    int img_y = line * img->cinfo.output_height / 262;

    if (img_y < 0 || img_y >= img->cinfo.output_height) {
	/* Past the edges of the image */
	*r = 0;
	*g = 0;
	*b = 0;
    }

    while (img->cinfo.output_scanline < img_y) {
	jpeg_read_scanlines(&img->cinfo, &img->buffer, 1);
    }

    *r = img->buffer[img_x * 3 + 0] / 255.0;
    *g = img->buffer[img_x * 3 + 1] / 255.0;
    *b = img->buffer[img_x * 3 + 2] / 255.0;
}

int main()
{
    int i, j;
    unsigned long clock = 0;
    struct image_info input;

    FILE *image = fopen("ntsc.pgm", "wb");
    assert(image != NULL);
    image_open(&input, "testimage.jpeg");

    printf("Vertical: %f Hz\n", MODE_DOTCLOCK / (MODE_HTOTAL * MODE_VTOTAL));

    /* Grayscale Portable Graymap header */
    fprintf(image, "P5 %d %d 255\n", MODE_WIDTH, MODE_HEIGHT);

    /*
     * Build the image from a pixel function that returns a value in
     * [-1, 1] given the current time, in seconds.
     */
    for (j=0; j<MODE_HEIGHT; j++) {
	for (i=0; i<MODE_WIDTH; i++) {
	    int level;
	    int line_number;
	    int horiz_pixel;
	    double line_offset;
	    float rf_signal, if_signal, irp;
	    float color_sin, color_cos, color_theta;

	    /* Time in seconds, NTSC lines, and an
	     * offset from the line beginning. The line
	     * number is one-based to match common NTSC usage.
	     */
	    line_number = clock / (LINE_TOTAL * MODE_DOTCLOCK) + 1;
	    line_offset = fmod(clock, LINE_TOTAL * MODE_DOTCLOCK) / MODE_DOTCLOCK;

	    /* Calculate the reference signal for colorburst, sine and cosine channels.
	     * Must be written carefully to avoid floating point error!
	     */
	    color_theta = fmod(clock * (COLORBURST_HZ / MODE_DOTCLOCK), 1) * 2*M_PI;
	    color_sin = sinf(color_theta);
	    color_cos = cosf(color_theta);

	    if (line_number <= 6 && line_number >= 4) {
		/* Serration pulses */
		if (line_offset < 0.43 * LINE_TOTAL ||
		    (line_offset > 0.5 * LINE_TOTAL &&
		     line_offset < 0.93 * LINE_TOTAL)) {
		    irp = -40;
		}
		else {
		    irp = 0;
		}
	    }
	    else if (line_number <= 9) {
		/* Equalizing pulses */
		if (line_offset < 0.04 * LINE_TOTAL ||
		    (line_offset > 0.5 * LINE_TOTAL &&
		     line_offset < 0.54 * LINE_TOTAL)) {
		    irp = -40;
		}
		else {
		    irp = 0;
		}
	    }
	    else if (line_offset >= LINE_FRONT && line_offset <= LINE_BACK) {
		/* Sync tip */
		irp = -40;
	    }
	    else if (line_offset >= LINE_BURST_START &&
		     line_offset <= LINE_BURST_END) {
		/* Colorburst */
		irp = color_cos * 20;
	    }
	    else if (line_offset < LINE_BLANK) {
		/* Horizontal blanking */
		irp = 0;
	    }
	    else if (line_number <= 20) {
		/* Blank lines */
		irp = 10;
	    }
	    else {
		/* Video signal */
		float r, g, b, y, u, v;

		image_sample(&input,
			     (line_offset - LINE_BLANK) / (LINE_TOTAL - LINE_BLANK),
			     line_number - 21,
			     &r, &g, &b);

		/* Slow but tweakable RGB to YUV, without bias terms */
		y = 0.257*r + 0.504*g + 0.098*b;
		u = 0.439*r - 0.368*g - 0.071*b;
		v = -0.148*r - 0.291*g + 0.439*b;

		/* Modulate U and V on alternate phases of the color carrier */
		irp = NTSC_BLACK + y * NTSC_LUMA +
		    color_sin * u * NTSC_CHROMA_U -
		    color_cos * v * NTSC_CHROMA_V;
	    }

	    /* Convert IRP level to an IF signal between 0 and 1 */
	    if_signal = 1 - (irp + 40.0) / 140.0;
	    
	    /* RF Modulation.
	     * This must be written carefully to avoid floating point error.
	     */ 
	    rf_signal = if_signal * sinf(fmod(clock * (CHANNEL_HZ /
						       MODE_DOTCLOCK), 1) * 2*M_PI); 

	    level = (rf_signal + 1) * 127;
	    assert(level >= 0);
	    assert(level <= 255);
	    fputc(level, image);
	    clock++;
	}
	clock += MODE_HTOTAL - MODE_WIDTH;
    }
    return 0;
}

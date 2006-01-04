/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * exr.cpp - Optional OpenEXR extensions to the HistogramImager,
 *           for saving high dynamic range images from it.
 *
 * Fyre - rendering and interactive exploration of chaotic functions
 * Copyright (C) 2004-2006 David Trowbridge and Micah Dowty
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

extern "C" {
#include "histogram-imager.h"
#include "config.h"
}

#include <ImfRgbaFile.h>
using namespace Imf;

void exr_save_real (HistogramImager *hi, const gchar *filename);

#define fyre_exr_error_quark() (g_quark_from_string("FYRE_EXR_ERROR"))
enum {
    FYRE_EXR_SAVE_FAILURE,
} FyreExrError;

extern "C" void exr_save_image_file(HistogramImager *hi, const gchar* filename, GError **error)
{
    try {
	exr_save_real (hi, filename);
    } catch (const std::exception &exc) {
	GError *nerror = g_error_new (fyre_exr_error_quark(), FYRE_EXR_SAVE_FAILURE, exc.what());
	*error = nerror;
    }
}

void
exr_save_real (HistogramImager *hi, const gchar *filename)
{
    int width = hi->width;
    int height = hi->height;
    RgbaOutputFile file (filename, width, height);
    Rgba *pixels = new Rgba[width * height];
    Rgba *cur_pixel = pixels;
    const guint oversample = hi->oversample;
    float fscale = histogram_imager_get_pixel_scale(hi);
    float one_over_gamma = 1.0 / hi->gamma;
    guint* cur_bucket = hi->histogram;

    struct {
	float r,g,b,a;
    } bg, fg, range, bucket, pixel;

    bg.r = hi->bgcolor.red / 65535.0;
    bg.g = hi->bgcolor.green / 65535.0;
    bg.b = hi->bgcolor.blue / 65535.0;
    bg.a = hi->bgalpha / 65535.0;

    fg.r = hi->fgcolor.red / 65535.0;
    fg.g = hi->fgcolor.green / 65535.0;
    fg.b = hi->fgcolor.blue / 65535.0;
    fg.a = hi->fgalpha / 65535.0;

    range.r = fg.r - bg.r;
    range.g = fg.g - bg.g;
    range.b = fg.b - bg.b;
    range.a = fg.a - bg.a;

    int histogram_stride = oversample * width;
    float oversample_squared = oversample * oversample;
    int histogram_block_stride = (oversample-1) * histogram_stride;

    /* This outer loop iterates over pixels in the resulting image */
    for (int pix_y = height; pix_y; pix_y--) {
	for (int pix_x = width; pix_x; pix_x--) {

	    /* Then for each pixel, loop over the corresponding histogram bins.
	     * For oversample==1 this is only one bin, otherwise it's a square
	     * block of oversample^2 bins. For each bin we compute the finished
	     * color values and add them. We don't actually use fyre's oversampling
	     * gamma here, since the OpenEXR pixel values should already be linear.
	     */
	    pixel.r = pixel.g = pixel.b = pixel.a = 0;

	    guint* cur_bucket_row = cur_bucket;
	    for (int bucket_y = oversample; bucket_y; bucket_y--) {
		guint* cur_bucket_sample = cur_bucket_row;
		for (int bucket_x = oversample; bucket_x; bucket_x--) {

		    /* Linear exposure plus gamma adjustment */
		    float luma = (*cur_bucket_sample) * fscale;
		    luma = pow(luma, one_over_gamma);

		    /* Optionally clamp before interpolating */
		    if (hi->clamped && luma > 1)
			luma = 1;

		    /* Color interpolation, with no per-component clamping */
		    bucket.r = luma * range.r + bg.r;
		    bucket.g = luma * range.g + bg.g;
		    bucket.b = luma * range.b + bg.b;
		    bucket.a = luma * range.a + bg.a;

		    /* Fyre images are generally authored to look good in sRGB,
		     * so they'll already be in the monitor's gamma. This should
		     * perform the inverse of OpenEXR's default monitor gamma
		     * correction. Alpha should always be linear, so leave
		     * that alone.
		     */
		    bucket.r = pow(bucket.r * 3.012, 2.2) / 5.55555;
		    bucket.g = pow(bucket.g * 3.012, 2.2) / 5.55555;
		    bucket.b = pow(bucket.b * 3.012, 2.2) / 5.55555;

		    /* Accumulate this bucket into our current pixel */
		    pixel.r += bucket.r;
		    pixel.g += bucket.g;
		    pixel.b += bucket.b;
		    pixel.a += bucket.a;

		    /* Next sample in this oversampling square */
		    cur_bucket_sample++;
		}

		/* Next line in the oversampling square */
		cur_bucket_row += histogram_stride;
	    }

	    /* Finish averaging over the oversampling square, and store this pixel */
	    cur_pixel->r = pixel.r / oversample_squared;
	    cur_pixel->g = pixel.g / oversample_squared;
	    cur_pixel->b = pixel.b / oversample_squared;
	    cur_pixel->a = pixel.a / oversample_squared;

	    /* Jump to the next pixel, and the beginning of the next block of buckets */
	    cur_pixel++;
	    cur_bucket += oversample;
	}

	/* Skip over all the histogram lines still part of the same row of buckets */
	cur_bucket += histogram_block_stride;
    }

    file.setFrameBuffer(pixels, 1, width);
    file.writePixels(height);

    delete[] pixels;
}

/* The End */

/*
 * exr.cpp - Optional OpenEXR extensions to the HistogramImager,
 *           for saving high dynamic range images from it.
 *
 * Fyre - rendering and interactive exploration of chaotic functions
 * Copyright (C) 2004 David Trowbridge and Micah Dowty
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

#include <ImfRgbaFile.h>

extern "C" {
#include "histogram-imager.h"
}

using namespace Imf;


extern "C" void exr_save_image_file(HistogramImager *hi, const gchar* filename)
{
  int width = hi->width;
  int height = hi->height;
  RgbaOutputFile file (filename, width, height);
  Rgba pixels[width * height];
  Rgba *cur_pixel = pixels;
  const guint oversample = hi->oversample;
  float fscale = histogram_imager_get_pixel_scale(hi);
  float one_over_gamma = 1.0 / hi->gamma;
  guint* cur_bucket = hi->histogram;

  struct {
    float r,g,b,a;
  } bg, fg, range;

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

  /* FIXME: implement oversampling support */

  /* A much simpler loop to use when oversampling is disabled */
  for (int y = height; y; y--) {
    for (int x = width; x; x--) {
      /* Linear exposure plus gamma adjustment */
      float luma = (*cur_bucket) * fscale;
      luma = pow(luma, one_over_gamma);

      /* Optionally clamp before interpolating */
      if (hi->clamped && luma > 1)
	luma = 1;

      /* Color interpolation, with no per-component clamping */
      cur_pixel->r = luma * range.r + bg.r;
      cur_pixel->g = luma * range.g + bg.g;
      cur_pixel->b = luma * range.b + bg.b;
      cur_pixel->a = luma * range.a + bg.a;

      /* Fyre images are generally authored to look good in sRGB,
       * so they'll already be in the monitor's gamma. This should
       * perform the inverse of OpenEXR's default monitor gamma
       * correction. Alpha should always be linear, so leave
       * that alone.
       */
      cur_pixel->r = pow(cur_pixel->r * 3.012, 2.2) / 5.55555;
      cur_pixel->g = pow(cur_pixel->g * 3.012, 2.2) / 5.55555;
      cur_pixel->b = pow(cur_pixel->b * 3.012, 2.2) / 5.55555;

      cur_pixel++;
      cur_bucket++;
    }
  }

  file.setFrameBuffer(pixels, 1, width);
  file.writePixels(height);
}

/* The End */

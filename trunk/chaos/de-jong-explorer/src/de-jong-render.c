/*
 * de-jong-render.c - Image calculation and rendering
 *
 * de Jong Explorer - interactive exploration of the Peter de Jong attractor
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

#include "de-jong.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

static void de_jong_check_dirty_flags(DeJong *self);
static void de_jong_reset_calc(DeJong *self);
static void de_jong_require_histogram(DeJong *self);
static void de_jong_require_image(DeJong *self);
static void de_jong_resize_color_table(DeJong *self, gulong minimum_entries);
static void de_jong_generate_color_table(DeJong *self);
static float de_jong_get_pixel_scale(DeJong *self);
static gulong de_jong_get_max_usable_density(DeJong *self);

static float uniform_variate();
static float normal_variate();
static int find_upper_pow2(int x);


/************************************************************************************/
/********************************************************************** Calculation */
/************************************************************************************/

void de_jong_calculate(DeJong *self, guint iterations) {
  de_jong_check_dirty_flags(self);
  de_jong_require_histogram(self);

  {
    /* Copy frequently used values to local vars */
    guint* const histogram = self->histogram;
    const gboolean tileable = self->tileable;
    const double a = self->a;
    const double b = self->b;
    const double c = self->c;
    const double d = self->d;

    /* Constants used in mapping rendering coordinates to image coordinates */
    const int count_width = self->width * self->oversample;
    const int count_height = self->height * self->oversample;
    const double scale = count_width / 5.0 * self->zoom;
    const double xcenter = count_width / 2.0 + self->xoffset * scale;
    const double ycenter = count_height / 2.0 + self->yoffset * scale;

    /* Toggles to disable features that aren't needed */
    const gboolean rotation_enabled = self->rotation > 0.0001 || self->rotation < -0.0001;
    const gboolean blur_enabled = self->blur_ratio > 0.0001 && self->blur_radius > 0.00001;

    /* Blurring variables */
    int blur_table_size;
    int blur_ratio_period;
    int blur_index, blur_ratio_index, blur_ratio_threshold;
    float *blur_table;

    /* Rotation variables */
    double sine_rotation, cosine_rotation;

    double x, y, point_x, point_y;
    gulong density, bucket;
    int i, ix, iy;
    guint *p;

    /* Precalculate the sine and cosine of the rotation angle, if we'll need it */
    if (rotation_enabled) {
      sine_rotation = sin(self->rotation);
      cosine_rotation = cos(self->rotation);
    }

    /* Initialize the blur table with a set of precalculated normally distributed
     * random numbers. Larger blur tables just increase the independence between
     * blocks of iterations. Since a new blur table is calculated at each run_iterations,
     * at infinity the image still has the same effect, but each iteration runs much faster.
     */
    if (blur_enabled) {
      /* Find a good size for the blur table. Our current heuristic finds
       * the smallest power of two that's still larger than 1/50 our iteration count.
       */
      blur_table_size = find_upper_pow2(iterations / 50);

      /* Allocate and fill the blur table */
      blur_table = alloca(blur_table_size * sizeof(blur_table[0]));
      for (i=0; i<blur_table_size; i++)
	blur_table[i] = normal_variate() * self->blur_radius;
      blur_index = 0;

      /* Initialize the blur ratio counter and threshold */
      blur_ratio_index = 0;
      blur_ratio_period = 1024;
      blur_ratio_threshold = self->blur_ratio * blur_ratio_period;
    }

    point_x = self->point_x;
    point_y = self->point_y;
    density = self->current_density;

    for(i=iterations; i; --i) {
      /* These are the actual Peter de Jong map equations. The new point value
       * gets stored into 'point', then we go on and mess with x and y before plotting.
       */
      x = sin(a * point_y) - cos(b * point_x);
      y = sin(c * point_x) - cos(d * point_y);
      point_x = x;
      point_y = y;

      /* If rotation is enabled, rotate each point around
       * the origin by self->rotation radians.
       */
      if (rotation_enabled) {
	x =  cosine_rotation * point_x + sine_rotation   * point_y;
	y = -sine_rotation   * point_x + cosine_rotation * point_y;
      }

      /* If blurring is enabled, use blur_ratio to decide how often to perturb
       * the apparent point position, and blur_radius to determine how much.
       * By perturbing the point using a normal variate, we create a true gaussian
       * blur as the number of iterations approaches infinity.
       */
      if (blur_enabled) {
	if (blur_ratio_index < blur_ratio_threshold) {
	  x += blur_table[blur_index];
	  blur_index = (blur_index+1) & (blur_table_size-1);
	  y += blur_table[blur_index];
	  blur_index = (blur_index+1) & (blur_table_size-1);
	}
	blur_ratio_index = (blur_ratio_index+1) & (blur_ratio_period-1);
      }

      /* Scale and translate our (x,y) coordinates into pixel coordinates */
      x = x * scale + xcenter;
      y = y * scale + ycenter;

      /* Convert (x,y) to integers.
       * Note that just casting to int here is incorrect! We want the behaviour
       * of floor(), always rounding toward -inf rather than zero. This should
       * behave identically to floor(), but a little faster.
       */
      if (x<0)
	ix = x-1;
      else
	ix = x;
      if (y<0)
	iy = y-1;
      else
	iy = y;

      if (tileable) {
	/* In tileable rendering, we wrap at the edges */
	ix %= count_width;
	iy %= count_height;
	if (ix < 0) ix += count_width;
	if (iy < 0) iy += count_height;
      }
      else {
	/* Otherwise, clip off the edges.
	 * Cast ix and iy to unsigned so our comparison against
	 * the width/height also implicitly compares against zero.
	 */
	if (((unsigned int)ix) >= count_width  ||
	    ((unsigned int)iy) >= count_height)
	  continue;
      }

      /* Plot our point in the counts array, updating the peak density */
      p = histogram + ix + count_width * iy;
      bucket = *p = *p + 1;
      if (bucket > density)
	density = bucket;
    }

    self->iterations += iterations;
    self->point_x = point_x;
    self->point_y = point_y;
    self->current_density = density;
  }
}

void       de_jong_calculate_motion(DeJong             *self,
				    guint               iterations,
				    gboolean            continuation,
				    DeJongInterpolator *interp,
				    gpointer            interp_data) {
  /* The equivalent of de_jong_calculate, but for an object moving according
   * to a given interpolation function.
   *
   * The given number of iterations are divided into smaller blocks, each of
   * which are run with different random coordinates between a and b. This gives
   * us accurate motion blur almost for free. Since the existing interpolated
   * parameters of this DeJong object are ignored, the 'continuation' flag must be
   * set on all but the first call for rendering to be reset properly.
   */
  const guint blocksize = iterations / 10;
  guint count;

  for (count=0; count<iterations; count+=blocksize) {
    interp(self, uniform_variate(), interp_data);
    self->calc_dirty_flag = !continuation;
    de_jong_calculate(self, blocksize);
  }
}


/************************************************************************************/
/****************************************************************** Image Rendering */
/************************************************************************************/

void de_jong_update_image(DeJong *self) {
  /* Convert our histogram counts to an 8-bit ARGB image data using our color lookup table,
   * downsampling by combining all count buckets that represent each of our output pixels.
   */
  de_jong_check_dirty_flags(self);
  de_jong_require_histogram(self);
  de_jong_require_image(self);
  de_jong_generate_color_table(self);

  {
    guint32 *pixel_p;
    guint32* const color_table = self->color_table;
    guint *count_p, *sample_p;
    guint count, count_clamp;
    const guint oversample = self->oversample;
    int x, y;

    pixel_p = (guint32*) gdk_pixbuf_get_pixels(self->image);
    count_p = self->histogram;

    /* Clamp count values to the size of our color table.
     * Assuming the color table generator did it's job
     * correctly, any count values higher than the maximum
     * one in the table would generate the same color as
     * the highest one.
     */
    count_clamp = self->color_table_filled_size - 1;

    if (oversample > 1) {
      /* Nice ugly loop that downsamples multiple (oversample^2)
       * histogram buckets to each pixel
       */

      const int oversample_squared = oversample * oversample;
      const int sample_stride = (self->width * oversample) - oversample;
      const int sample_y_stride = (self->width * oversample) * (oversample - 1);
      int sample_x, sample_y;
      int ch0, ch1, ch2, ch3;
      union {
	guint32 word;
	struct {
	  guchar ch0, ch1, ch2, ch3;
	} channels;
      } sample_pixel;

      for (y=self->height; y; y--) {
	for (x=self->width; x; x--) {

	  /* Convert each oversampled input point to a color separately, then
	   * average the resulting colors using the ch0 through ch3 channel
	   * accumulators. Note that which channel is which depends on the
	   * machine's endianness, so we can't name them red, green, blue,
	   * and alpha here. This can be though of as dividing each pixel into
	   * and oversample-by-oversample grid of squares and plotting one
	   * histogram bucket in each, with antialiasing.
	   */

	  ch0 = ch1 = ch2 = ch3 = 0;
	  sample_p = count_p;

	  for (sample_y=oversample; sample_y; sample_y--) {
	    for (sample_x=oversample; sample_x; sample_x--) {

	      count = *(count_p++);
	      if (count > count_clamp)
		sample_pixel.word = color_table[count_clamp];
	      else
		sample_pixel.word = color_table[count];

	      ch0 += sample_pixel.channels.ch0;
	      ch1 += sample_pixel.channels.ch1;
	      ch2 += sample_pixel.channels.ch2;
	      ch3 += sample_pixel.channels.ch3;
	    }
	    sample_p += sample_stride;
	  }
	  count_p += oversample;

	  sample_pixel.channels.ch0 = ch0 / oversample_squared;
	  sample_pixel.channels.ch1 = ch1 / oversample_squared;
	  sample_pixel.channels.ch2 = ch2 / oversample_squared;
	  sample_pixel.channels.ch3 = ch3 / oversample_squared;
	  *(pixel_p++) = sample_pixel.word;
	}
	count_p += sample_y_stride;
      }
    }
    else {
      /* A much simpler and faster loop to use when oversampling is disabled */

      for (y=self->height; y; y--) {
	for (x=self->width; x; x--) {
	  count = *(count_p++);
	  if (count > count_clamp)
	    *(pixel_p++) = color_table[count_clamp];
	  else
	    *(pixel_p++) = color_table[count];
	}
      }
    }

    self->render_dirty_flag = FALSE;
  }
}

static void de_jong_resize_color_table(DeJong *self, gulong size) {
  /* Resize the color table to exactly 'size' entries. Upon completion,
   * self->color_table_filled_size will equal 'size', and
   * self->color_table_allocated_size will be at least 'size'.
   * If the current table is too small, this reallocates one twice as
   * large as necessary, since the required size usually grows. However,
   * if the required size is less than 1/10 the current size, the table will
   * shrink to twice the current minimum size.
   */

  self->color_table_filled_size = size;

  /* Just to reduce allocation overhead during those crucial first few frames,
   * put a lower limit on the amount of memory we're going to allocate.
   */
  if (size < 1024)
    size = 1024;

  if ((self->color_table_allocated_size < size) ||
      (self->color_table_allocated_size > 10 * size)) {
    if (self->color_table)
      g_free(self->color_table);

    /* Allocate it to double the size we need now, as we expect our needs to grow. */
    self->color_table_allocated_size = size * 2;
    self->color_table = g_malloc(self->color_table_allocated_size * sizeof(self->color_table[0]));
  }
}

static float de_jong_get_pixel_scale(DeJong *self) {
  /* Calculate the scale factor for converting histogram counts to
   * luminance values between 0 and 1.
   */
  float density, fscale;

  /* Calculate the average histogram density, ignoring points that fall
   * outside our histogram due to clipping.
   */
  density = self->iterations / (self->width * self->height * self->oversample * self->oversample);

  /* fscale is a floating point number that, when multiplied by a raw
   * counts[] value, gives values between 0 and 1 corresponding to full
   * white and full black.
   */
  fscale = (self->exposure * self->zoom) / density;

  /* The very first frame we render will often be very underexposed.
   * If fscale > 0.5, this makes countsclamp negative and we get incorrect
   * results. The lowest usable value of countsclamp is 1.
   */
  if (fscale > 0.5)
    fscale = 0.5;

  return fscale;
}

static void de_jong_generate_color_table(DeJong *self) {
  /* Regenerate the contents of the color mapping table, a mapping from all
   * possible histogram values to the corresponding ARGB color, in the current image.
   */
  guint count;
  int r, g, b, a;
  float pixel_scale = de_jong_get_pixel_scale(self);
  gulong usable_density = de_jong_get_max_usable_density(self);
  float luma;
  double one_over_gamma = 1/self->gamma;

  /* Our actual color table size should be either the maximum
   * usable density, or our histogram's current peak density,
   * whichever is smaller.
   */
  if (usable_density > self->current_density)
    usable_density = self->current_density;

  /* Make sure our table is appropriately sized */
  de_jong_resize_color_table(self, usable_density + 1);

  /* Generate one color for every currently-possible count value that
   * doesn't fully saturate our image, as determined by de_jong_get_max_usable_density
   */
  for (count=0; count < self->color_table_filled_size; count++) {

    /* Scale and gamma-correct */
    luma = count * pixel_scale;
    luma = pow(luma, one_over_gamma);

    /* Optionally clamp before interpolating */
    if (self->clamped && luma > 1)
      luma = 1;

    /* Linearly interpolate between fgcolor and bgcolor */
    r = ((int)(self->bgcolor.red   * (1-luma) + self->fgcolor.red   * luma)) >> 8;
    g = ((int)(self->bgcolor.green * (1-luma) + self->fgcolor.green * luma)) >> 8;
    b = ((int)(self->bgcolor.blue  * (1-luma) + self->fgcolor.blue  * luma)) >> 8;
    a = ((int)(self->bgalpha       * (1-luma) + self->fgalpha       * luma)) >> 8;

    /* Always clamp color components */
    if (r<0) r = 0;  if (r>255) r = 255;
    if (g<0) g = 0;  if (g>255) g = 255;
    if (b<0) b = 0;  if (b>255) b = 255;
    if (a<0) a = 0;  if (a>255) a = 255;

    /* Colors are always ARGB order in little endian */
    self->color_table[count] = GUINT32_TO_LE( (a<<24) | (b<<16) | (g<<8) | r );
  }
}

static gulong de_jong_get_max_usable_density(DeJong *self) {
  /* This determines the highest histogram count value that will
   * produce any change in the color value of a pixel. This can be
   * used as an upper limit on the size of the color table. Note that
   * its value will change as the image is calculated, because it
   * depends on the pixel_scale.
   *
   * This function is basically the inverse of our color table's
   * function, mapping the most exposed color possible back to
   * a count value.
   */
  double max_luma, max_usable;

  if (self->clamped) {
    /* If clamping is on, the maximum useful luminance will always be 1. easy! */
    max_luma = 1;
  }
  else {
    /* But without clamping, it's possible for larger luminances to push
     * our color 'past' the foreground color. Envision a vector in 4 dimensional
     * RGBA space, pointing from background color to foreground color.
     *
     * This vector is represented by v_r, v_g, v_b, and v_a. We want to see how
     * far we can extend this vector until the resulting color is fully saturated.
     *
     * The value of each component in the fully saturated color depends on the sign
     * of the corresponding component in the vector. If it is zero, the resulting
     * color will always equal the background color, If it's negative, it will
     * eventually fall to 0, If it's positive, it will eventually reach 65535.
     *
     * Once we know what the fully saturated color is, we can easily use the
     * per-channel deltas to find out how much luminance it would take to
     * saturate each channel. The final max_luma is just the highest channel
     * saturation luminance.
     */
    int delta_r, delta_g, delta_b, delta_a;
    int clamped_r, clamped_g, clamped_b, clamped_a;
    double max_luma_r, max_luma_g, max_luma_b, max_luma_a;

    delta_r = self->fgcolor.red   - self->bgcolor.red;
    delta_g = self->fgcolor.green - self->bgcolor.green;
    delta_b = self->fgcolor.blue  - self->bgcolor.blue;
    delta_a = self->fgalpha       - self->bgalpha;

    if (delta_r > 0) clamped_r = 65535; else if (delta_r < 0) clamped_r = 0;
      else clamped_r = self->bgcolor.red;
    if (delta_g > 0) clamped_g = 65535; else if (delta_g < 0) clamped_g = 0;
      else clamped_g = self->bgcolor.green;
    if (delta_b > 0) clamped_b = 65535; else if (delta_b < 0) clamped_b = 0;
      else clamped_b = self->bgcolor.blue;
    if (delta_a > 0) clamped_a = 65535; else if (delta_a < 0) clamped_a = 0;
      else clamped_a = self->bgalpha;

    if (delta_r == 0) max_luma_r = 0;
      else max_luma_r = ((double)(clamped_r - self->bgcolor.red))   / delta_r;
    if (delta_g == 0) max_luma_g = 0;
      else max_luma_g = ((double)(clamped_g - self->bgcolor.green)) / delta_g;
    if (delta_b == 0) max_luma_b = 0;
      else max_luma_b = ((double)(clamped_b - self->bgcolor.blue))  / delta_b;
    if (delta_a == 0) max_luma_a = 0;
      else max_luma_a = ((double)(clamped_a - self->bgalpha))       / delta_a;

    max_luma = 0;
    if (max_luma_r > max_luma) max_luma = max_luma_r;
    if (max_luma_g > max_luma) max_luma = max_luma_g;
    if (max_luma_b > max_luma) max_luma = max_luma_b;
    if (max_luma_a > max_luma) max_luma = max_luma_a;
  }

  /* Put max_luma through the inverse of our color table's gamma operation */
  max_luma = pow(max_luma, self->gamma);

  /* And now we can finally get the count value by dividing out our pixel scale */
  max_usable = max_luma / de_jong_get_pixel_scale(self);

  /* For some input values, it's possible for this to generate really
   * truly gigantic numbers. We clamp these to something a bit more
   * reasonable, to avoid arithmetic overflow later on.
   */
  if (max_usable > G_MAXINT/2)
    max_usable = G_MAXINT/2;
  return (gulong) max_usable;
}


/************************************************************************************/
/************************************************************* Bifurcation Diagrams */
/************************************************************************************/

void de_jong_calculate_bifurcation(DeJong             *self,
				   DeJongInterpolator *interp,
				   gpointer            interp_data,
				   guint               iterations) {
  de_jong_check_dirty_flags(self);
  de_jong_require_histogram(self);

  {
    guint* const histogram = self->histogram;
    const int count_width = self->width * self->oversample;
    const int count_height = self->height * self->oversample;
    const gdouble y_min = -5;
    const gdouble y_max = 5;

    DeJong *interpolant;
    double x, y, a, b, c, d, alpha, point_x, point_y;
    gulong density, bucket;
    int i, block, ix, iy;
    guint *p;
    struct {
      double x, y;
    } *points;

    density = self->current_density;
    interpolant = de_jong_new();
    points = g_malloc(count_width * sizeof(points[0]));

    for (i=iterations; i;) {

      /* At each iteration block, we pick a new interpolated set of points
       * and the corresponding X coordinate on our histogram.
       */
      alpha = uniform_variate();
      interp(interpolant, alpha, interp_data);
      self->calc_dirty_flag = FALSE;
      ix = alpha * (count_width - 1);

      a = interpolant->a;
      b = interpolant->b;
      c = interpolant->c;
      d = interpolant->d;

      /* If we haven't seen this point recently, make a new random starting point */
      point_x = points[ix].x;
      point_y = points[ix].y;
      if (point_x == 0 && point_y == 0) {
	point_x = uniform_variate();
	point_y = uniform_variate();
      }

      for(block=1000; i && block; --i, --block) {
	/* These are the actual Peter de Jong map equations. The new point value
	 * gets stored into 'point', then we go on and mess with x and y before plotting.
	 */
	x = sin(a * point_y) - cos(b * point_x);
	y = sin(c * point_x) - cos(d * point_y);
	point_x = x;
	point_y = y;

	if (y >= y_min && y < y_max) {
	  iy = (int)( (y - y_min) / (y_max - y_min) * count_height );

	  /* Plot our point in the counts array, updating the peak density */
	  p = histogram + ix + count_width * iy;
	  bucket = *p = *p + 1;
	  if (bucket > density)
	    density = bucket;
	}

	self->iterations++;
      }

      points[ix].x = point_x;
      points[ix].y = point_y;
    }

    self->current_density = density;
    g_object_unref(interpolant);
    g_free(points);
  }
}


/************************************************************************************/
/************************************************************************ Utilities */
/************************************************************************************/

static void de_jong_reset_calc(DeJong *self) {
  /* Reset the histogram and calculation state */
  if (self->histogram)
    memset(self->histogram, 0, self->width * self->height *
	   self->oversample * self->oversample * sizeof(self->histogram[0]));
  self->current_density = 0;
  self->iterations = 0;

  /* Random starting point */
  self->point_x = uniform_variate();
  self->point_y = uniform_variate();

  self->calc_dirty_flag = FALSE;
}

static void de_jong_check_dirty_flags(DeJong *self) {
  /* Check dirty flags, invalidating stale data */

  if (self->size_dirty_flag) {
    /* We've resized. Deallocate the histogram and image
     * if they've been allocated, and set the render and
     * calc dirty flags.
     */
    if (self->histogram) {
      g_free(self->histogram);
      self->histogram = NULL;
    }
    if (self->image) {
      gdk_pixbuf_unref(self->image);
      self->image = NULL;
    }

    self->render_dirty_flag = TRUE;
    self->calc_dirty_flag = TRUE;
    self->size_dirty_flag = FALSE;
  }

  if (self->calc_dirty_flag) {
    /* We need to restart calculation. Clear the histogram and
     * rendering state, choose a random starting point.
     */
    de_jong_reset_calc(self);
  }
}

static void de_jong_require_histogram(DeJong *self) {
  /* Allocate a histogram if we don't have one already */
  if (!self->histogram) {
    self->histogram = g_malloc(sizeof(self->histogram[0]) *
			       self->width * self->height *
			       self->oversample * self->oversample);
    de_jong_reset_calc(self);
  }
}

static void de_jong_require_image(DeJong *self) {
  /* Allocate an image pixbuf if we don't have one already */
  if (!self->image) {
    self->image = gdk_pixbuf_new(GDK_COLORSPACE_RGB, TRUE, 8, self->width, self->height);
    self->render_dirty_flag = TRUE;
  }
}

static float uniform_variate() {
  /* A uniform random variate between 0 and 1 */
  return ((float) rand()) / RAND_MAX;
}

static float normal_variate() {
  /* A unit-normal random variate, implemented with the Box-Muller method */
  return sqrt(-2*log(uniform_variate())) * cos(uniform_variate() * (2*M_PI));
}

static int find_upper_pow2(int x) {
  /* Find the smallest power of two greater than or equal to x */
  int p = 1;
  while (p < x)
    p <<= 1;
  return p;
}

/* The End */

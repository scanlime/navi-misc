#include <ImfRgbaFile.h>

using namespace Imf;

extern "C" {
#include "histogram-imager.h"
}

#if 0
extern "C" void exr_write_histogram (HistogramImager *hi, const char* filename)
{
  int width = hi->width * hi->oversample;
  int height = hi->height * hi->oversample;
  RgbaOutputFile file (filename, width, height);

  Rgba pixels[width * height];
  Rgba *cur_pixel = pixels;
  guint* cur_bucket = hi->histogram;

  for (int y=height; y; y--) {
    for (int x=width; x; x--) {
      /* convert to an actual probability multiplied by a constant scale factor */
      cur_pixel->r = cur_pixel->g = cur_pixel->b = *cur_bucket / hi->total_points_plotted * 10000;
      cur_pixel->a = 1;

      cur_pixel++;
      cur_bucket++;
    }
  }

  file.setFrameBuffer(pixels, 1, width);
  file.writePixels(height);
}
#endif

/********* CRUFT BARRIER *********/
// #if 0
extern "C" void exr_write_histogram (HistogramImager *hi, const char* filename)
{
  int width = hi->width;
  int height = hi->height;
  RgbaOutputFile file (filename, width, height);
  Rgba pixels[width * height];
  Rgba *cur_pixel = pixels;
  const guint oversample = hi->oversample;
  float fscale = histogram_imager_get_pixel_scale(hi);
  float one_over_gamma = 1.0 / hi->gamma;
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

  if (oversample > 1)
  {
    guint* hist_p, sample_p;
  }
  else
  {
    /* A much simpler loop to use when oversampling is disabled */
    guint* cur_bucket = hi->histogram;
    for (int y = height; y; y--)
    {
      for (int x = width; x; x--)
      {
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
}

// #endif

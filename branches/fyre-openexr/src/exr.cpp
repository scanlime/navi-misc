#include <ImfRgbaFile.h>

using namespace Imf;

/* color interpolation component macro */
#define CIC(i) ((half(i)) / (half(65535)))

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
  gulong usable_density = histogram_imager_get_max_usable_density (hi);
  Rgba bg, fg, range;

  if (usable_density > hi->peak_density)
    usable_density = hi->peak_density;

  bg.r = CIC(hi->bgcolor.red); bg.g = CIC(hi->bgcolor.green); bg.b = CIC(hi->bgcolor.blue); bg.a = CIC(hi->bgalpha);
  fg.r = CIC(hi->fgcolor.red); fg.g = CIC(hi->fgcolor.green); fg.b = CIC(hi->fgcolor.blue); fg.a = CIC(hi->fgalpha);
  range.r = fg.r - bg.r;       range.g = fg.g - bg.g;         range.b = fg.b - bg.b;        range.a = fg.a - bg.a;

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
        cur_pixel->r = (half(*cur_bucket) / usable_density) * range.r + bg.r;
        cur_pixel->g = (half(*cur_bucket) / usable_density) * range.g + bg.g;
        cur_pixel->b = (half(*cur_bucket) / usable_density) * range.b + bg.b;
//        cur_pixel->a = (half(*cur_bucket) / usable_density) * range.a + bg.a;
        cur_pixel->a = 1;

        cur_pixel++;
        cur_bucket++;
      }
    }
    file.setFrameBuffer(pixels, 1, width);
    file.writePixels(height);
  }
}

// #endif

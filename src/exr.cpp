#include <ImfRgbaFile.h>

using namespace Imf;

extern "C" {
#include "histogram-imager.h"
}

extern "C" void exr_write_histogram(HistogramImager *hi, const char* filename)
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

/* Quick and dirty proof-of-concept type test thingy for
 * a different kind of bifurcation diagram. Two parameters
 * are mapped to the X and Y axes, pixel color and intensity
 * indicates the peak densities for an X and Y histogram.
 * Parameter values that produce fixed points and cycles will
 * be much more intense than pixels representing chaotic values.
 */

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define HISTOGRAM_SIZE 256
#define WIDTH          1600
#define HEIGHT         1200
#define ITERATIONS     10000

void calc(double a, double b, double c, double d, int *x_peak, int *y_peak) {
  double px, py, x, y;
  int x_buckets[HISTOGRAM_SIZE];
  int y_buckets[HISTOGRAM_SIZE];
  int i, xd, yd;

  px = ((double)random()) / RAND_MAX;
  py = ((double)random()) / RAND_MAX;

  memset(x_buckets, 0, sizeof(x_buckets));
  memset(y_buckets, 0, sizeof(y_buckets));
  *x_peak = 0;
  *y_peak = 0;

  for (i=0; i<ITERATIONS; i++) {
    x = sin(a * py) - cos(b * px);
    y = sin(c * px) - cos(d * py);
    px = x;
    py = y;
    xd = ++x_buckets[(int)(x * (HISTOGRAM_SIZE/4 - 1) + (HISTOGRAM_SIZE/2))];
    yd = ++y_buckets[(int)(y * (HISTOGRAM_SIZE/4 - 1) + (HISTOGRAM_SIZE/2))];
    if (xd > *x_peak) *x_peak = xd;
    if (xd > *y_peak) *y_peak = yd;
  }
}

int main(int argc, char **argv) {
  FILE *f = fopen("image.ppm", "wb");
  int i, j;
  int x_peak, y_peak;
  unsigned char rgb[3];

  fprintf(f, "P6\n%d %d\n255\n", WIDTH, HEIGHT);

  for (j=0;j<HEIGHT;j++) {
    printf("row %d of %d\n", j+1, HEIGHT);

    for (i=0;i<WIDTH;i++) {

      calc((i * 2.0 / WIDTH  - 1) * 10.0,
	   (j * 2.0 / HEIGHT - 1) * 10.0,
	   2.42754, -2.17719, &x_peak, &y_peak);

      rgb[0] = 255 - (log(x_peak) * 255 / log(ITERATIONS));
      rgb[1] = 255 - (log(y_peak) * 255 / log(ITERATIONS));
      rgb[2] = (((int)rgb[0]) + rgb[1])/2;

      fwrite(rgb, 1, 3, f);
    }
  }
  return 0;
}


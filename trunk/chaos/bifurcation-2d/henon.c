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
#define WIDTH          1200
#define HEIGHT         1200
#define ITERATIONS     5000

double bigx, bigy;
double smallx, smally;

void calc(double a, double b, int *x_peak, int *y_peak) {
  double px, py, x, y;
  int x_buckets[HISTOGRAM_SIZE];
  int y_buckets[HISTOGRAM_SIZE];
  int i, xd = 0, yd = 0, xp, yp;

  px = 0.2;
  py = 0.2;

  memset(x_buckets, 0, sizeof(x_buckets));
  memset(y_buckets, 0, sizeof(y_buckets));
  *x_peak = 0;
  *y_peak = 0;

  for (i=0; i<ITERATIONS; i++) {
    x = py + 1 - a*px*px;
    y = b*px;
    px = x;
    py = y;
    xp = (int)(x * (HISTOGRAM_SIZE/180.0 - 1) + (HISTOGRAM_SIZE/90.0));
    yp = (int)(y * (HISTOGRAM_SIZE/180.0 - 1) + (HISTOGRAM_SIZE/90.0));
    if(px < smallx)
      smallx = px;
    if(px > bigx)
      bigx = px;
    if(py < smally)
      smally = py;
    if(py > bigy)
      bigy = py;
    if(xp < HISTOGRAM_SIZE && xp >= 0)
      xd = ++x_buckets[xp];
    if(yp < HISTOGRAM_SIZE && yp >= 0)
      yd = ++y_buckets[yp];
    if (xd > *x_peak) *x_peak = xd;
    if (xd > *y_peak) *y_peak = yd;
  }
}

int main(int argc, char **argv) {
  FILE *f = fopen("image.ppm", "wb");
  int i, j;
  int x_peak = 0, y_peak = 0;
  unsigned char rgb[3];

  bigx = bigy = smallx = smally = 0;

  fprintf(f, "P6\n%d %d\n255\n", WIDTH, HEIGHT);
  fflush(f);

  for (j=0;j<HEIGHT;j++) {
    printf("row %d of %d\n", j+1, HEIGHT);

    for (i=0;i<WIDTH;i++) {

/*      calc((i * 2.0 / WIDTH  - 1) * 1.0,
	   (j * 2.0 / HEIGHT - 1) * 1.0,
	   &x_peak, &y_peak);*/
	calc(((double)i * 2.0 / WIDTH), ((double)j * 2.0 / HEIGHT) - 1.0, &x_peak, &y_peak);

      rgb[0] = 255 - (x_peak * 255 / ITERATIONS);
      rgb[1] = 255 - (y_peak * 255 / ITERATIONS);
      rgb[2] = (((int)rgb[0]) + rgb[1])/2;

      fwrite(rgb, 3, 1, f);
    }
    if(j % 50 == 0)
      fflush(f);
  }
  fclose(f);
  printf("min,max: (%f,%f), (%f,%f)\n", smallx, smally, bigx, bigy);

  return 0;
}


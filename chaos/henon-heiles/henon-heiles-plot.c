#include "histogram-imager.h"
#include "runge-kutta.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define INDEX(w,lb,hb,p) (((hb)-(lb))/(w)*(p) + (lb))
#define ENERGY 0.8333

/* axes:
 * x:  [-0.5, 1.0]
 * Px: [-0.5, 0.5]
 */

HistogramPlot plot;

void ode (float *last, float *deriv, float time)
{
  float x = last[0];
  float y = last[1];
  deriv[0] = last[2];
  deriv[1] = last[3];
  deriv[2] = -x - 2*x*y;
  deriv[3] = y*y - y - x*x;
  deriv[4] = 1;
}

void py(float *point)
{
  float x = point[0];
  float y = point[1];
  float px = point[2];
  point[3] = sqrt ((-3*(3*x*x*(2*y+1) - 2*y*y*y + 3*y*y - 3*(2*ENERGY - px*px))) / 3) / 10;
}

int main (int argc, char **argv)
{
  float point1[5], point2[5];
  float *a, *b, *c;
  float t = 0, tdelt = 0.001;
  int i;

  a = point1;
  b = point2;

  /*
  point1[0] = -0.4;
  point1[1] = 0;
  point1[2] = 0;
  point1[3] = 0.081649658093;
  point1[4] = 0;
  */

  point1[0] = 0.4;
  point1[1] = 0;
  point1[2] = 0;
  py(point1);
  fprintf (stderr, "py is %f\n", point1[3]);
  point1[4] = 0;

  for (i = 0; i < 50000000; i++)
//  for (i = 0; i < 500000; i++)
  {
    rk (ode, a, b, 5, t, tdelt);
    if (a[0] < 0 && b[0] > 0)
    {
      float dy, dy0;
      float q[5];

      /* gross mess to do a linear projection to
         land a point on the plane of section */
      dy = b[0] - a[0];
      dy0 = 0 - a[0];
      vector_subtract (b, a, q, 5);
      vector_normalize (q, q, 5);
      vector_scale (q, dy / dy0, q, 5);
      vector_add (a, q, q, 5);
//      printf ("%f %f\n", q[1], q[3]);
      printf ("%f %f\n", a[1], a[3]);
    }
//    printf ("%f %f\n", a[4], a[0]);
    t += tdelt;

    /* swap */
    c = a;
    a = b;
    b = c;
    if (i % 500000 == 0)
    {
      fprintf (stderr, "%d iterations\n", i);
      fflush (stdout);
    }
  }
}

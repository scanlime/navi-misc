#include "histogram-imager.h"
#include "runge-kutta.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

// #define INDEX(w,lb,hb,p) (((hb)-(lb))/(w)*(p) + (lb))
#define INDEX(length, min, max, value)   (((value)-(min))*(length)/((max)-(min)))
// #define ENERGY 0.08333
#define ENERGY 0.125

/* axes:
 * x:  [-0.5, 1.0]
 * Px: [-0.5, 0.5]
 */

HistogramPlot plot;
HistogramImager *hi;
int w, h;

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
  point[3] = sqrt (-1*(x*x + y*y + 2.0*x*x*y - (2.0/3)*y*y*y + px*px - 2*ENERGY));
}

void hhrun (float *point)
{
  float point1[5], point2[5];
  float *a, *b, *c;
  float t = 0, tdelt = 0.001;
  int i, xi, yi;

  memcpy (point1, point, 5 * sizeof (float));

  a = point1;
  b = point2;

  for (i = 0; i < 50000000; i++)
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
//      printf ("%f %f\n", a[1], a[3]);
      xi = INDEX (w, -0.6, 0.8, a[1]);
      yi = INDEX (h, -0.5, 0.5, a[3]);
      HISTOGRAM_IMAGER_PLOT (plot, xi, yi);
    }
    t += tdelt;

    /* swap */
    c = a;
    a = b;
    b = c;
    if (i % 1000000 == 0)
      fprintf (stderr, ".");
  }
  fprintf (stderr, "\n");
}

int main (int argc, char **argv)
{
  int i;
  float point[5];

  g_type_init ();
  hi = histogram_imager_new ();
  g_object_set (G_OBJECT (hi), "exposure", 0.10, "gamma", 2.0, NULL);
  g_object_set (G_OBJECT (hi), "width", 800, "height", 600);

  srand (time (NULL));

  histogram_imager_get_hist_size (hi, &w, &h);
  histogram_imager_prepare_plots (hi, &plot);

  point[1] = 0;
  point[2] = 0;
  point[4] = 0;

  for (i = 0; i < 50; i++)
  {
    point[0] = (rand () * 0.6) / RAND_MAX - 0.3;
    point[1] = (rand () * 0.5) / RAND_MAX - 0.25;
    py (point);
    fprintf (stderr, "running integration for point (%f, %f, %f, %f)\n    ", point[0], point[1], point[2], point[3]);
    hhrun (point);

    point[0] = -point[0];
    hhrun (point);
    histogram_imager_finish_plots (hi, &plot);
    histogram_imager_save_image_file (hi, "test.png");
    if (i % 5 == 0)
      fprintf (stderr, "  %d trajectories plotted so far\n");
  }
  fprintf (stderr, "  50 trajectories finished\n");

  histogram_imager_finish_plots (hi, &plot);
  histogram_imager_save_image_file (hi, "test.png");
}

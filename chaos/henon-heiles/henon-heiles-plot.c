#include "histogram-imager.h"
#include "runge-kutta.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define INDEX(length, min, max, value)   (((value)-(min))*(length)/((max)-(min)))
// #define ENERGY 0.08333
#define ENERGY 0.125
#define POINTS 1000

/* axes:
 * x:  [-0.5, 1.0]
 * Px: [-0.5, 0.5]
 */

HistogramPlot plot;
HistogramImager *hi;
int w, h;
long long iterations;

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

void py (float *point)
{
	float x = point[0];
	float y = point[1];
	float px = point[2];
	point[3] = sqrt (-1*(x*x + y*y + 2.0*x*x*y - (2.0/3)*y*y*y + px*px - 2*ENERGY));
}

long hhrun (float *point, float *pointret)
{
	float point1[5], point2[5];
	float *a, *b, *c;
	float t = 0, tdelt = 0.001;
	int xi, yi;
	long i;
	int points = 0;
	float *scratch = g_new0 (float, 5 * 6);

	memcpy (point1, point, 5 * sizeof (float));

	a = point1;
	b = point2;

	for (i = 0;; i++) {
		rk (ode, a, b, 5, t, tdelt, scratch);
		if (a[0] < 0 && b[0] > 0) {
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
			xi = INDEX (w, -0.6, 0.8, a[1]);
			yi = INDEX (h, -0.5, 0.5, a[3]);
			HISTOGRAM_IMAGER_PLOT (plot, xi, yi);
			points++;
			if (points % 100 == 0)
				fprintf (stderr, ".");
		}
		t += tdelt;

		/* swap */
		c = a;
		a = b;
		b = c;
		if (points > POINTS)
			break;
	}
	fprintf (stderr, "\n");

	g_free (scratch);

	memcpy (pointret, a, 5 * sizeof (float));

	iterations += i;
	return POINTS;
}

int main (int argc, char **argv)
{
	int i;
	float point[5], scratch[5];
	gchar *filename, *pstring;
	long points;

	g_type_init ();
	hi = histogram_imager_new ();
	g_object_set (G_OBJECT (hi), "exposure", 0.05, "gamma", 1.0, NULL);
	g_object_set (G_OBJECT (hi), "width", 800, "height", 600, NULL);

	srand (time (NULL));

	points = 0;

	if (argc >= 3) {
		point[0] = atof (argv[1]);
		point[1] = atof (argv[2]);
	} else {
		point[0] = (((float) rand ()) / RAND_MAX) * 0.6 - 0.3;
		point[1] = (((float) rand ()) / RAND_MAX) * 0.5 - 0.25;
	}
	point[2] = 0;
	point[3] = 0;
	point[4] = 0;

	py (point);

	histogram_imager_get_hist_size (hi, &w, &h);
	pstring = g_strdup_printf ("%f-%f-%f-%f.png", point[0], point[1], point[2], point[3]);

	fprintf (stderr, "running integration for point (%f, %f, %f, %f)\n", point[0], point[1], point[2], point[3]);
	while (1) {
		histogram_imager_prepare_plots (hi, &plot);

		points += hhrun (point, scratch);
		point[0] = -point[0];
		hhrun (point, point);

		histogram_imager_finish_plots (hi, &plot);
		filename = g_strdup_printf ("%s--%.5d.png", pstring, points);
		histogram_imager_save_image_file (hi, filename);
		g_free (filename);
		g_print ("Completed %lld iterations, %d points plotted\n", iterations, points);
	}
}

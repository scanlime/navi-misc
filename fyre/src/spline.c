/* GTK - The GIMP Toolkit
 * Copyright (C) 1997 David Mosberger
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/*
 * Modified by the GTK+ Team and others 1997-2000.  See the AUTHORS
 * file for a list of people on the GTK+ Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GTK+ at ftp://ftp.gtk.org/pub/gtk/.
 */

/* These spline solving and evaluation functions were separated from GtkCurve.
 * See the CurveEditor widget for more information.
 */

#include <string.h>
#include "spline.h"

static SplineControlPoint template_linear_points[] = {
  {0, 0},
  {1, 1},
};

static SplineControlPoint template_smooth_points[] = {
  {0,     0   },
  {0.375, 0.25},
  {0.625, 0.75},
  {1,     1   },
};

const Spline spline_template_linear = {template_linear_points, sizeof(template_linear_points) / sizeof(SplineControlPoint)};
const Spline spline_template_smooth = {template_smooth_points, sizeof(template_smooth_points) / sizeof(SplineControlPoint)};



GType spline_get_type(void) {
  static GType spline_type = 0;
  if (!spline_type)
    spline_type = g_boxed_type_register_static("Spline",
					       (GBoxedCopyFunc) spline_copy,
					       (GBoxedFreeFunc) spline_free);
  return spline_type;
}

Spline* spline_copy(const Spline *spline) {
  Spline *n = g_malloc(sizeof(Spline));
  n->num_points = spline->num_points;
  n->points = g_malloc(spline->num_points * sizeof(SplineControlPoint));
  memcpy(n->points, spline->points, spline->num_points * sizeof(SplineControlPoint));
  return n;
}

void spline_free(Spline *spline) {
  g_free(spline->points);
  g_free(spline);
}

guchar* spline_serialize(Spline *spline, gsize *size) {
  guchar *buffer;
  *size = spline->num_points * sizeof(SplineControlPoint);
  buffer = g_malloc(*size);
  memcpy(buffer, spline->points, *size);
  return buffer;
}

Spline* spline_unserialize(const guchar *data, gsize size) {
  Spline *n = g_malloc(sizeof(Spline));
  n->num_points = size / sizeof(SplineControlPoint);
  n->points = g_malloc(n->num_points * sizeof(SplineControlPoint));
  memcpy(n->points, (void *) data, n->num_points * sizeof(SplineControlPoint));
  return n;
}

/* Solve the tridiagonal equation system that determines the second
   derivatives for the interpolation points.  (Based on Numerical
   Recipies 2nd Edition.) */
void    spline_solve(Spline *spline, gfloat y2[]) {
  gfloat p, sig, *u;
  gint i, k;

  u = g_malloc ((spline->num_points - 1) * sizeof (u[0]));

  y2[0] = u[0] = 0.0;	/* set lower boundary condition to "natural" */

  for (i = 1; i < spline->num_points - 1; ++i)
    {
      sig = ((spline->points[i][0] - spline->points[i - 1][0]) /
	     (spline->points[i + 1][0] - spline->points[i - 1][0]));
      p = sig * y2[i - 1] + 2.0;
      y2[i] = (sig - 1.0) / p;
      u[i] = ((spline->points[i + 1][1] - spline->points[i][1])
	      / (spline->points[i + 1][0] - spline->points[i][0]) -
	      (spline->points[i][1] - spline->points[i - 1][1]) /
	      (spline->points[i][0] - spline->points[i - 1][0]));
      u[i] = (6.0 * u[i] / (spline->points[i + 1][0] - spline->points[i - 1][0]) - sig * u[i - 1]) / p;
    }

  y2[spline->num_points - 1] = 0.0;
  for (k = spline->num_points - 2; k >= 0; --k)
    y2[k] = y2[k] * y2[k + 1] + u[k];

  g_free (u);
}

gfloat spline_eval(Spline *spline, gfloat y2[], gfloat val) {
  gint k_lo, k_hi, k;
  gfloat h, b, a;

  /* do a binary search for the right interval: */
  k_lo = 0; k_hi = spline->num_points - 1;
  while (k_hi - k_lo > 1)
    {
      k = (k_hi + k_lo) / 2;
      if (spline->points[k][0] > val)
	k_hi = k;
      else
	k_lo = k;
    }

  h = spline->points[k_hi][0] - spline->points[k_lo][0];
  g_assert (h > 0.0);

  a = (spline->points[k_hi][0] - val) / h;
  b = (val - spline->points[k_lo][0]) / h;
  return a*spline->points[k_lo][1] + b*spline->points[k_hi][1] +
    ((a*a*a - a)*y2[k_lo] + (b*b*b - b)*y2[k_hi]) * (h*h)/6.0;
}

gfloat  spline_solve_and_eval(Spline *spline, gfloat val) {
  /* Solve a spline and evaluate one point from it. This is
   * the most convenient way to interpolate given a set of control
   * points saved from a CurveEditor widget. Note that this
   * assumes all control points are active.
   */
  gfloat *y2v, result;

  y2v = g_malloc (spline->num_points * sizeof (gfloat));

  spline_solve(spline, y2v);
  result = spline_eval(spline, y2v, val);

  if (result < 0) result = 0;
  if (result > 1) result = 1;

  g_free (y2v);
  return result;
}

Spline* spline_find_active_points(Spline *spline) {
  /* Return a new spline with only the active points from the first one
   */
  gfloat prev, ry;
  gint dst, i, first_active = -1;
  Spline *active = g_malloc(sizeof(Spline));

  /* count active points: */
  prev = -1;
  for (i = active->num_points = 0; i < spline->num_points; ++i)
    if (spline->points[i][0] > prev)
      {
	if (first_active < 0)
	  first_active = i;
	prev = spline->points[i][0];
	++active->num_points;
      }

  /* handle degenerate case.
   * If we have less than two points, create a second false
   * point with the same Y coordinate so we get a horizontal line.
   */
  if (active->num_points < 2)
    {
      if (active->num_points > 0)
	ry = spline->points[first_active][1];
      else
	ry = 0;
      if (ry < 0) ry = 0;
      if (ry > 1) ry = 1;

      active->num_points = 2;
      active->points = g_malloc(active->num_points * sizeof(SplineControlPoint));

      active->points[0][0] = 0;
      active->points[0][1] = ry;

      active->points[1][0] = 1;
      active->points[1][1] = ry;
    }

  else
    {
      active->points = g_malloc(active->num_points * sizeof(SplineControlPoint));

      prev = -1;
      for (i = dst = 0; i < spline->num_points; ++i)
	if (spline->points[i][0] > prev)
	  {
	    prev    = spline->points[i][0];
	    active->points[dst][0] = spline->points[i][0];
	    active->points[dst][1] = spline->points[i][1];
	    ++dst;
	  }
    }

  return active;
}

void spline_solve_and_eval_range(Spline *spline,
				 int     veclen,
				 gfloat  vector[],
				 float   lower,
				 float   upper) {
  gfloat *y2v, result;
  int i;
  float x, dx;

  y2v = g_malloc (spline->num_points * sizeof (gfloat));

  spline_solve(spline, y2v);

  x = lower;
  dx = (upper - lower) / (veclen - 1);

  for (i=0,x=0; i<veclen; i++,x+=dx)
    {
      result = spline_eval(spline, y2v, x);
      if (result < 0) result = 0;
      if (result > 1) result = 1;
      vector[i] = result;
    }

  g_free (y2v);
}

void    spline_solve_and_eval_all(Spline *spline,
				  int     veclen,
				  gfloat  vector[])
{
  spline_solve_and_eval_range(spline, veclen, vector, 0, 1);
}

/* The End */

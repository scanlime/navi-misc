/*
 * bifurcation-diagram.c - A HistogramImager that renders a bifurcation diagram
 *                         in which one axis interpolates across de Jong parameters
 *                         and the other axis shows a 1 dimensional projection of the
 *                         image at those parameters.
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

#include "bifurcation-diagram.h"
#include <stdlib.h>

static void bifurcation_diagram_class_init(BifurcationDiagramClass *klass);
static void bifurcation_diagram_init(BifurcationDiagram *self);
static void bifurcation_diagram_dispose(GObject *gobject);


/************************************************************************************/
/**************************************************** Initialization / Finalization */
/************************************************************************************/

GType bifurcation_diagram_get_type(void) {
  static GType dj_type = 0;

  if (!dj_type) {
    static const GTypeInfo dj_info = {
      sizeof(BifurcationDiagramClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) bifurcation_diagram_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(BifurcationDiagram),
      0,
      (GInstanceInitFunc) bifurcation_diagram_init,
    };

    dj_type = g_type_register_static(HISTOGRAM_IMAGER_TYPE, "BifurcationDiagram", &dj_info, 0);
  }

  return dj_type;
}

static void bifurcation_diagram_class_init(BifurcationDiagramClass *klass) {
  GObjectClass *object_class;
  object_class = (GObjectClass*) klass;

  object_class->dispose      = bifurcation_diagram_dispose;
}

static void bifurcation_diagram_init(BifurcationDiagram *self) {
  /* Nothing to do here yet, everything's set up by our G_PARAM_CONSTRUCT properties */
}

static void bifurcation_diagram_dispose(GObject *gobject) {
  BifurcationDiagram *self = BIFURCATION_DIAGRAM(gobject);
}

BifurcationDiagram* bifurcation_diagram_new() {
  return BIFURCATION_DIAGRAM(g_object_new(bifurcation_diagram_get_type(), NULL));
}


/************************************************************************************/
/********************************************************************** Calculation */
/************************************************************************************/

#if 0
void bifurcation_diagram_calculate(BifurcationDiagram    *self,
				   ParameterInterpolator *interp,
				   gpointer               interp_data,
				   guint                  iterations) {
  bifurcation_diagram_check_dirty_flags(self);
  bifurcation_diagram_require_histogram(self);

  {
    guint* const histogram = self->histogram;
    const int hist_width = self->width * self->oversample;
    const int hist_height = self->height * self->oversample;
    const gdouble y_min = -5;
    const gdouble y_max = 5;

    BifurcationColumn *column;
    double x, y, a, b, c, d, alpha, point_x, point_y;
    gulong density, bucket;
    int block, ix, iy, i, j;
    guint *p;
    BifurcationDiagram *interpolant;
    int tmp;
    interpolant = bifurcation_diagram_new();

    density = self->current_density;

    if (!self->columns) {
      /* Create columns and number them */
      self->columns = g_new0(BifurcationColumn, hist_width);
      for (i=0; i<hist_width; i++)
	self->columns[i].ix = i;

      /* Shuffle them, so we render in a seemingly-random order */
      for (i=hist_width-1; i>=0; i--) {
	j = ((int) random() % (i+1));
	tmp = self->columns[i].ix;
	self->columns[i].ix = self->columns[j].ix;
	self->columns[j].ix = tmp;
      }
    }

    for (i=iterations; i;) {

      /* Pick the next column, with a randomly chosen set of interpolated parameters */
      column = &self->columns[self->current_column++];
      if (self->current_column >= hist_width)
	self->current_column = 0;

      ix = column->ix;

      if (!column->initialized) {
	column->point_x = uniform_variate();
	column->point_y = uniform_variate();
	column->initialized = TRUE;
      }
      point_x = column->point_x;
      point_y = column->point_y;

      j = random() % (sizeof(self->columns[0].interpolated) / sizeof(self->columns[0].interpolated[0]));
      if (!column->interpolated[j].initialized) {
	interp(interpolant, (column->ix + uniform_variate()) / (hist_width - 1), interp_data);
	column->interpolated[j].a = interpolant->a;
	column->interpolated[j].b = interpolant->b;
	column->interpolated[j].c = interpolant->c;
	column->interpolated[j].d = interpolant->d;
	column->interpolated[j].initialized = TRUE;
      }
      a = column->interpolated[j].a;
      b = column->interpolated[j].b;
      c = column->interpolated[j].c;
      d = column->interpolated[j].d;

      for(block=50; i && block; --i, --block) {
	/* These are the actual Peter de Jong map equations. The new point value
	 * gets stored into 'point', then we go on and mess with x and y before plotting.
	 */
	x = sin(a * point_y) - cos(b * point_x);
	y = sin(c * point_x) - cos(d * point_y);
	point_x = x;
	point_y = y;

	if (y >= y_min && y < y_max) {
	  iy = (int)( (y - y_min) / (y_max - y_min) * hist_height );

	  /* Plot our point in the counts array, updating the peak density */
	  p = histogram + ix + hist_width * iy;
	  bucket = *p = *p + 1;
	  if (bucket > density)
	    density = bucket;

	  self->iterations++;
	}
      }

      /* Update the column */
      column->point_x = point_x;
      column->point_y = point_y;
    }

    self->current_density = density;
    g_object_unref(interpolant);
  }
}
#endif

/* The End */

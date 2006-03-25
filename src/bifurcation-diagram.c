/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * bifurcation-diagram.c - A HistogramImager that renders a bifurcation diagram
 *                         in which one axis interpolates across de Jong parameters
 *                         and the other axis shows a 1 dimensional projection of the
 *                         image at those parameters.
 *
 * Fyre - rendering and interactive exploration of chaotic functions
 * Copyright (C) 2004-2006 David Trowbridge and Micah Dowty
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

#include <string.h>
#include <math.h>
#include "bifurcation-diagram.h"
#include "math-util.h"

static void               bifurcation_diagram_class_init        (BifurcationDiagramClass *klass);
static void               bifurcation_diagram_init              (BifurcationDiagram      *self);
static void               bifurcation_diagram_dispose           (GObject                 *gobject);

static void               bifurcation_diagram_init_columns      (BifurcationDiagram      *self);
static BifurcationColumn* bifurcation_diagram_next_column       (BifurcationDiagram      *self);
static void               bifurcation_diagram_get_column_params (BifurcationDiagram      *self,
								 BifurcationColumn       *column,
								 DeJongParams            *param);

static gpointer parent_class = NULL;


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
    parent_class = g_type_class_ref(G_TYPE_OBJECT);
    object_class = (GObjectClass*) klass;

    object_class->dispose      = bifurcation_diagram_dispose;
}

static void bifurcation_diagram_init(BifurcationDiagram *self) {
}

static void bifurcation_diagram_dispose(GObject *gobject) {
    BifurcationDiagram *self = BIFURCATION_DIAGRAM(gobject);

    if (self->columns) {
	g_free(self->columns);
	self->columns = NULL;
    }

    if (self->interpolant) {
	g_object_unref(self->interpolant);
	self->interpolant = NULL;
    }

    if (self->interp_data && self->interp_data_free)
	self->interp_data_free(self->interp_data);
    self->interp_data = NULL;
    self->interp_data_free = NULL;

    G_OBJECT_CLASS(parent_class)->dispose(gobject);
}

BifurcationDiagram* bifurcation_diagram_new() {
    return BIFURCATION_DIAGRAM(g_object_new(bifurcation_diagram_get_type(), NULL));
}


/************************************************************************************/
/************************************************************************* Settings */
/************************************************************************************/

void bifurcation_diagram_set_interpolator (BifurcationDiagram    *self,
					   ParameterInterpolator *interp,
					   gpointer               interp_data,
					   GFreeFunc              interp_data_free) {
    /* Free the old interpolator data */
    if (self->interp_data && self->interp_data_free)
	self->interp_data_free(self->interp_data);

    self->interp = interp;
    self->interp_data = interp_data;
    self->interp_data_free = interp_data_free;

    self->calc_dirty_flag = TRUE;
}

void bifurcation_diagram_set_linear_endpoints (BifurcationDiagram     *self,
					       DeJong                 *first,
					       DeJong                 *second) {
    ParameterHolderPair *pair, *oldpair;

    /* If the old interpolator was also linear, see if we can avoid this update... */
    if (self->interp == PARAMETER_INTERPOLATOR(parameter_holder_interpolate_linear)) {
	oldpair = (ParameterHolderPair*) self->interp_data;

	if ((!memcmp(&DE_JONG(oldpair->a)->param, &first->param, sizeof(first->param))) &&
	    (!memcmp(&DE_JONG(oldpair->b)->param, &second->param, sizeof(second->param))))
	    return;
    }

    pair = g_new(ParameterHolderPair, 1);
    pair->a = g_object_ref(first);
    pair->b = g_object_ref(second);

    bifurcation_diagram_set_interpolator(self, PARAMETER_INTERPOLATOR(parameter_holder_interpolate_linear),
					 pair, (GFreeFunc) parameter_holder_pair_free);
}


/************************************************************************************/
/************************************************************************** Columns */
/************************************************************************************/

static void bifurcation_diagram_init_columns (BifurcationDiagram *self) {
    int hist_width;
    int i, tmp, j;

    /* Do we need to resize the column array? */
    histogram_imager_get_hist_size(HISTOGRAM_IMAGER(self), &hist_width, NULL);
    if (hist_width != self->num_columns) {

	if (self->columns)
	    g_free(self->columns);

	/* Create columns and number them */
	self->num_columns = hist_width;
	self->current_column = 0;
	self->columns = g_new0(BifurcationColumn, self->num_columns);
	for (i=0; i<self->num_columns; i++)
	    self->columns[i].ix = i;

	/* Shuffle them, so we render in a seemingly-random order */
	for (i=self->num_columns-1; i>=0; i--) {
	    j = int_variate(0, i+1);
	    tmp = self->columns[i].ix;
	    self->columns[i].ix = self->columns[j].ix;
	    self->columns[j].ix = tmp;
	}

	self->calc_dirty_flag = TRUE;
    }

    /* Should we be resetting the columns, either due to
     * a change in interpolants or a column array resize?
     */
    if (self->calc_dirty_flag || HISTOGRAM_IMAGER(self)->histogram_clear_flag) {
	/* Clear the histogram if it isn't already */
	if (!HISTOGRAM_IMAGER(self)->histogram_clear_flag)
	    histogram_imager_clear(HISTOGRAM_IMAGER(self));

	for (i=0; i<self->num_columns; i++) {
	    /* Invalidate each column and each interpolated parameter set */
	    self->columns[i].point.valid = FALSE;
	    for (j=0; j<(sizeof(self->columns[0].interpolated)/
			 sizeof(self->columns[0].interpolated[0])); j++) {
		self->columns[i].interpolated[j].valid = FALSE;
	    }
	}

	HISTOGRAM_IMAGER(self)->histogram_clear_flag = FALSE;
	self->calc_dirty_flag = FALSE;
    }
}

static BifurcationColumn* bifurcation_diagram_next_column (BifurcationDiagram *self) {
    /* Get the next column, wrapping around when we hit the end */
    BifurcationColumn *column = &self->columns[self->current_column];
    if (++self->current_column >= self->num_columns)
	self->current_column = 0;

    /* Initialize this column's point if it isn't yet */
    if (!column->point.valid) {
	column->point.x = uniform_variate();
	column->point.y = uniform_variate();
	column->point.valid = TRUE;
    }

    return column;
}

static void bifurcation_diagram_get_column_params (BifurcationDiagram *self,
						   BifurcationColumn  *column,
						   DeJongParams       *param) {
    /* Get a random parameter set from the given column, creating it if necessary */
    int interpIndex = int_variate(0, sizeof(self->columns[0].interpolated)/
				  sizeof(self->columns[0].interpolated[0]));

    if (!column->interpolated[interpIndex].valid) {

	/* Create an interpolant if we don't have one yet */
	if (!self->interpolant)
	    self->interpolant = de_jong_new();

	/* Pick a random place within the column to perform the interpolation */
	self->interp(PARAMETER_HOLDER(self->interpolant),
		     (column->ix + uniform_variate()) / (self->num_columns - 1),
		     self->interp_data);
	column->interpolated[interpIndex].param = self->interpolant->param;

	column->interpolated[interpIndex].valid = TRUE;
    }

    *param = column->interpolated[interpIndex].param;
}


/************************************************************************************/
/********************************************************************** Calculation */
/************************************************************************************/

void bifurcation_diagram_calculate (BifurcationDiagram *self,
				    guint               iterations_total,
				    guint               iterations_per_column) {
    DeJongParams param;
    BifurcationColumn *column;
    HistogramPlot plot;
    int hist_width, hist_height;
    double x, y, point_x, point_y;
    int i, col_i, ix, iy;

    const float y_min = -3;
    const float y_max = 3;

    bifurcation_diagram_init_columns(self);
    histogram_imager_prepare_plots(HISTOGRAM_IMAGER(self), &plot);
    histogram_imager_get_hist_size(HISTOGRAM_IMAGER(self), &hist_width, &hist_height);

    for (i=iterations_total; i;) {

	column = bifurcation_diagram_next_column(self);
	bifurcation_diagram_get_column_params(self, column, &param);
	ix = column->ix;
	point_x = column->point.x;
	point_y = column->point.y;

	for(col_i=iterations_per_column; i && col_i; --i, --col_i) {
	    /* These are the actual Peter de Jong map equations. The new point value
	     * gets stored into 'point', then we go on and mess with x and y before plotting.
	     */
	    x = sin(param.a * point_y) - cos(param.b * point_x);
	    y = sin(param.c * point_x) - cos(param.d * point_y);
	    point_x = x;
	    point_y = y;

	    if (y >= y_min && y < y_max) {
		iy = (int)( (y - y_min) / (y_max - y_min) * hist_height );

		HISTOGRAM_IMAGER_PLOT(plot, ix, iy);
	    }
	}

	column->point.x = point_x;
	column->point.y = point_y;
    }

    histogram_imager_finish_plots(HISTOGRAM_IMAGER(self), &plot);
}

/* The End */

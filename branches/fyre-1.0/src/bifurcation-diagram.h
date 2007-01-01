/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * bifurcation-diagram.h - A HistogramImager that renders a bifurcation diagram
 *                         in which one axis interpolates across de Jong parameters
 *                         and the other axis shows a 1 dimensional projection of the
 *                         image at those parameters.
 *
 * Fyre - rendering and interactive exploration of chaotic functions
 * Copyright (C) 2004-2007 David Trowbridge and Micah Dowty
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

#ifndef __BIFURCATION_DIAGRAM_H__
#define __BIFURCATION_DIAGRAM_H__

#include <gtk/gtk.h>
#include "histogram-imager.h"
#include "de-jong.h"

G_BEGIN_DECLS

#define BIFURCATION_DIAGRAM_TYPE            (bifurcation_diagram_get_type ())
#define BIFURCATION_DIAGRAM(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), BIFURCATION_DIAGRAM_TYPE, BifurcationDiagram))
#define BIFURCATION_DIAGRAM_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), BIFURCATION_DIAGRAM_TYPE, BifurcationDiagramClass))
#define IS_BIFURCATION_DIAGRAM(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BIFURCATION_DIAGRAM_TYPE))
#define IS_BIFURCATION_DIAGRAM_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), BIFURCATION_DIAGRAM_TYPE))

typedef struct _BifurcationDiagram          BifurcationDiagram;
typedef struct _BifurcationDiagramClass     BifurcationDiagramClass;

typedef struct {
    /* Stores state information about one column of the bifurcation diagram rendering.
     * This includes the current point position, the column index, and several sets
     * of preinterpolated parameters.
     */
    guint ix;

    struct {
	gboolean valid;
	gdouble x, y;
    } point;

    struct {
	gboolean valid;
	DeJongParams param;
	double a,b,c,d;
    } interpolated[8];

} BifurcationColumn;


struct _BifurcationDiagram {
    HistogramImager parent;

    /* The interpolation function and data used to produce the bifurcation
     * diagram's X axis. The calculation dirty flag must be set when the
     * interpolation function changes in a meaningful way.
     */
    ParameterInterpolator *interp;
    gpointer               interp_data;
    GFreeFunc              interp_data_free;
    gboolean               calc_dirty_flag;

    /* Column cache. This starts out empty, (all initialized flags are FALSE)
     * but is gradually filled in with interpolated coordinates and current points.
     * Caching the interpolated params speeds up rendering greatly, and caching
     * the current point is necessary for transients to eventually fade.
     */
    BifurcationColumn *columns;
    int current_column, num_columns;

    /* Temporary DeJong object used during interpolation, created as needed */
    DeJong *interpolant;
};

struct _BifurcationDiagramClass {
    HistogramImagerClass parent_class;
};


/************************************************************************************/
/******************************************************************* Public Methods */
/************************************************************************************/

GType                bifurcation_diagram_get_type();
BifurcationDiagram*  bifurcation_diagram_new();

void                 bifurcation_diagram_calculate            (BifurcationDiagram    *self,
							       guint                  iterations_total,
							       guint                  iterations_per_column);

/* The most flexible way to set the interpolation, by
 * providing a new function and opaque interpolation data.
 * A free function for the data can also optionally be
 * provided. This will always cause calculation to start over.
 */
void                 bifurcation_diagram_set_interpolator     (BifurcationDiagram    *self,
							       ParameterInterpolator *interp,
							       gpointer               interp_data,
							       GFreeFunc              interp_data_free);

/* An optimized way to set up linear interpolation between
 * endpoints from two DeJong objects. If the existing interpolation
 * was already linear, with the same parameters, the calculation
 * dirty flag will not be set.
 */
void                 bifurcation_diagram_set_linear_endpoints (BifurcationDiagram     *self,
							       DeJong                 *first,
							       DeJong                 *second);

G_END_DECLS

#endif /* __BIFURCATION_DIAGRAM_H__ */

/* The End */

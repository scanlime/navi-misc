/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * iterative-map.h - The IterativeMap object builds on the ParameterHolder and
 *		     HistogramRender objects to provide a rendering of a chaotic
 *		     map into a histogram image.
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

#ifndef __ITERATIVE_MAP_H__
#define __ITERATIVE_MAP_H__

#include <gtk/gtk.h>
#include "histogram-imager.h"

G_BEGIN_DECLS

#define ITERATIVE_MAP_TYPE            (iterative_map_get_type ())
#define ITERATIVE_MAP(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), ITERATIVE_MAP_TYPE, IterativeMap))
#define ITERATIVE_MAP_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), ITERATIVE_MAP_TYPE, IterativeMapClass))
#define IS_ITERATIVE_MAP(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ITERATIVE_MAP_TYPE))
#define IS_ITERATIVE_MAP_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), ITERATIVE_MAP_TYPE))

typedef struct _IterativeMap      IterativeMap;
typedef struct _IterativeMapClass IterativeMapClass;

struct _IterativeMap {
    HistogramImager parent;

    /* Current calculation state */
    gdouble iterations;

    /* Estimated iterations per second, for calculate_timed and friends */
    gdouble iter_speed_estimate;

    /* For background rendering in the idle handler */
    guint idle_handler;
    double render_time;
};

struct _IterativeMapClass {
    HistogramImagerClass parent_class;

    void (*iterative_map) (IterativeMap *self);

    /* Overrideable methods */
    void (*calculate)        (IterativeMap          *self,
			      guint                  iterations);
    void (*calculate_motion) (IterativeMap          *self,
			      guint                  iterations,
			      gboolean               continuation,
			      ParameterInterpolator *interp,
			      gpointer               interp_data);
};

/************************************************************************************/
/******************************************************************* Public Methods */
/************************************************************************************/

GType         iterative_map_get_type               ();

/* Simple calculation functions, implemented by subclasses.
 * They calculate a fixed number of iterations.
 */
void          iterative_map_calculate              (IterativeMap          *self,
						    guint                  iterations);
void          iterative_map_calculate_motion       (IterativeMap          *self,
						    guint                  iterations,
						    gboolean               continuation,
						    ParameterInterpolator *interp,
						    gpointer               interp_data);

/* Calculation functions implemented by this base class,
 * that stop after an estimated amount of time rather
 * than a number of iterations. Each time this runs,
 * it uses the actual time elapsed to update an estimate
 * of the calculation speed used to come up with a
 * number of iterations to pass the actual rendering
 * functions.
 */
void          iterative_map_calculate_timed        (IterativeMap          *self,
						    double                 seconds);
void          iterative_map_calculate_motion_timed (IterativeMap          *self,
						    double                 seconds,
						    gboolean               continuation,
						    ParameterInterpolator *interp,
						    gpointer               interp_data);

/* Start or stop running calculation from a main loop
 * idle handler. The current 'render_time' is the number
 * of seconds that we nominally calculate for during
 * each main loop iteration. The default of 15ms is
 * fine for most interactive use.
 */
void          iterative_map_start_calculation      (IterativeMap          *self);
void          iterative_map_stop_calculation       (IterativeMap          *self);
gboolean      iterative_map_is_calculation_running (IterativeMap          *self);

G_END_DECLS

#endif /* __ITERATIVE_MAP_H__ */

/* The End */

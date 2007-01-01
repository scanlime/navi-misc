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

#include "iterative-map.h"
#include <stdlib.h>

enum {
    CALCULATION_FINISHED_SIGNAL,
    CALCULATION_START_SIGNAL,
    CALCULATION_STOP_SIGNAL,
    LAST_SIGNAL,
};

static void iterative_map_class_init(IterativeMapClass *klass);
static void iterative_map_init(IterativeMap *self);
static int  iterative_map_idle_handler(gpointer user_data);
static guint limit_iterations(guint iters);

static guint iterative_map_signals[LAST_SIGNAL] = { 0 };


/************************************************************************************/
/**************************************************** Initialization / Finalization */
/************************************************************************************/

GType iterative_map_get_type(void) {
    static GType im_type = 0;
    if (!im_type) {
	static const GTypeInfo im_info = {
	    sizeof(IterativeMapClass),
	    NULL, /* base init */
	    NULL, /* base finalize */
	    (GClassInitFunc) iterative_map_class_init,
	    NULL, /* class finalize */
	    NULL, /* class data */
	    sizeof(IterativeMap),
	    0,
	    (GInstanceInitFunc) iterative_map_init,
	};

	im_type = g_type_register_static(HISTOGRAM_IMAGER_TYPE, "IterativeMap", &im_info, 0);
    }

    return im_type;
}

static void iterative_map_class_init(IterativeMapClass *klass) {
    iterative_map_signals[CALCULATION_FINISHED_SIGNAL] =
	g_signal_new("calculation-finished",
		     G_TYPE_FROM_CLASS(klass),
		     G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
		     G_STRUCT_OFFSET(IterativeMapClass, iterative_map),
		     NULL,
		     NULL,
		     g_cclosure_marshal_VOID__VOID,
		     G_TYPE_NONE, 0);

    iterative_map_signals[CALCULATION_START_SIGNAL] =
	g_signal_new("calculation-start",
		     G_TYPE_FROM_CLASS(klass),
		     G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
		     G_STRUCT_OFFSET(IterativeMapClass, iterative_map),
		     NULL,
		     NULL,
		     g_cclosure_marshal_VOID__VOID,
		     G_TYPE_NONE, 0);

    iterative_map_signals[CALCULATION_STOP_SIGNAL] =
	g_signal_new("calculation-stop",
		     G_TYPE_FROM_CLASS(klass),
		     G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
		     G_STRUCT_OFFSET(IterativeMapClass, iterative_map),
		     NULL,
		     NULL,
		     g_cclosure_marshal_VOID__VOID,
		     G_TYPE_NONE, 0);
}

static void iterative_map_init(IterativeMap *self) {
    self->render_time = 0.015;
}


/************************************************************************************/
/********************************************************************** Calculation */
/************************************************************************************/

void iterative_map_calculate(IterativeMap *self, guint iterations) {
    IterativeMapClass *class = ITERATIVE_MAP_CLASS(G_OBJECT_GET_CLASS(self));
    class->calculate(self, iterations);
    g_signal_emit(G_OBJECT(self), iterative_map_signals[CALCULATION_FINISHED_SIGNAL], 0);
}

void iterative_map_calculate_motion(IterativeMap          *self,
                                    guint                  iterations,
                                    gboolean               continuation,
                                    ParameterInterpolator *interp,
                                    gpointer               interp_data) {
    IterativeMapClass *class = ITERATIVE_MAP_CLASS(G_OBJECT_GET_CLASS(self));
    class->calculate_motion(self, iterations, continuation, interp, interp_data);
    g_signal_emit(G_OBJECT(self), iterative_map_signals[CALCULATION_FINISHED_SIGNAL], 0);
}

static guint limit_iterations(guint iters)
{
    /* Put both upper and lower limits on the number of iters to run
     * at once- if the iteration count is too low, our next speed estimate
     * will be way off, and if it's too high we could have problems with
     * memory allocated per-iteration.
     */
    return MAX(MIN(iters, 10000000), 1000);
}

void iterative_map_calculate_timed(IterativeMap          *self,
				   double                 seconds) {
    GTimer *timer;
    double elapsed;
    guint iterations;

    iterations = limit_iterations(self->iter_speed_estimate * seconds + 0.5);
    timer = g_timer_new();
    g_timer_start(timer);

    iterative_map_calculate(self, iterations);

    elapsed = g_timer_elapsed(timer, NULL);
    g_timer_destroy(timer);

    self->iter_speed_estimate = iterations / elapsed;
}

void iterative_map_calculate_motion_timed(IterativeMap          *self,
					  double                 seconds,
					  gboolean               continuation,
					  ParameterInterpolator *interp,
					  gpointer               interp_data) {

    GTimer *timer;
    double elapsed;
    guint iterations;

    iterations = limit_iterations(self->iter_speed_estimate * seconds + 0.5);
    timer = g_timer_new();
    g_timer_start(timer);

    iterative_map_calculate_motion(self, iterations, continuation, interp, interp_data);

    elapsed = g_timer_elapsed(timer, NULL);
    g_timer_destroy(timer);

    self->iter_speed_estimate = iterations / elapsed;
}

static int    iterative_map_idle_handler(gpointer user_data)
{
    IterativeMap* self = ITERATIVE_MAP(user_data);
    iterative_map_calculate_timed(self, self->render_time);
    return 1;
}

void          iterative_map_start_calculation      (IterativeMap          *self)
{
    if (self->idle_handler)
	return;
    self->idle_handler = g_idle_add(iterative_map_idle_handler, self);
    g_signal_emit(G_OBJECT(self), iterative_map_signals[CALCULATION_START_SIGNAL], 0);
}

void          iterative_map_stop_calculation       (IterativeMap          *self)
{
    if (!self->idle_handler)
	return;
    g_source_remove(self->idle_handler);
    self->idle_handler = 0;
    g_signal_emit(G_OBJECT(self), iterative_map_signals[CALCULATION_STOP_SIGNAL], 0);
}

gboolean      iterative_map_is_calculation_running (IterativeMap          *self)
{
    return (self->idle_handler != 0);
}

/* The End */

/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * batch-image-render.c - Still image rendering with no GUI
 *
 * Fyre - rendering and interactive exploration of chaotic functions
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

#include "config.h"
#include <stdio.h>
#include <string.h>
#include "batch-image-render.h"

#ifdef HAVE_GNET
#include "cluster-model.h"
#endif

typedef struct {
    gulong      target_density;
    GMainLoop*  main_loop;
    GTimer*     status_timer;
} BatchImageRender;

static void       on_calc_finished            (IterativeMap*      map,
					       BatchImageRender*  self);

void batch_image_render(IterativeMap*  map,
			const char*    filename,
			gulong         target_density)
{
    BatchImageRender self;

    self.target_density = target_density;
    self.main_loop = g_main_loop_new(NULL, FALSE);
    self.status_timer = g_timer_new();

    /* Have the map let us know after each iteration block finishes */
    g_signal_connect(map, "calculation-finished", G_CALLBACK(on_calc_finished), &self);

    /* Keep the rendering time low enough that gnet doesn't
     * get cranky, but high enough that it still gets most of our CPU time
     */
    map->render_time = 0.1;
    iterative_map_start_calculation(map);
    g_main_loop_run(self.main_loop);
    iterative_map_stop_calculation(map);

    g_timer_destroy(self.status_timer);

#ifdef HAVE_EXR
    /* Save as an OpenEXR file if it has a .exr extension, otherwise use PNG */
    if (strlen(filename) > 4 && strcmp(".exr", filename + strlen(filename) - 4)==0) {
	printf("Creating OpenEXR image...\n");
	exr_save_image_file(HISTOGRAM_IMAGER(map), filename);
    }
    else
#endif
	{
	    printf("Creating PNG image...\n");
	    histogram_imager_save_image_file(HISTOGRAM_IMAGER(map), filename);
	}
}


static void       on_calc_finished            (IterativeMap*       map,
					       BatchImageRender*   self)
{
    double elapsed, remaining;

    if (HISTOGRAM_IMAGER(map)->peak_density >= self->target_density)
	g_main_loop_quit(self->main_loop);

    /* Limit the update rate of this status message independently
     * from our actual calculation speed.
     */
    if (g_timer_elapsed(self->status_timer, NULL) < 2.0)
        return;
    g_timer_start(self->status_timer);

    /* This should be a fairly accurate time estimate, since (asymptotically at least)
     * current_density increases linearly with the number of iterations performed.
     * Elapsed time and time remaining are in seconds.
     */
    elapsed = histogram_imager_get_elapsed_time(HISTOGRAM_IMAGER(map));
    remaining = elapsed * self->target_density / HISTOGRAM_IMAGER(map)->peak_density - elapsed;

    /* After each batch of iterations, show the percent completion, number
     * of iterations (in scientific notation), iterations per second,
     * density / target density, and elapsed time / remaining time.
     */
    printf("%6.02f%%   %.3e   %.2e/sec   %6ld / %ld   %02d:%02d:%02d / %02d:%02d:%02d\n",
	   100.0 * HISTOGRAM_IMAGER(map)->peak_density / self->target_density,
	   map->iterations, map->iterations / elapsed,
	   HISTOGRAM_IMAGER(map)->peak_density, self->target_density,
	   ((int)elapsed) / (60*60), (((int)elapsed) / 60) % 60, ((int)elapsed)%60,
	   ((int)remaining) / (60*60), (((int)remaining) / 60) % 60, ((int)remaining)%60);

#ifdef HAVE_GNET
    {
	ClusterModel *cluster = cluster_model_get(map, FALSE);
	if (cluster) {
	    cluster_model_show_status(cluster);
	    g_object_unref(cluster);
	    printf("\n");
	}
    }
#endif
}

/* The End */

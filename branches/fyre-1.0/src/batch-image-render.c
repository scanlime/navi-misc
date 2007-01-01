/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * batch-image-render.c - Still image rendering with no GUI
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

#include "config.h"
#include <stdio.h>
#include <string.h>
#include "batch-image-render.h"

#ifdef HAVE_GNET
#include "cluster-model.h"
#endif

typedef struct {
    double      quality;
    GMainLoop*  main_loop;
    GTimer*     status_timer;
} BatchImageRender;

static void       on_calc_finished            (IterativeMap*      map,
					       BatchImageRender*  self);

void batch_image_render(IterativeMap*  map,
			const char*    filename,
			double         quality)
{
    BatchImageRender self;

#ifdef HAVE_GNET
    {
	ClusterModel *cluster = cluster_model_get(map, FALSE);
	if (cluster) {
	    /* Stream in new results very infrequently, since this is batch rendering */
	    cluster_model_set_min_stream_interval(cluster, 10.0);
	    g_object_unref(cluster);
	}
    }
#endif

    self.quality = quality;
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
	GError *error = NULL;
	printf("Creating OpenEXR image...\n");
	exr_save_image_file(HISTOGRAM_IMAGER(map), filename, &error);
	if (error) {
	    g_print ("Error: %s\n", error->message);
	    g_error_free (error);
	}
    }
    else
#endif
	{
            GError *error = NULL;
	    printf("Creating PNG image...\n");
	    histogram_imager_save_image_file(HISTOGRAM_IMAGER(map), filename, &error);
	    if (error) {
                g_print ("Error: %s\n", error->message);
		g_error_free (error);
	    }
	}
}


static void       on_calc_finished            (IterativeMap*       map,
					       BatchImageRender*   self)
{
    double elapsed, remaining;
    double current_quality = histogram_imager_compute_quality(HISTOGRAM_IMAGER(map));

    if (current_quality >= self->quality)
	g_main_loop_quit(self->main_loop);

    /* Limit the update rate of this status message independently
     * from our actual calculation speed.
     */
    if (g_timer_elapsed(self->status_timer, NULL) < 2.0)
        return;
    g_timer_start(self->status_timer);

    /* Compute a somewhat bogus time estimate using a linear approximation */
    elapsed = histogram_imager_get_elapsed_time(HISTOGRAM_IMAGER(map));
    remaining = elapsed * self->quality / current_quality - elapsed;

    /* After each batch of iterations, show the percent completion, number
     * of iterations (in scientific notation), iterations per second,
     * current quality / target quality, and elapsed time / remaining time.
     */
    printf("%6.02f%%   %.3e   %.2e/sec  %8.04f / %.01f   %02d:%02d:%02d / %02d:%02d:%02d\n",
	   100.0 * current_quality / self->quality,
	   map->iterations, map->iterations / elapsed,
	   current_quality, self->quality,
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

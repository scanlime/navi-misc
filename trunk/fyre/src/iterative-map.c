/*
 * iterative-map.h - The IterativeMap object builds on the ParameterHolder and
 *		     HistogramRender objects to provide a rendering of a chaotic
 *		     map into a histogram image.
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

#include "iterative-map.h"
#include <stdlib.h>

static void iterative_map_class_init(IterativeMapClass *klass);
static void iterative_map_init(IterativeMap *self);

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
  /* Nothing to do here */
}

static void iterative_map_init(IterativeMap *self) {
  /* Nothing to do here */
}

void iterative_map_calculate(IterativeMap *self, guint iterations) {
  IterativeMapClass *class = ITERATIVE_MAP_CLASS(G_OBJECT_GET_CLASS(self));
  class->calculate(self, iterations);
}

void iterative_map_calculate_motion(IterativeMap          *self,
                                    guint                  iterations,
                                    gboolean               continuation,
                                    ParameterInterpolator *interp,
                                    gpointer               interp_data) {
  IterativeMapClass *class = ITERATIVE_MAP_CLASS(G_OBJECT_GET_CLASS(self));
  class->calculate_motion(self, iterations, continuation, interp, interp_data);
}

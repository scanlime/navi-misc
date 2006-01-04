/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * probability-map.h - A 2-D random variable with a probability
 *                     distribution function defined as an image.
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

#ifndef __PROBABILITY_MAP_H__
#define __PROBABILITY_MAP_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define PROBABILITY_MAP_TYPE            (probability_map_get_type ())
#define PROBABILITY_MAP(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), PROBABILITY_MAP_TYPE, ProbabilityMap))
#define PROBABILITY_MAP_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), PROBABILITY_MAP_TYPE, ProbabilityMapClass))
#define IS_PROBABILITY_MAP(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), PROBABILITY_MAP_TYPE))
#define IS_PROBABILITY_MAP_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), PROBABILITY_MAP_TYPE))

typedef struct _ProbabilityMap         ProbabilityMap;
typedef struct _ProbabilityMapClass    ProbabilityMapClass;

struct _ProbabilityMap {
    GObject object;

    /* Public read-only */

    int     width;
    int     height;

    /* Private */

    gfloat* cumulative;           /* 1D cumulative distribution function */
    gsize   cumulative_length;

    gdouble image_scale_x;
    gdouble image_scale_y;

    gdouble energy;
    int current;
};

struct _ProbabilityMapClass {
    GObjectClass parent_class;
};

typedef enum {
    FYRE_CHANNEL_RED,
    FYRE_CHANNEL_GREEN,
    FYRE_CHANNEL_BLUE,
    FYRE_CHANNEL_ALPHA,
    FYRE_CHANNEL_LUMA,
} FyreImageChannel;


/************************************************************************************/
/******************************************************************* Public Methods */
/************************************************************************************/

GType            probability_map_get_type            ();

/* Create a new probability map from a file, automatically identifying its type. */
ProbabilityMap*  probability_map_new_file            (const gchar*          filename);

/* Create a new probability map from a file, automatically identifying its type.
 * Uses data from the indicated channel.
 */
ProbabilityMap*  probability_map_new_file_channel    (const gchar*          filename,
						      FyreImageChannel      channel);

/* Create a new probability map from a GdkPixbuf. */
ProbabilityMap*  probability_map_new_pixbuf          (GdkPixbuf*            pixbuf);

/* Create a new probability map from a GdkPixbuf. Uses data from the indicated channel */
ProbabilityMap*  probability_map_new_pixbuf_channel  (GdkPixbuf*            pixbuf,
						      FyreImageChannel      channel);

/* Create a new probability map from a raw image, given its pixel and row strides,
 * and the GType used for each pixel sample. The data does not need to remain valid
 * after this call completes, the caller can free it immediately.
 *
 * Currently, pixel types of G_TYPE_UCHAR, G_TYPE_UINT, G_TYPE_ULONG, G_TYPE_FLOAT,
 * and G_TYPE_DOUBLE are valid.
 */
ProbabilityMap*  probability_map_new_raw          (const guchar*         data,
						   gint                  width,
						   gint                  height,
						   gint                  row_stride,
						   gint                  pixel_stride,
						   gint                  pixel_type);

/* There are several functions for retrieving new random values from the
 * probability map: integer pixel coordinates, normalized (0 to 1) floating
 * point coordinates with no filtering, uniform filtering, or gaussian filtering.
 *
 * With no filtering, the results will always lie on pixel boundaries. With
 * uniform filtering, each pixel is treated as a square uniform distribution.
 * With gaussian filtering, each pixel is the center of a small gaussian distribution
 * with a given standard deviation.
 */
void             probability_map_ints             (ProbabilityMap*       self,
						   guint*                x,
						   guint*                y);
void             probability_map_normalized       (ProbabilityMap*       self,
						   gdouble*              x,
						   gdouble*              y);
void             probability_map_uniform          (ProbabilityMap*       self,
						   gdouble*              x,
						   gdouble*              y);
void             probability_map_gaussian         (ProbabilityMap*       self,
						   gdouble*              x,
						   gdouble*              y,
						   double                radius);

G_END_DECLS

#endif /* __PROBABILITY_MAP_H__ */

/* The End */

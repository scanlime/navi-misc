/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * probability-map.c - A 2-D random variable with a probability
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

#include <gtk/gtk.h>
#include "probability-map.h"
#include "math-util.h"

static void probability_map_class_init(ProbabilityMapClass *klass);
static void probability_map_dispose(GObject *gobject);


/************************************************************************************/
/**************************************************** Initialization / Finalization */
/************************************************************************************/

GType probability_map_get_type(void)
{
    static GType obj_type = 0;

    if (!obj_type) {
	static const GTypeInfo obj_info = {
	    sizeof(ProbabilityMapClass),
	    NULL, /* base_init */
	    NULL, /* base_finalize */
	    (GClassInitFunc) probability_map_class_init,
	    NULL, /* class_finalize */
	    NULL, /* class_data */
	    sizeof(ProbabilityMap),
	    0,
	    NULL, /* instance init */
	};

	obj_type = g_type_register_static(G_TYPE_OBJECT, "ProbabilityMap", &obj_info, 0);
    }

    return obj_type;
}

static void probability_map_class_init(ProbabilityMapClass *klass)
{
    GObjectClass *object_class = (GObjectClass*) klass;
    object_class->dispose = probability_map_dispose;
}

static void probability_map_dispose(GObject *gobject)
{
    ProbabilityMap *self = PROBABILITY_MAP(gobject);

    if (self->cumulative) {
	g_free(self->cumulative);
	self->cumulative = NULL;
    }
}


/************************************************************************************/
/********************************************************************* Constructors */
/************************************************************************************/

ProbabilityMap*  probability_map_new_file            (const gchar*          filename)
{
    return probability_map_new_file_channel(filename, FYRE_CHANNEL_LUMA);
}

ProbabilityMap*  probability_map_new_file_channel    (const gchar*          filename,
						      FyreImageChannel      channel)
{
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(filename, NULL);
    ProbabilityMap *self;

    g_assert(pixbuf != NULL);
    self = probability_map_new_pixbuf_channel(pixbuf, channel);
    gdk_pixbuf_unref(pixbuf);

    return self;
}

ProbabilityMap*  probability_map_new_pixbuf          (GdkPixbuf*            pixbuf)
{
    return probability_map_new_pixbuf_channel(pixbuf, FYRE_CHANNEL_LUMA);
}

ProbabilityMap*  probability_map_new_pixbuf_channel  (GdkPixbuf*            pixbuf,
						      FyreImageChannel      channel)
{
    int offset;

    g_assert(gdk_pixbuf_get_colorspace(pixbuf) == GDK_COLORSPACE_RGB);
    g_assert(gdk_pixbuf_get_bits_per_sample(pixbuf) == 8);

    switch (channel) {

    case FYRE_CHANNEL_RED:
	offset = 0;
	break;

    case FYRE_CHANNEL_GREEN:
	offset = 1;
	break;

    case FYRE_CHANNEL_BLUE:
	offset = 2;
	break;

    case FYRE_CHANNEL_ALPHA:
	g_assert(gdk_pixbuf_get_has_alpha(pixbuf));
	offset = 3;
	break;

    default:
	g_warning("Unsupported channel");
	offset = 0;
    }

    return probability_map_new_raw(gdk_pixbuf_get_pixels(pixbuf) + offset,
				   gdk_pixbuf_get_width(pixbuf),
				   gdk_pixbuf_get_height(pixbuf),
				   gdk_pixbuf_get_rowstride(pixbuf),
				   gdk_pixbuf_get_n_channels(pixbuf),
				   G_TYPE_UCHAR);
}

ProbabilityMap*  probability_map_new_raw          (const guchar*         data,
						   gint                  width,
						   gint                  height,
						   gint                  row_stride,
						   gint                  pixel_stride,
						   gint                  pixel_type)
{
    ProbabilityMap *self = PROBABILITY_MAP(g_object_new(probability_map_get_type(), NULL));
    gdouble sum = 0;
    const guchar* row = data;
    const guchar* pixel;
    int x;
    int y = height;
    gfloat *output;

    self->width = width;
    self->height = height;
    self->image_scale_x = 1.0 / (width - 1);
    self->image_scale_y = 1.0 / (height - 1);

    self->cumulative_length = width * height;
    self->cumulative = g_new(gfloat, self->cumulative_length);
    output = self->cumulative;

    /* Caluclate a cumulative distribution. For each pixel, we save the
     * sum of it and all prior pixels. For speed, these inner loops are
     * implemented separately for each pixel type using a macro.
     */

#define CALCULATE_SUMS(type) do {       \
        while (y) {                     \
	    x = width;                  \
	    pixel = row;                \
	    while (x) {                 \
		sum += *(type*)pixel;   \
		*output = sum;          \
		output++;               \
		x--;                    \
		pixel += pixel_stride;  \
	    }                           \
	    y--;                        \
	    row += row_stride;          \
	};                              \
    } while (0)

    switch (pixel_type) {

    case G_TYPE_UCHAR:
	CALCULATE_SUMS(guchar);
	break;

    case G_TYPE_UINT:
	CALCULATE_SUMS(guint);
	break;

    case G_TYPE_ULONG:
	CALCULATE_SUMS(gulong);
	break;

    case G_TYPE_FLOAT:
	CALCULATE_SUMS(gfloat);
	break;

    case G_TYPE_DOUBLE:
	CALCULATE_SUMS(gdouble);
	break;

    default:
	g_warning("Unsupported pixel format");

    }

#undef CALCULATE_SUMS

    return self;
}


/************************************************************************************/
/******************************************************************* Public Methods */
/************************************************************************************/

void             probability_map_ints             (ProbabilityMap*       self,
						   guint*                x,
						   guint*                y)
{
#if 1
    gfloat key;
    gfloat* cumulative = self->cumulative;
    gsize length = self->cumulative_length;
    gsize width = self->width;
    struct {
	int index;
	int ge;
    } endpoints[2], midpoints[2];

    /* Start with a uniform variate, scaled to cover the range of our CDF */
    key = uniform_variate() * cumulative[length - 1];

    /* Do a binary search for the first value in 'cumulative' that's greater than
     * or equal to 'key'. These semantics are necessary to ensure that in a run
     * of values with the same cumulative value, only the first one is ever chosen
     * since the other values have zero probability in the original image.
     */
    endpoints[0].index = 0;
    endpoints[1].index = length - 1;
    endpoints[0].ge = cumulative[endpoints[0].index] >= key;
    endpoints[1].ge = cumulative[endpoints[1].index] >= key;

    while (endpoints[1].index > endpoints[0].index) {
	midpoints[0].index = (endpoints[0].index + endpoints[1].index) >> 1;
	midpoints[1].index = midpoints[0].index + 1;
	midpoints[0].ge = cumulative[midpoints[0].index] >= key;
	midpoints[1].ge = cumulative[midpoints[1].index] >= key;

	if ((!midpoints[0].ge) && midpoints[1].ge) {
	    /* We're sitting right on the stopping condition */
	    break;
	}
	else if (!midpoints[1].ge) {
	    /* The solution is above this */
	    endpoints[0] = midpoints[1];
	}
	else if (midpoints[0].ge) {
	    /* The solution is below this */
	    endpoints[1] = midpoints[0];
	}
	else {
	    /* This should only be possible if our CDF wasn't sorted */
	    g_assert(0);
	}
    }

    /* Convert an index within our cumulative array into (x,y) coords */
    *x = midpoints[1].index % width;
    *y = midpoints[1].index / width;
#else
    gfloat* cumulative = self->cumulative;
    gsize length = self->cumulative_length;
    gdouble energy;
    gint current = self->current;
    gsize width = self->width;

    energy = uniform_variate() * 1000;

    while (energy > 0) {
	energy -= cumulative[current+1] - cumulative[current];
	current++;
	if (current == length-2)
	    current = 0;
    }

    *x = current % width;
    *y = current / width;

    self->current = current;
#endif
}

void             probability_map_normalized       (ProbabilityMap*       self,
						   gdouble*              x,
						   gdouble*              y)
{
    guint xi, yi;
    probability_map_ints(self, &xi, &yi);
    *x = xi * self->image_scale_x;
    *y = yi * self->image_scale_y;
}

void             probability_map_uniform          (ProbabilityMap*       self,
						   gdouble*              x,
						   gdouble*              y)
{
    probability_map_normalized(self, x, y);
    *x += uniform_variate() * self->image_scale_x;
    *y += uniform_variate() * self->image_scale_y;
}

void             probability_map_gaussian         (ProbabilityMap*       self,
						   gdouble*              x,
						   gdouble*              y,
						   double                radius)
{
    double a, b;

    probability_map_normalized(self, x, y);

    normal_variate_pair(&a, &b);
    *x += a * self->image_scale_x * radius;
    *y += b * self->image_scale_y * radius;
}

/* The End */

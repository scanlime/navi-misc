/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * RtgChannel - The channel object retrieves and stores (x,y) pairs,
 *              where X is an independent variable (usually time).
 *              One or more channels may supply the content in a
 *              graph. Each channel maintains its own history buffer,
 *              as each channel can have an independent sampling rate
 *              and history length.
 *
 *
 *

FIXME: These are just temporary notes

Operations:

Insert point with X value greater than all existing points (fastest)
Insert point with arbitrary X value

Search for point nearest to / before / after a given X value
Get the Y value of a point
Iterate over points forward
Iterate over points backward

Search for an interval between two X values
Get the mean, minimum, and maximum over that interval
Skip to an adjacent interval before/after the current one
- Intervals have three components:
   - between the beginning of the interval and the first sample
     (computed according to interpolation)
   - all complete sample-to-sample periods in the interval
     (looked up from the pyramid buffers)
   - between the last sample and the end of the interval
     (computed according to interpolation)

Representation:

- Pyramids of 1/2 scale buffers for fast interval lookup
 - Individual buffers are stored as B+-trees

Pyramid scale is based on dividing X values, not number of samples.
This makes it fast to determine which levels must be considered,
and it means pyramids can be updated rather than completely regenerated
when samples are inserted/deleted.

Lowest level: B+-tree associating X value keys with Y value data
Other levels: B+-tree associating quantized X value keys with interval stats.
              Pyramid level determines X quantization. Minimum and maximum
              quantization levels must be set by user?

+-----------------------+
|           -           |
+-----------+-----------+
|           |           |
+-----+-----+-----+-----+
|     |     |     |     |
+--+--+--+--+--+--+--+--+
|  |  |  |  |  |  |  |  |
+--+--+--+--+--+--+--+--+


 *
 * rtgraph real-time graphing package
 * Copyright (C) 2004 Micah Dowty <micah@navi.cx>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef __RTG_CHANNEL_H__
#define __RTG_CHANNEL_H__

#include <glib.h>

G_BEGIN_DECLS

#define RTG_CHANNEL_TYPE            (rtg_channel_get_type ())
#define RTG_CHANNEL(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), RTG_CHANNEL_TYPE, RtgChannel))
#define RTG_CHANNEL_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), RTG_CHANNEL_TYPE, RtgChannelClass))
#define IS_RTG_CHANNEL(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), RTG_CHANNEL_TYPE))
#define IS_RTG_CHANNEL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), RTG_CHANNEL_TYPE))

typedef struct _RtgChannel      RtgChannel;
typedef struct _RtgChannelClass RtgChannelClass;
typedef struct _RtgChannelIter  RtgChannelIter;

struct _RtgChannel {
    GObject object;
};

struct _RtgChannelClass {
    GObjectClass parent_class;
};

struct _RtgChannelIter {

};

enum {
    RTG_INTERPOLATION_PREVIOUS,
    RTG_INTERPOLATION_NEAREST,
    RTG_INTERPOLATION_LINEAR,
    RTG_INTERPOLATION_SMOOTH,
};


/************************************************************************************/
/******************************************************************* Public Methods */
/************************************************************************************/

GType         rtg_channel_get_type          ();
RtgChannel*   rtg_channel_new               ();

/* The channel's capacity, specified as a number of samples
 * and/or a maximum range of X values. Zero indicates no restriction.
 */
void          rtg_channel_set_max_samples   (RtgChannels* self, gulong max_samples);
gulong        rtg_channel_get_max_samples   (RtgChannels* self);

/* A channel may be equipped with strings specifying a name, and units
 * for each variable. These are optional, and not used by the channel itself.
 */
void          rtg_channel_set_name          (RtgChannel* self, gchar* name);
gchar*        rtg_channel_get_name          (RtgChannel* self);
void          rtg_channel_set_x_units       (RtgChannel* self, gchar* units);
gchar*        rtg_channel_get_x_units       (RtgChannel* self);
void          rtg_channel_set_y_units       (RtgChannel* self, gchar* units);
gchar*        rtg_channel_get_y_units       (RtgChannel* self);

/* The channel's current interpolation type, an RTG_INTERPOLATION_* constant.
 * These are used when looking up values from channel iterators.
 */
void          rtg_channel_set_interpolation (RtgChannel* self, guint interpolation);
guint         rtg_channel_get_interpolation (RtgChannel* self);

/* Adding new values, requires only an X and Y coordinate to add */
void          rtg_channel_add_value         (RtgChannel* self, gdouble x, gdouble y);

/* Initialize an iterator to the current beginning or end of our buffer */
void          rtg_channel_iter_first        (RtgChannel* self, RtgChannelIter* iter);
void          rtg_channel_iter_last         (RtgChannel* self, RtgChannelIter* iter);

/* Is this iterator still valid? Invalid iterators may be those that extend
 * past the bounds of our buffer, or refer to data points that no longer exist.
 */
gboolean      rtg_channel_iter_is_valid     (RtgChannel* self, RtgChannelIter* iter);

/* Seeking an iterator given an X coordinate. This can put the iterator exactly
 * at the given coordinate, in which case an interpolated value is used, or it can
 * snap to the data point before it, after it, or nearest to it.
 */
void          rtg_channel_iter_at           (RtgChannel* self, RtgChannelIter* iter, gdouble x);
void          rtg_channel_iter_before       (RtgChannel* self, RtgChannelIter* iter, gdouble x);
void          rtg_channel_iter_after        (RtgChannel* self, RtgChannelIter* iter, gdouble x);
void          rtg_channel_iter_nearest      (RtgChannel* self, RtgChannelIter* iter, gdouble x);

/* Seek an iterator forward (toward positive X) by some number of samples or by an X
 * coordinate amount. Negative numbers seek toward negative X.
 */
void          rtg_channel_iter_seek_samples (RtgChannel* self, RtgChannelIter* iter, int samples);
void          rtg_channel_iter_seek         (RtgChannel* self, RtgChannelIter* iter, gdouble x);

/* Retrieve the current Y value at an iterator, interpolating as necessary */
gdouble       rtg_channel_iter_read         (RtgChannel* self, RtgChannelIter* iter);


G_END_DECLS

#endif /* __RTG_CHANNEL_H__ */

/* The End */

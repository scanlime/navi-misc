/*
 * RtgChannel - The channel object retrieves and stores (x,y) pairs,
 *              where X is an independent variable (usually time).
 *              One or more channels may supply the content in a
 *              graph. Each channel maintains its own history buffer,
 *              as each channel can have an independent sampling rate
 *              and history length.
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

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define RTG_CHANNEL_TYPE            (rtg_channel_get_type ())
#define RTG_CHANNEL(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), RTG_CHANNEL_TYPE, RtgChannel))
#define RTG_CHANNEL_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), RTG_CHANNEL_TYPE, RtgChannelClass))
#define IS_RTG_CHANNEL(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), RTG_CHANNEL_TYPE))
#define IS_RTG_CHANNEL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), RTG_CHANNEL_TYPE))

typedef struct _RtgChannel      RtgChannel;
typedef struct _RtgChannelClass RtgChannelClass;

struct _RtgChannel {
  GObject object;
};

struct _RtgChannelClass {
  GObjectClass parent_class;
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
 * and/or a range of stored X values. +/- Inf may be used in
 * specifying the range.
 */
void          rtg_channel_set_max_samples   (RtgChannels

void          rtg_channel_set_name          (RtgChannel* self, gchar* name);
gchar*        rtg_channel_get_name          (RtgChannel* self);
void          rtg_channel_set_x_units       (RtgChannel* self, gchar* units);
gchar*        rtg_channel_get_x_units       (RtgChannel* self);
void          rtg_channel_set_y_units       (RtgChannel* self, gchar* units);
gchar*        rtg_channel_get_y_units       (RtgChannel* self);
void          rtg_channel_set_interpolation (RtgChannel* self, guint interpolation);
guint         rtg_channel_get_interpolation (RtgChannel* self);

void          rtg_channel_add_value         (RtgChannel* self, gdouble x, gdouble y);





G_END_DECLS

#endif /* __RTG_CHANNEL_H__ */

/* The End */

/*
 * bptree.h - A somewhat generic B+-Tree implementation used by rtgraph
 *            channels internally. Keys and values are stored in the nodes
 *            themselves, and are arbitrary but fixed sizes. User-defined
 *            comparison and allocation functions are used. All pointers
 *            are relative to a user-defined base, so this will work with
 *            memory-mapped files.
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

#ifndef __RTG_BPTREE_H__
#define __RTG_BPTREE_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

typedef struct _RtgBPTree       RtgBPTree;
typedef struct _RtgBPTreeHeader RtgBPTreeHeader;
typedef struct _RtgBPTreeIter   RtgBPTreeIter;

/* An index into the memory at pointer_base */
typedef e

typedef gpointer                RtgBPIndexPointer;
typedef gpointer                RtgBPLeafPointer;

/* The B+-Tree object itself. This contains pointers that can't be
 * stored persistently, so it must always be recreated every time
 * a B+-Tree is loaded.
 */
struct _RtgBPTree {
  int        (*compare_keys)(gpointer a, gpointer b);
  gpointer   (*malloc)(gsize bytes);
  void       (*free)(gpointer p);

  /* Data types. These should be rounded up if necessary for alignment */
  gsize      sizeof_key;
  gsize      sizeof_value;

  /* Tree proportions */
  int        index_size;
  int        leaf_size;

  /* Endpoints in our doubly-linked list of leaves */
  RtgBPLeafPointer *first_leaf, *last_leaf;

  /* Optional memory address offset */
  gpointer   pointer_base;

  /* Memory management hooks */

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

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


G_END_DECLS

#endif /* __RTG_CHANNEL_H__ */

/* The End */

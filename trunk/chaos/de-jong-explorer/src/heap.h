/*
 * heap.h - Simple heap and priority queue algorithms. The heap is represented
 *          by a C struct and a glib boxed type. The heap's contents are
 *          arbitrary pointers. When creating a heap, a comparison function
 *          is supplied.
 *
 *          This implements a max-heap which can be used as a max priority queue.
 *          It can easily be used as a min-heap and min priority queue though by
 *          providing an inverted compare function.
 *
 *          This is based on algorithms described in "Introduction to Algorithms,
 *          second edition" by Thomas H. Cormen, Charles E. Leisserson, Ronald
 *          L. Rivest, and Clifford Stein.
 *
 * de Jong Explorer - interactive exploration of the Peter de Jong attractor
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

#ifndef __HEAP_H__
#define __HEAP_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define HEAP_TYPE            (heap_get_type ())
#define HEAP(obj)            (((Heap*)(obj)))

typedef struct _Heap Heap;

struct _Heap {
  gpointer *array;
  gsize     heap_size;
  gsize     array_size;
  GCompareDataFunc cmp;
  gpointer  cmp_data;
};


/************************************************************************************/
/******************************************************************* Public Methods */
/************************************************************************************/

GType     heap_get_type();
Heap*     heap_new(gsize array_size, GCompareDataFunc cmp, gpointer cmp_data);
Heap*     heap_copy(const Heap *self);
void      heap_free(Heap *self, GFreeFunc free);

gpointer  heap_get_maximum(Heap *self);
gpointer  heap_extract_maximum(Heap *self);
void      heap_insert(Heap *self, gpointer item);

G_END_DECLS

#endif /* __HEAP_H__ */

/* The End */

/*
 * heap.c - Simple heap and priority queue algorithms. The heap is represented
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

#include "heap.h"

#define PARENT(i)  ((i) >> 1)
#define LEFT(i)    ((i) << 1)
#define RIGHT(i)   (LEFT(i) | 1)

typedef gsize heap_index_t;

static void heap_max_heapify(Heap *self, heap_index_t i);


GType heap_get_type() {
  static GType heap_type = 0;
  if (!heap_type)
    heap_type = g_boxed_type_register_static("Heap",
					     (GBoxedCopyFunc) heap_copy,
					     (GBoxedFreeFunc) heap_free);
  return heap_type;
}

Heap* heap_new(gsize array_size, GCompareDataFunc cmp, gpointer cmp_data) {
  Heap *h = g_malloc0(sizeof(Heap));
  h->array = g_malloc0(sizeof(gpointer) * array_size);
  h->array_size = array_size;
  h->cmp = cmp;
  h->cmp_data = cmp_data;
  return h;
}

Heap* heap_copy(const Heap *self) {
  Heap *h = g_malloc0(sizeof(Heap));
  memcpy(h, self, sizeof(Heap));
  h->array = g_malloc0(sizeof(gpointer) * self->array_size);
  memcpy(h->array, self->array, sizeof(gpointer) * self->array_size);
  return h;
}

void heap_free(Heap *self, GFreeFunc free) {
  heap_index_t i;

  if (free) {
    for (i=0; i<self->heap_size; i++)
      free(self->array[i]);
  }

  g_free(self->array);
  g_free(self);
}

static void heap_max_heapify(Heap *self, heap_index_t i) {
  /* Given an array index i, with valid max-heaps as both children,
   * rearrange the heap such that i's subtree is now a valid max-heap.
   *
   * This is based on the pseudocode on page 130 of 'Introduction to Algorithms',
   * but with a loop replacing the tail recursion.
   */
  heap_index_t l, r, largest;
  gpointer tmp;

  while (1) {
    l = LEFT(i);
    r = RIGHT(i);

    if (l < self->heap_size && self->cmp(self->array[l], self->array[i], self->cmp_data) > 0)
      largest = l;
    else
      largest = i;

    if (r < self->heap_size && self->cmp(self->array[r], self->array[largest], self->cmp_data) > 0)
      largest = r;

    if (largest == i)
      return;

    /* Exchange largest and i */
    tmp = self->array[largest];
    self->array[largest] = self->array[i];
    self->array[i] = tmp;

    i = largest;
  }
}

gpointer heap_get_maximum(Heap *self) {
  if (self->heap_size < 1)
    g_log(G_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "Heap underflow");

  {
    int i;
    for (i=1; i<self->heap_size; i++)
      if (self->cmp(self->array[0], self->array[i] ,self->cmp_data) > 1) {
	printf("Eep, %d is larger than the 'maximum' \n", i);
      }
  }

  return self->array[0];
}

gpointer heap_extract_maximum(Heap *self) {
  gpointer extracted = heap_get_maximum(self);
  self->array[0] = self->array[self->heap_size-1];
  self->heap_size--;
  heap_max_heapify(self, 0);
  return extracted;
}

void heap_insert(Heap *self, gpointer item) {
  /* This is actually a combination of the MAX-HEAP-INSERT
   * and HEAP-INCREASE-KEY algorithms. This module doesn't
   * yet have a separate function for increasing the key
   * of an item, since it doesn't have a fast way to
   * refer to a particular item from outside this module.
   */
  heap_index_t i;
  gpointer tmp;

  self->heap_size++;
  if (self->heap_size > self->array_size)
    g_log(G_LOG_DOMAIN, G_LOG_LEVEL_ERROR, "Heap overflow");

  i = self->heap_size - 1;
  self->array[i] = item;

  while (i > 0 && self->cmp(self->array[PARENT(i)], self->array[i], self->cmp_data) < 0) {
    tmp = self->array[PARENT(i)];
    self->array[PARENT(i)] = self->array[i];
    self->array[i] = tmp;

    i = PARENT(i);
  }
}

/* The End */

/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * bptree.h - A somewhat generic B+-Tree implementation used by rtgraph
 *            channels internally. Keys and values are stored in the nodes
 *            themselves, and are arbitrary but fixed sizes. User-defined
 *            comparison and allocation functions are used. Nodes are allocated
 *            as pages in an RtgPageStorage, for optional disk backing.
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

#include <glib.h>
#include "pagestorage.h"
#include "pageatom.h"

G_BEGIN_DECLS

typedef struct _RtgBPTree       RtgBPTree;
typedef struct _RtgBPIter       RtgBPIter;

/* The B+-Tree object itself. This contains pointers that can't be
 * stored persistently, so it must always be recreated every time
 * a B+-Tree is loaded.
 */
struct _RtgBPTree {
    /* Data types */
    gsize      sizeof_key;
    gsize      sizeof_value;
    GCompareDataFunc compare;
    gpointer   compare_user_data;

    /* Offsets and lengths within our index nodes */
    struct {
	int parent_offset;  /* parent index node address */
	int count_offset;   /* current number of children (one greater than the number of keys) */
	int keys_offset;
	int keys_count;
	int child_offset;
	int child_count;
    } index;

    /* Offsets and lengths within our leaf nodes */
    struct{
	int prev_offset;    /* previous leaf address */
	int next_offset;    /* next leaf address */
	int parent_offset;  /* parent index node address */
	int origin_offset;  /* position of first item in the wraparound key/value arrays */
	int count_offset;   /* count of key/value pairs currently stored */
	int keys_offset;
	int values_offset;
	int key_value_count;
    } leaf;

    /* Our storage, and atoms referring to our important pages. */
    RtgPageStorage *storage;
    RtgPageAtom     root, first_leaf, last_leaf;

    /* A version stamp used to track iterator validity */
    gulong stamp;
};


/* An opaque structure representing a marked position in a B+-Tree.
 * It is invalidated by modifying the tree.
 */
struct _RtgBPIter {
    gulong          stamp;
    RtgPageAddress  leaf_page;
    int             leaf_index;
};

/* The function called by rtg_bptree_foreach */
typedef gboolean (*RtgBPTreeCallback)(gconstpointer key, gpointer value, gpointer user_data);


/************************************************************************************/
/******************************************************************* Public Methods */
/************************************************************************************/

/* Create a new RtgBPTree object, backed by pages in the given storage.
 * The tree is identified by a name string. If the tree already exists
 * in the storage, this will just refer to the existing data. If no such
 * tree exists, this will create an empty one.
 *
 * Any key/value type may be used, as long as the sizes of each are provided
 * along with a comparison function. The keys and values are expected to be
 * small compared to the storage's page size. If larger data must be stored,
 * page addresses should be given as the keys/values with the real data stored
 * indirectly at those addresses.
 *
 * The data sizes given here must match the sizes used when creating the
 * page storage, however we have no way of checking this at runtime.
 */
RtgBPTree*        rtg_bptree_new                 (RtgPageStorage*   storage,
						  const char*       name,
						  gsize             sizeof_key,
						  gsize             sizeof_value,
						  GCompareDataFunc  compare,
						  gpointer          compare_user_data);

/* Delete the RtgBPTree object. This doesn't affect data in our page storage */
void              rtg_bptree_close               (RtgBPTree*        self);

/* Test an iterator's validity. Iterators are valid across storage
 * allocations, but not across tree modification.
 */
static inline
gboolean          rtg_bptree_iter_is_valid       (RtgBPTree*        self,
						  RtgBPIter*        iter)
{
    return self->stamp == iter->stamp;
}

/* Call a user-supplied function for each item in the tree. Return nonzero
 * to terminate iteration.
 */
void              rtg_bptree_foreach             (RtgBPTree*        self,
						  RtgBPTreeCallback func,
						  gpointer          user_data);

/* Add a new key/value pair. Multiple values with the same key may exist.
 * If 'iter' is non-NULL, it will be positioned at the new node very quickly.
 */
void              rtg_bptree_insert              (RtgBPTree*        self,
						  gconstpointer     key,
						  gconstpointer     value,
						  RtgBPIter*        iter);

/* Position an iterator at the first occurrance of a key exactly equal
 * to the given one.
 */
void              rtg_bptree_find                (RtgBPTree*        self,
						  gconstpointer     key,
						  RtgBPIter*        iter);

/* Position one iterator to the nearest item less than the given key,
 * and the other to the nearest item greater than the given key. Either
 * iterator may be NULL to disregard that value. If the given key exists
 * in the tree, both returned iterators will point to an instance of it.
 */
void              rtg_bptree_find_nearest        (RtgBPTree*        self,
						  gconstpointer     key,
						  RtgBPIter*        less,
						  RtgBPIter*        greater);

/* Seek an iterator to the first/last values in the tree */
void              rtg_bptree_first               (RtgBPTree*        self,
						  RtgBPIter*        iter);
void              rtg_bptree_last                (RtgBPTree*        self,
						  RtgBPIter*        iter);

/* Advance an itertator to the next/previous record
 */
void              rtg_bptree_prev                (RtgBPTree*        self,
						  RtgBPIter*        iter);
void              rtg_bptree_next                (RtgBPTree*        self,
 						  RtgBPIter*        iter);

/* Read the current key/value at an iterator. Returns NULL if
 * the iterator is invalid. The resulting pointer becomes invalid
 * if the tree is modified or if any allocations are performed in
 * the same RtgPageStorage.
 */
gconstpointer     rtg_bptree_read_key            (RtgBPTree*        self,
						  RtgBPIter*        iter);
gpointer          rtg_bptree_read_value          (RtgBPTree*        self,
						  RtgBPIter*        iter);

/* Replace the current value pointed to by an iterator */
void              rtg_bptree_write_value         (RtgBPTree*        self,
						  RtgBPIter*        iter,
						  gpointer          key);

/* Delete the current key and value pointed to by an iterator */
void              rtg_bptree_remove              (RtgBPTree*        self,
						  RtgBPIter*        iter);

G_END_DECLS

#endif /* __RTG_CHANNEL_H__ */

/* The End */

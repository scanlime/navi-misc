/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * bptree.c - A somewhat generic B+-Tree implementation used by rtgraph
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

#include <config.h>
#include <string.h>
#include "bptree.h"
#include "rtgmem.h"

#define INVALID_STAMP ((gulong)-1)

/* Node flags */
#define NODE_IS_LEAF  0x00000001


static int        index_search                   (RtgBPTree*     tree,
						  RtgPageAddress page,
						  gconstpointer  key);
static int        leaf_ringbuf_index             (RtgBPTree*     tree,
						  RtgPageAddress page,
						  int            i);
static int        leaf_last_index                (RtgBPTree*     tree,
						  RtgPageAddress page);
static int        leaf_search                    (RtgBPTree*     tree,
						  RtgPageAddress page,
						  gconstpointer  key);

static void       rtg_bptree_invalidate_iters    (RtgBPTree*        self);
static void       init_namespaced_page_atom      (RtgPageStorage*   storage,
						  RtgPageAtom*      atom,
						  const char*       tree_name,
						  const char*       page_name);
static gboolean   validate_iter                  (RtgBPTree*        self,
						  RtgBPIter*        iter);
static inline
gint              compare_keys                   (RtgBPTree*        self,
						  gconstpointer     a,
						  gconstpointer     b);
static void       rtg_bptree_init                (RtgBPTree*        self);


/************************************************************************************/
/******************************************************************* Node Accessors */
/************************************************************************************/

/* Our index and value nodes are dynamically proportioned based on the key, value,
 * and page sizes we use. These proportions are stored as offsets and array sizes
 * in the RtgBPTree object, and used later in these accessor functions.
 */

/* Macros for allocating space. A little preprocessor abuse makes the
 * structure of the index and leaf nodes easier to understand.
 */
#define ALLOC_SIZE(s)   offset; offset += RTG_ALIGN_CEIL(s)
#define ALLOC_TYPE(t)   ALLOC_SIZE(sizeof(t))

static inline
void              index_init                     (RtgBPTree*        self)
{
    int offset = 0;

    /* Reserve space for the flags field, common to
     * both leaf and index nodes.
     */
    offset += RTG_ALIGN_CEIL(sizeof(int));

    /* First get fixed-size parts out of the way */
    self->index.parent_offset = ALLOC_TYPE(RtgPageAddress);
    self->index.count_offset  = ALLOC_TYPE(int);

    /* Use the highest keys_count that will keep our total size under
     * or equal to the storage's page size. child_count is always keys_count+1,
     * since we need a key in between every child.
     *
     * The total size can then be calculated as:
     *
     *    offset + RTG_ALIGN_CEIL(sizeof_key * keys_count) +
     *       RTG_ALIGN_CEIL(sizeof(RtgPageAddress) * (keys_count+1)) <= page_size
     *
     * The alignment issue makes this inequality a pain to solve
     * directly. Instead, we assume worst-case alignment padding:
     *
     *    offset + 2*(G_MEM_ALIGN-1) + (sizeof_key + sizeof(RtgPageAddress))*
     *       keys_count + sizeof(RtgPageAddress) <= page_size
     *
     * This can be solved easily:
     */
    self->index.keys_count = (self->storage->page_size - offset - 2*(G_MEM_ALIGN-1)
			      - sizeof(RtgPageAddress)) / (self->sizeof_key + sizeof(RtgPageAddress));

    /* However, since we were conservative in the alignment assumption, we may actually
     * be able to fit more keys in each page. Try to increase key_count without
     * going over page_size.
     */
    while ((offset + RTG_ALIGN_CEIL(self->sizeof_key * (self->index.keys_count+1)) +
	    RTG_ALIGN_CEIL(sizeof(RtgPageAddress) * (self->index.keys_count+2))) <=
	   self->storage->page_size)
	self->index.keys_count++;

    /* In a B-Tree, we always need one more child pointer than key */
    self->index.child_count = self->index.keys_count + 1;

    self->index.keys_offset   = ALLOC_SIZE( self->sizeof_key * self->index.keys_count );
    self->index.child_offset  = ALLOC_SIZE( sizeof(RtgPageAddress) * self->index.child_count );

    g_assert(offset <= self->storage->page_size);
}

static inline
void              leaf_init                      (RtgBPTree*        self)
{
    int offset = 0;

    /* Reserve space for the flags field, common to
     * both leaf and index nodes.
     */
    offset += RTG_ALIGN_CEIL(sizeof(int));

    /* First get fixed-size parts out of the way */
    self->leaf.prev_offset   = ALLOC_TYPE(RtgPageAddress);
    self->leaf.next_offset   = ALLOC_TYPE(RtgPageAddress);
    self->leaf.parent_offset = ALLOC_TYPE(RtgPageAddress);
    self->leaf.origin_offset = ALLOC_TYPE(int);
    self->leaf.count_offset  = ALLOC_TYPE(int);

    /* As above, Use the highest keys_count that will keep our total size under
     * or equal to the storage's page size. keys_count and values_count are the same.
     *
     * The total size can then be calculated as:
     *
     *    offset + RTG_ALIGN_CEIL(sizeof_key * key_value_count) +
     *       RTG_ALIGN_CEIL(sizeof_value * key_value_count) <= page_size
     *
     * The alignment issue makes this inequality a pain to solve
     * directly. Instead, we assume worst-case alignment padding:
     *
     *    offset + 2*(G_MEM_ALIGN-1) + (sizeof_key + sizeof_value) *
     *       key_value_count <= page_size
     *
     * This can be solved easily:
     */
    self->leaf.key_value_count = (self->storage->page_size - offset - 2*(G_MEM_ALIGN-1)) /
	(self->sizeof_key + self->sizeof_value);

    /* However, since we were conservative in the alignment assumption, we may actually
     * be able to fit more keys in each page. Try to increase key_count without
     * going over page_size.
     */
    while ((offset + RTG_ALIGN_CEIL(self->sizeof_key * (self->leaf.key_value_count+1)) +
	    RTG_ALIGN_CEIL(self->sizeof_value * (self->leaf.key_value_count+1))) <=
	   self->storage->page_size)
	self->leaf.key_value_count++;

    self->leaf.keys_offset    = ALLOC_SIZE( self->sizeof_key * self->leaf.key_value_count );
    self->leaf.values_offset  = ALLOC_SIZE( self->sizeof_value * self->leaf.key_value_count );

    g_assert(offset <= self->storage->page_size);
}

/* A macro for accessing a dynamically positioned
 * structure element within a storage page
 */
#define DYNAMIC_STRUCT_ITEM(tree, page, offset_name, type) \
    ((type)(rtg_page_storage_lookup(tree->storage, page) + tree->offset_name##_offset))

/* Macros that refer to individual items in our dynamically
 * proportioned structures. By implementing these as macros,
 * they can be used as lvalues.
 */
#define node_flags(tree, page)     (*(int*)rtg_page_storage_lookup(tree->storage, page))

#define index_parent(tree, page)   (*DYNAMIC_STRUCT_ITEM(tree, page, index.parent, RtgPageAddress*))
#define index_count(tree, page)    (*DYNAMIC_STRUCT_ITEM(tree, page, index.count, int*))
#define index_key(tree, page, i)   (DYNAMIC_STRUCT_ITEM(tree, page, index.keys, gpointer) + (tree)->sizeof_key*(i))
#define index_child(tree, page, i) (DYNAMIC_STRUCT_ITEM(tree, page, index.child, RtgPageAddress*)[i])

#define leaf_prev(tree, page)      (*DYNAMIC_STRUCT_ITEM(tree, page, leaf.prev, RtgPageAddress*))
#define leaf_next(tree, page)      (*DYNAMIC_STRUCT_ITEM(tree, page, leaf.next, RtgPageAddress*))
#define leaf_parent(tree, page)    (*DYNAMIC_STRUCT_ITEM(tree, page, leaf.parent, RtgPageAddress*))
#define leaf_origin(tree, page)    (*DYNAMIC_STRUCT_ITEM(tree, page, leaf.origin, int*))
#define leaf_count(tree, page)     (*DYNAMIC_STRUCT_ITEM(tree, page, leaf.count, int*))
#define leaf_key(tree, page, i)    (DYNAMIC_STRUCT_ITEM(tree, page, leaf.keys, gpointer) + (tree)->sizeof_key*(i))
#define leaf_value(tree, page, i)  (DYNAMIC_STRUCT_ITEM(tree, page, leaf.values, gpointer) + (tree)->sizeof_value*(i))

static inline
gboolean          node_is_leaf                   (RtgBPTree*     tree,
						  RtgPageAddress page)
{
    return !!(node_flags(tree, page) & NODE_IS_LEAF);
}

static int        index_search                   (RtgBPTree*     tree,
						  RtgPageAddress page,
						  gconstpointer  key)
{
    /* In an index page, find the array index corresponding to the
     * child that should contain a particular key. This should never
     * fail, as all index pages should contain at least one child pointer.
     * This is implemented using a binary search.
     */

    struct {
	int i;
	int cmp;
    } range_a, range_b, center_low, center_high;

    /* Inclusive search range, in key indices.
     * Start with the whole page, and get initial comparison
     * results for the endpoints.
     */
    range_a.i = 0;
    range_b.i = index_count(tree, page) - 2;  /* index_count measures children, we need keys */

    if (range_b.i == -1) {
	/* Only one child, follow that */
	return 0;
    }

    /* Can't have empty index pages */
    g_assert(range_a.i < range_b.i);

    range_a.cmp = compare_keys(tree, index_key(tree, page, range_a.i), key);
    range_b.cmp = compare_keys(tree, index_key(tree, page, range_b.i), key);

    while (1) {

	if (range_a.cmp > 0)
	    return range_a.i;

	if (range_b.cmp <= 0)
	    return range_b.i + 1;

	/* Create indices on both sides of the split boundary, and
	 * get comparison results for both.
	 */
	center_low.i  = (range_a.i + range_b.i) >> 1;
	center_high.i = center_low.i + 1;

	center_low.cmp = compare_keys(tree, index_key(tree, page, center_low.i), key);
	center_high.cmp = compare_keys(tree, index_key(tree, page, center_high.i), key);

	/* Follow the split up or down */
	if (center_low.cmp <= 0 && center_high.cmp <= 0)
	    range_a = center_high;

	else if (center_low.cmp > 0 && center_high.cmp > 0)
	    range_b = center_low;

	/* At this point it should be the case that we're sitting right on
	 * the boundary. Return the child between center_low and center_high.
	 */
	else {
	    g_assert(center_low.cmp <= 0);
	    g_assert(center_high.cmp > 0);
	    return center_high.i;
	}
    }
}

static int        leaf_ringbuf_index             (RtgBPTree*     tree,
						  RtgPageAddress page,
						  int            i)
{
    /* Convert an index relative to the ring buffer origin into a physical
     * index within the leaf's key/value array.
     */
    return (leaf_origin(tree, page) + i) % tree->leaf.key_value_count;
}

static int        leaf_last_index                (RtgBPTree*     tree,
						  RtgPageAddress page)
{
    /* The complement to leaf_origin- gets the last index in use on a page */

    int count = leaf_count(tree, page);
    g_assert(count > 0);
    return leaf_ringbuf_index(tree, page, count-1);
}

static int        leaf_search                    (RtgBPTree*     tree,
						  RtgPageAddress page,
						  gconstpointer  key)
{
    /* Find the index of the last key in a particular leaf that is
     * less than or equal to the supplied key. This uses a binary
     * search. Returns -1 if no such keys are found (the leaf is
     * empty, or all items are greater than the key)
     */

    struct {
	int ring_index;
	int flat_index;
	int cmp;
    } range_a, range_b, center_low, center_high;

    /* Inclusive search range, in ring buffer indices.
     * Start with the whole leaf, and get initial comparison
     * results for the endpoints.
     */
    range_a.ring_index = 0;
    range_b.ring_index = leaf_count(tree, page) - 1;

    if (range_a.ring_index > range_b.ring_index) {
	/* Empty leaf- stop here, since range_b is invalid */
	return -1;
    }

    range_a.flat_index = leaf_ringbuf_index(tree, page, range_a.ring_index);
    range_b.flat_index = leaf_ringbuf_index(tree, page, range_b.ring_index);

    range_a.cmp = compare_keys(tree, leaf_key(tree, page, range_a.flat_index), key);
    range_b.cmp = compare_keys(tree, leaf_key(tree, page, range_b.flat_index), key);

    while (range_a.ring_index <= range_b.ring_index) {

	/* Down to one item? */
	if (range_a.ring_index == range_b.ring_index) {
	    if (range_a.cmp <= 0)
		return range_a.flat_index;
	    else
		return -1;
	}

	/* Create indices on both sides of the split boundary, and
	 * get flat indices and comparison results for both.
	 */
	center_low.ring_index  = (range_a.ring_index + range_b.ring_index) >> 1;
	center_high.ring_index = center_low.ring_index + 1;

	center_low.flat_index = leaf_ringbuf_index(tree, page, center_low.ring_index);
	center_high.flat_index = leaf_ringbuf_index(tree, page, center_high.ring_index);

	center_low.cmp = compare_keys(tree, leaf_key(tree, page, center_low.flat_index), key);
	center_high.cmp = compare_keys(tree, leaf_key(tree, page, center_high.flat_index), key);

	/* If both sides of the split are below or equal to our
	 * search key, choose the high side. This ensures that we'll find
	 * the largest key if several of them are equal.
	 */
	if (center_low.cmp <= 0 && center_high.cmp <= 0)
	    range_a = center_high;

	/* If both sides are above our search key, keep looking below */
	else if (center_low.cmp > 0 && center_high.cmp > 0)
	    range_b = center_low;

	/* Otherwise, it should be the case that we're sitting right on
	 * the boundary between <= and >, and center_low has our answer
	 */
	else {
	    g_assert(center_low.cmp <= 0);
	    g_assert(center_high.cmp > 0);
	    return center_low.flat_index;
	}
    }
    return -1;
}


/************************************************************************************/
/****************************************************************** Private Methods */
/************************************************************************************/

static void       rtg_bptree_invalidate_iters    (RtgBPTree*        self)
{
    self->stamp++;

    /* Skip over the value we've reserved for iters that are always invalid */
    if (self->stamp == INVALID_STAMP)
	self->stamp++;
}

static void       init_namespaced_page_atom      (RtgPageStorage*   storage,
						  RtgPageAtom*      atom,
						  const char*       tree_name,
						  const char*       page_name)
{
    /* Init a RtgPageAtom to hold a page address with a particular name,
     * namespaced within our tree name.
     */
    gchar *name = g_strdup_printf("%s.%s", tree_name, page_name);
    rtg_page_atom_find_or_create(storage, RTG_PAGE_HEADER, atom, name,
				 RtgPageAddress, RTG_PAGE_NULL);
    g_free(name);
}

static gboolean   validate_iter                  (RtgBPTree*        self,
						  RtgBPIter*        iter)
{
    if (rtg_bptree_iter_is_valid(self, iter)) {
	return TRUE;
    }
    else {
	g_warning("Invalid RtgBPIter %p in tree %p", iter, self);
	return FALSE;
    }
}

static inline
gint              compare_keys                   (RtgBPTree*        self,
						  gconstpointer     a,
						  gconstpointer     b)
{
    return self->compare(a, b, self->compare_user_data);
}

static void       rtg_bptree_init                (RtgBPTree*        self)
{
    /* Initialize a new bptree by creating a root and one leaf */

    RtgPageAddress root, leaf;

    root = rtg_page_storage_alloc(self->storage);
    leaf = rtg_page_storage_alloc(self->storage);

    rtg_page_atom_value(self->storage, &self->root,
			RtgPageAddress) = root;
    rtg_page_atom_value(self->storage, &self->first_leaf,
			RtgPageAddress) = leaf;
    rtg_page_atom_value(self->storage, &self->last_leaf,
			RtgPageAddress) = leaf;

    node_flags   (self, root)    = 0;
    index_parent (self, root)    = RTG_PAGE_NULL;
    index_count  (self, root)    = 1;
    index_child  (self, root, 0) = leaf;

    node_flags   (self, leaf)    = NODE_IS_LEAF;
    leaf_prev    (self, leaf)    = RTG_PAGE_NULL;
    leaf_next    (self, leaf)    = RTG_PAGE_NULL;
    leaf_parent  (self, leaf)    = root;
    leaf_origin  (self, leaf)    = 0;
    leaf_count   (self, leaf)    = 0;
}


/************************************************************************************/
/******************************************************************* Public Methods */
/************************************************************************************/

RtgBPTree*        rtg_bptree_new                 (RtgPageStorage*   storage,
						  const char*       name,
						  gsize             sizeof_key,
						  gsize             sizeof_value,
						  GCompareDataFunc  compare,
						  gpointer          compare_user_data)
{
    RtgBPTree* self = g_new0(RtgBPTree, 1);

    self->storage = storage;
    self->sizeof_key = sizeof_key;
    self->sizeof_value = sizeof_value;
    self->compare = compare;
    self->compare_user_data = compare_user_data;

    /* Init dynamically proportioned structures */
    index_init(self);
    leaf_init(self);

    /* Initialize atoms that link this tree to pages in storage */
    init_namespaced_page_atom(storage, &self->root, name, "root");
    init_namespaced_page_atom(storage, &self->first_leaf, name, "first_leaf");
    init_namespaced_page_atom(storage, &self->last_leaf, name, "last_leaf");

    /* If the root doesn't exist, this tree needs initialization */
    if (rtg_page_atom_value(storage, &self->root, RtgPageAddress) == RTG_PAGE_NULL)
	rtg_bptree_init(self);

    return self;
}

void              rtg_bptree_close               (RtgBPTree*        self)
{
    g_free(self);
}

void              rtg_bptree_foreach             (RtgBPTree*        self,
						  RtgBPTreeCallback func,
						  gpointer          user_data)
{
    RtgBPIter iter;

    rtg_bptree_first(self, &iter);
    while (rtg_bptree_iter_is_valid(self, &iter)) {

	if (func( rtg_bptree_read_key(self, &iter),
		  rtg_bptree_read_value(self, &iter),
		  user_data ))
	    break;

	rtg_bptree_next(self, &iter);
    }
}

void              rtg_bptree_first               (RtgBPTree*        self,
						  RtgBPIter*        iter)
{
    /* Start with the first leaf page */
    iter->leaf_page = rtg_page_atom_value(self->storage, &self->first_leaf, RtgPageAddress);
    g_assert(iter->leaf_page != RTG_PAGE_NULL);

    /* This leaf will be empty if the tree has no items in it */
    if (!leaf_count(self, iter->leaf_page)) {
	iter->stamp = INVALID_STAMP;
	return;
    }

    /* Start with the first index in this leaf's ring buffer */
    iter->leaf_index = leaf_origin(self, iter->leaf_page);

    /* Mark the iter as valid */
    iter->stamp = self->stamp;
}

void              rtg_bptree_last                (RtgBPTree*        self,
						  RtgBPIter*        iter)
{
    /* Start with the last leaf page */
    iter->leaf_page = rtg_page_atom_value(self->storage, &self->last_leaf, RtgPageAddress);
    g_assert(iter->leaf_page != RTG_PAGE_NULL);

    /* This leaf will be empty if the tree has no items in it */
    if (!leaf_count(self, iter->leaf_page)) {
	iter->stamp = INVALID_STAMP;
	return;
    }

    /* Find the last index in this leaf's ring buffer */
    iter->leaf_index = leaf_last_index(self, iter->leaf_page);

    /* Mark the iter as valid */
    iter->stamp = self->stamp;
}

void              rtg_bptree_prev                (RtgBPTree*        self,
						  RtgBPIter*        iter)
{
    if (!validate_iter(self, iter))
	return;

    if (iter->leaf_index == leaf_origin(self, iter->leaf_page)) {
	/* We're at the beginning of this leaf already. Head to the previous leaf. */

	iter->leaf_page = leaf_prev(self, iter->leaf_page);
	if (iter->leaf_page == RTG_PAGE_NULL) {
	    iter->stamp = INVALID_STAMP;
	    return;
	}

	/* Find the last index in this leaf's ring buffer */
	iter->leaf_index = leaf_last_index(self, iter->leaf_page);
    }
    else {
	/* Just hit the previous item in this leaf's ring buffer */

	if (iter->leaf_index == 0)
	    iter->leaf_index = self->leaf.key_value_count;
	else
	    iter->leaf_index--;
    }
}

void              rtg_bptree_next                (RtgBPTree*        self,
						  RtgBPIter*        iter)
{
    if (!validate_iter(self, iter))
	return;

    if (iter->leaf_index == leaf_last_index(self, iter->leaf_page)) {
	/* We're at the end of this leaf, head to the next one */

	iter->leaf_page = leaf_next(self, iter->leaf_page);
	if (iter->leaf_page == RTG_PAGE_NULL) {
	    iter->stamp = INVALID_STAMP;
	    return;
	}

	iter->leaf_index = leaf_origin(self, iter->leaf_page);
    }
    else {
	/* Just hit the next item in this leaf's ring buffer */

	if (iter->leaf_index == self->leaf.key_value_count-1)
	    iter->leaf_index = 0;
	else
	    iter->leaf_index++;
    }
}

gconstpointer     rtg_bptree_read_key            (RtgBPTree*        self,
						  RtgBPIter*        iter)
{
    if (!validate_iter(self, iter))
	return NULL;
    return leaf_key(self, iter->leaf_page, iter->leaf_index);
}

gpointer          rtg_bptree_read_value          (RtgBPTree*        self,
						  RtgBPIter*        iter)
{
    if (!validate_iter(self, iter))
	return NULL;
    return leaf_value(self, iter->leaf_page, iter->leaf_index);
}

void              rtg_bptree_write_value         (RtgBPTree*        self,
						  RtgBPIter*        iter,
						  gpointer          key)
{
    if (!validate_iter(self, iter))
	return;
    memcpy(leaf_value(self, iter->leaf_page, iter->leaf_index),
	   key, self->sizeof_value);
}

void              rtg_bptree_find                (RtgBPTree*        self,
						  gconstpointer     key,
						  RtgBPIter*        iter)
{
    /* Find an item with key <= our given key */
    rtg_bptree_find_nearest(self, key, iter, NULL);

    /* Didn't find an exact match? */
    if (rtg_bptree_iter_is_valid(self, iter) &&
	compare_keys(self, key, rtg_bptree_read_key(self, iter)) != 0)
	iter->stamp = INVALID_STAMP;
}

void              rtg_bptree_find_nearest        (RtgBPTree*        self,
						  gconstpointer     key,
						  RtgBPIter*        less,
						  RtgBPIter*        greater)
{
    RtgPageAddress page;
    int i;
    RtgBPIter iter;

    /* Descend through index pages until we hit a leaf */
    page = rtg_page_atom_value(self->storage, &self->root, RtgPageAddress);
    while (!node_is_leaf(self, page))
	page = index_child(self, page, index_search(self, page, key));

    /* Find the largest node <= our key */
    i = leaf_search(self, page, key);

    if (i < 0) {
	/* Search failed, return invalid iters */
	if (less)
	    less->stamp = INVALID_STAMP;
	if (greater)
	    greater->stamp = INVALID_STAMP;
	return;
    }

    /* Fill in our own iter for the node found by leaf_search.
     * This will become the 'less' return value if our caller
     * wants it. If our caller wants 'greater', that's just the next
     * value after this one.
     */
    iter.stamp = self->stamp;
    iter.leaf_page = page;
    iter.leaf_index = i;

    if (less)
	*less = iter;

    if (greater) {
	rtg_bptree_next(self, &iter);
	*greater = iter;
    }
}

void              rtg_bptree_insert              (RtgBPTree*        self,
						  gconstpointer     key,
						  gconstpointer     value,
						  RtgBPIter*        iter)
{

    rtg_bptree_invalidate_iters(self);
}

void              rtg_bptree_remove              (RtgBPTree*        self,
						  RtgBPIter*        iter)
{
    if (!validate_iter(self, iter))
	return;


    rtg_bptree_invalidate_iters(self);
}

/* The End */

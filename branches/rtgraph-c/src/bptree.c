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
#include "bptree.h"
#include "rtgmem.h"

#define INVALID_STAMP ((gulong)-1)


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


/************************************************************************************/
/****************************************************************** Private Methods */
/************************************************************************************/

#if 0
static void       rtg_bptree_invalidate_iters    (RtgBPTree*        self)
{
    self->stamp++;

    /* Skip over the value we've reserved for iters that are always invalid */
    if (self->stamp == INVALID_STAMP)
	self->stamp++;
}
#endif

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

    /* Create the root if it doesn't exist */
    if (rtg_page_atom_value(storage, &self->root, RtgPageAddress) == RTG_PAGE_NULL) {
	RtgPageAddress root = rtg_page_storage_alloc(storage);
	rtg_page_atom_value(storage, &self->root, RtgPageAddress) = root;

	index_parent(self, root) = RTG_PAGE_NULL;
	index_count(self, root) = 0;
    }

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
}

void              rtg_bptree_insert              (RtgBPTree*        self,
						  gpointer          key,
						  gpointer          value,
						  RtgBPIter*        iter)
{
}

void              rtg_bptree_find                (RtgBPTree*        self,
						  gpointer          key,
						  RtgBPIter*        iter)
{
}

void              rtg_bptree_find_nearest        (RtgBPTree*        self,
						  gpointer          key,
						  RtgBPIter*        less,
						  RtgBPIter*        greater)
{
}

void              rtg_bptree_first               (RtgBPTree*        self,
						  RtgBPIter*        iter)
{
}

void              rtg_bptree_last                (RtgBPTree*        self,
						  RtgBPIter*        iter)
{
}

void              rtg_bptree_prev                (RtgBPTree*        self,
						  RtgBPIter*        iter)
{
    if (!validate_iter(self, iter))
	return;

}

void              rtg_bptree_next                (RtgBPTree*        self,
						  RtgBPIter*        iter)
{
    if (!validate_iter(self, iter))
	return;

}

gpointer          rtg_bptree_read_key            (RtgBPTree*        self,
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

}

void              rtg_bptree_remove              (RtgBPTree*        self,
						  RtgBPIter*        iter)
{
    if (!validate_iter(self, iter))
	return;
}

/* The End */

/*
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

    index_init(self);

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

gpointer          rtg_bptree_prev                (RtgBPTree*        self,
						  RtgBPIter*        iter)
{
}

gpointer          rtg_bptree_next                (RtgBPTree*        self,
						  RtgBPIter*        iter)
{
}

gpointer          rtg_bptree_read_key            (RtgBPTree*        self,
						  RtgBPIter*        iter)
{
}

gpointer          rtg_bptree_read_value          (RtgBPTree*        self,
						  RtgBPIter*        iter)
{
}

void              rtg_bptree_write_value         (RtgBPTree*        self,
						  RtgBPIter*        iter,
						  gpointer          key)
{
}

void              rtg_bptree_remove              (RtgBPTree*        self,
						  RtgBPIter*        iter)
{
}

/* The End */

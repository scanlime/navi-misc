/*
 * pageatom.c - Page atoms are defined as simple, space-efficient,
 *              and unsorted/unindexed associations between strings
 *              and small pieces of data.
 *
 *              The strings are encouraged to be namespaced using '/'
 *              as a separator. The values are often page addresses,
 *              but may be anything that's small relative to a page size.
 *
 *              The key property of page atoms are that once they are
 *              allocated, they never change position. Page atoms
 *              can also occupy otherwise-wasted space in other pages.
 *
 *              These are named Atoms after the similar string to integer
 *              associations in the X window system.
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
#include <gtk/gtk.h>
#include "pageatom.h"

/* Any page that's part of the atom list is marked as such by this
 * footer included at the end of the page:
 */
struct atom_page_footer {
    int            offset;       /* First offset within this page we can use for atom storage */
    int            utilization;  /* Number of bytes actually in use, starting at offset */
    RtgPageAddress next;         /* All pages in this atom list form a circular linked list */
};

#define ATOM_LEN_SIZE   RTG_ALIGN_CEIL(sizeof(int))

/* The atoms are stored sequentially in each page, starting at
 * the footer's specified offset and ending before the footer itself.
 *
 * Each atom consists of:
 *   - an atom_size_t value, covering the atom's string and data
 *   - A null-terminated alignment-padded string. If this string
 *     is empty (begins with '\0') the atom is marking unused space
 *     and may be reclaimed.
 *   - The atom's data, also alignment-padded
 */


/***********************************************************************/
/*************************************************** Private Methods ***/
/***********************************************************************/

/* Addressing convenience macros */
#define page_address(page)            rtg_page_storage_lookup(storage, page)
#define page_item(page, offset, type) (*((type*)(page_address(page)+(offset))))
#define page_footer(page) \
    page_item(page, storage->page_size - \
    RTG_ALIGN_CEIL(sizeof(struct atom_page_footer)), struct atom_page_footer)

static size_t strnlen                    (const char* s,
					  size_t maxlen)
{
    /* This is available as a GNU extension, but we
     * use our own version for portability.
     */
    size_t len = 0;
    while (len<maxlen && *s) {
	len++;
	s++;
    }
    return len;
}

gboolean          rtg_page_atom_alloc_extend     (RtgPageStorage*   storage,
						  RtgPageAddress    initial,
						  RtgPageAddress*   page,
						  int*              offset,
						  int               length)
{
    /* Try to allocate an area of 'length' bytes by extending the
     * utilization of an existing page. This is the fastest method of
     * allocation, and the most common.
     * Returns TRUE if 'page' and 'offset' now point to an allocated
     * area of 'length' bytes, or FALSE if no pages could satisfy this request.
     */
    RtgPageAddress current = initial;

    while (1) {
	int max_utilization = storage->page_size - page_footer(current).offset -
	    RTG_ALIGN_CEIL(sizeof(struct atom_page_footer));

	/* Yay, this page has room */
	if (page_footer(current).utilization + length <= max_utilization) {
	    *page = current;
	    *offset = page_footer(current).utilization + page_footer(current).offset;
	    page_footer(current).utilization += length;
	}

	/* Next page, checking for wraparound */
	current = page_footer(current).next;
	if (current == initial)
	    return FALSE;
    }
}

gboolean          rtg_page_atom_alloc_reclaim    (RtgPageStorage*   storage,
						  RtgPageAddress    initial,
						  RtgPageAddress*   page,
						  int*              offset,
						  int               length)
{
    /* Try to allocate an area of 'length' bytes by recycling one or
     * more contiguous deleted atoms. This is slowest, but we should
     * try it before allocating new pages.
     */
    RtgPageAtom current, next;
    gboolean not_last_page;

    rtg_page_atom_iter_init(storage, initial, &current);
    next = current;
    do {
	not_last_page = rtg_page_atom_iter_next(storage, &next);

	/* 'current' is always a valid and unique page,
	 * 'next' is only valid if not_last_page is true.
	 */

	if (rtg_page_atom_get_key(storage, &current)[0] == '\0') {
	    /* This is a deleted atom */

	    /* If the next atom has also been deleted, and it's
	     * on the same page, combine the two. We do this even
	     * if not necessary for the current allocation request,
	     * because it's cheap to and will speed up future
	     * allocations.
	     */
	    while (not_last_page && current.page == next.page &&
		   rtg_page_atom_get_key(storage, &next)[0] == '\0') {

		/* Merge atoms by updating current's length to exactly
		 * encompass next's length. Get a new 'next' item.
		 */
		current.length += next.length + ATOM_LEN_SIZE;
		page_item(current.page, current.key_offset -
			  ATOM_LEN_SIZE, int) = current.length;

		not_last_page = rtg_page_atom_iter_next(storage, &next);
	    }

	    /* Is this atom exactly the right size? */
	    if (current.length + ATOM_LEN_SIZE == length) {
		*offset = current.key_offset - ATOM_LEN_SIZE;
		*page = current.page;
		return TRUE;
	    }

	    /* Is this atom large enough to split into one that's
	     * exactly the right size plus an extra deleted node?
	     * This threshold is a little arbitrary, but we want to
	     * make sure that the other node we split off isn't so tiny
	     * as to be completely unusable.
	     */
	    if (current.length >= length + G_MEM_ALIGN*4) {

		/* Write a new length word immediately after this allocation,
		 * and mark it as a deleted atom.
		 */
		page_item(current.page, current.key_offset + current.length,
			  int) = current.length - length;
		page_item(current.page, current.key_offset + current.length + 1,
			  char) = '\0';

		*offset = current.key_offset - ATOM_LEN_SIZE;
		*page = current.page;
	    }
	}

	current = next;
    } while (not_last_page);

    return FALSE;
}


void              rtg_page_atom_alloc_new_page   (RtgPageStorage*   storage,
						  RtgPageAddress    initial,
						  RtgPageAddress*   page,
						  int*              offset,
						  int               length)
{
    /* Try to allocate an area of 'length' bytes by creating a new page.
     * This should always work, but it's our last resort.
     * Note that this actually ends up placing the new page just after
     * the initial page. This is easiest to do, but may have odd performance
     * impacts. Of course, this whole data structure isn't designed for
     * speed.
     */
    RtgPageAddress current;

    current = rtg_page_storage_alloc(storage);
    rtg_page_atom_add_page(storage, initial, current, 0);

    *page = current;
    *offset = page_footer(current).utilization + page_footer(current).offset;
    page_footer(current).utilization += length;

    g_assert(page_footer(current).utilization + page_footer(current).offset +
	     RTG_ALIGN_CEIL(sizeof(struct atom_page_footer)) < storage->page_size);
}


/***********************************************************************/
/**************************************************** Public Methods ***/
/***********************************************************************/

void              rtg_page_atom_iter_init        (RtgPageStorage*   storage,
						  RtgPageAddress    initial,
						  RtgPageAtom*      iter)
{
    iter->initial_page = initial;
    iter->page = initial;
}

gboolean          rtg_page_atom_iter_next        (RtgPageStorage*   storage,
						  RtgPageAtom*      iter)
{
    int offset;
    int offset_limit;
    int length_size;
    int key_size;
    RtgPageAddress next;

    /* Point to the length byte after the current atom */
    offset = iter->value_offset + iter->value_length;

    /* Loop until we have a valid offset... this will skip empty pages */
    while (1) {
	/* Have we exited the portion of this page used for atoms? */
	offset_limit = page_footer(iter->page).offset + page_footer(iter->page).utilization;
	if (offset < offset_limit)
	    break;

	/* Next page, if we aren't wrapping around */
	next = page_footer(iter->page).next;
	if (next == iter->initial_page) {
	    /* Can't go anywhere without wrapping around */
	    return FALSE;
	}
	iter->page = next;

	/* Point to the beginning of this page's atom list */
	offset = page_footer(iter->page).offset;
    }

    /* Offset points to the length byte on our new atom */
    iter->length = page_item(iter->page, offset, int);
    length_size = ATOM_LEN_SIZE;
    offset += length_size;
    g_assert(offset < offset_limit);

    /* Now it points to the key string. Count it and advance past */
    iter->key_offset = offset;
    key_size = RTG_ALIGN_CEIL(strnlen(rtg_page_atom_get_key(storage, iter), offset_limit-offset) + 1);
    offset += key_size;
    g_assert(offset < offset_limit);

    /* Everything remaining in 'length' is the value */
    iter->value_offset = offset;
    iter->value_length = iter->length - (length_size + key_size);

    return TRUE;
}

const char*       rtg_page_atom_get_key          (RtgPageStorage*   storage,
						  RtgPageAtom*      iter)
{
    return &page_item(iter->page, iter->key_offset, char);
}

gpointer          rtg_page_atom_get_value        (RtgPageStorage*   storage,
						  RtgPageAtom*      iter)
{
    return page_address(iter->page) + iter->value_offset;
}

void              rtg_page_atom_set_value        (RtgPageStorage*   storage,
						  RtgPageAtom*      iter,
						  gpointer          value,
						  gsize             value_len)
{
    g_assert(value_len <= iter->value_length);
    memcpy(page_address(iter->page) + iter->value_offset, value, value_len);
}

void              rtg_page_atom_delete           (RtgPageStorage*   storage,
						  RtgPageAtom*      iter)
{
    page_item(iter->page, iter->key_offset, char) = '\0';
}

void              rtg_page_atom_new              (RtgPageStorage*   storage,
						  RtgPageAddress    initial,
						  RtgPageAtom*      iter,
						  const char*       key,
						  int               value_size)
{
    RtgPageAddress page;
    int offset;
    int atom_size, key_size;

    /* Empty strings are used to mark deleted nodes */
    g_assert(key[0] != '\0');

    /* Calculate the length of the contiguous area we'll need to allocate */
    key_size = RTG_ALIGN_CEIL(strlen(key)+1);
    value_size = RTG_ALIGN_CEIL(value_size);
    atom_size = ATOM_LEN_SIZE + key_size + value_size;

    /* Try our various atom allocation methods */
    if (!rtg_page_atom_alloc_extend(storage, initial, &page, &offset, atom_size)) {
	if (!rtg_page_atom_alloc_reclaim(storage, initial, &page, &offset, atom_size)) {
	    rtg_page_atom_alloc_new_page(storage, initial, &page, &offset, atom_size);
	}
    }

    /* Write out this atom's length and key */
    iter->initial_page = initial;
    iter->page = page;

    iter->length = key_size + value_size;
    page_item(page, offset, int) = iter->length;
    offset += ATOM_LEN_SIZE;

    iter->key_offset = offset;
    memset(page_address(page) + iter->key_offset, 0, key_size);
    strcpy(page_address(page) + iter->key_offset, key);
    offset += key_size;

    iter->value_offset = offset;
    iter->value_length = value_size;
}

gboolean          rtg_page_atom_find             (RtgPageStorage*   storage,
						  RtgPageAddress    initial,
						  RtgPageAtom*      iter,
						  const char*       key)
{
    /* Empty strings are used to mark deleted nodes */
    g_assert(key[0] != '\0');

    rtg_page_atom_iter_init(storage, initial, iter);

    do {
	if (!strcmp(rtg_page_atom_get_key(storage, iter), key))
	    return TRUE;
    } while (rtg_page_atom_iter_next(storage, iter));

    return FALSE;
}

void              rtg_page_atom_init_page        (RtgPageStorage*   storage,
						  RtgPageAddress    initial,
						  int               reserved)
{
    /* Fill in the page footer */
    page_footer(initial).offset = RTG_ALIGN_CEIL(reserved);
    page_footer(initial).utilization = 0;
    page_footer(initial).next   = initial;
}

void              rtg_page_atom_add_page         (RtgPageStorage*   storage,
						  RtgPageAddress    in_list,
						  RtgPageAddress    new_page,
						  int               reserved)
{
    /* Initialize the new page, then link it into our list */
    rtg_page_atom_init_page(storage, new_page, reserved);
    page_footer(new_page).next = page_footer(in_list).next;
    page_footer(in_list).next = new_page;
}

/* The End */

/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * pageatom.h - Page atoms are defined as simple, space-efficient,
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

#ifndef __RTG_PAGE_ATOM_H__
#define __RTG_PAGE_ATOM_H__

#include <glib.h>
#include "rtgmem.h"
#include "pagestorage.h"

G_BEGIN_DECLS

typedef struct _RtgPageAtom  RtgPageAtom;

/* The RtgPageAtom structure doesn't directly correspond to how
 * atoms are actually stored in a page, but it marks the position
 * of a particular atom and is used as an iterator.
 */
struct _RtgPageAtom {
    RtgPageAddress  initial_page;
    RtgPageAddress  page;
    int             length;
    int             key_offset;
    int             value_offset;
    int             value_length;
};


/***********************************************************************/
/**************************************************** Public Methods ***/
/***********************************************************************/

/* The pages in a particular atom list are circularly linked,
 * so any page can be used to initialize an iterator. The iterator
 * will point to the first atom in the provided page.
 */
void              rtg_page_atom_iter_init        (RtgPageStorage*   storage,
						  RtgPageAddress    initial,
						  RtgPageAtom*      iter);

/* Move an iterator forward by one. Returns FALSE and stops iteration if
 * we would be wrapping around to the other side of the circular list.
 */
gboolean          rtg_page_atom_iter_next        (RtgPageStorage*   storage,
						  RtgPageAtom*      iter);

/* Return the string key for the current atom. If the iterator
 * points to an atom marking unused space, this returns
 * the empty string. The returned pointer is invalid
 * across RtgPageStorage reallocations.
 */
const char*       rtg_page_atom_get_key          (RtgPageStorage*   storage,
						  RtgPageAtom*      iter);

/* Get a pointer to the current atom's value. The value's length can't
 * change. The user may retrieve the value length using iter->value-length,
 * but that will be a padded length used for storage purposes.
 * The user can set a key by writing to the returned pointer.
 */
gpointer          rtg_page_atom_get_value        (RtgPageStorage*   storage,
						  RtgPageAtom*      iter);

/* A convenience function for setting the current atom's value. The size
 * of the provided data must be provided, as it may be smaller than the space
 * actually allocated.
 */
void              rtg_page_atom_set_value        (RtgPageStorage*   storage,
						  RtgPageAtom*      iter,
						  gpointer          value,
						  gsize             value_len);

/* A macro turning an atom value into a C lvalue of the given type */
#define           rtg_page_atom_value(storage, atom, type) \
    (*(type*)rtg_page_atom_get_value(storage, atom))

/* Delete the current atom, marking it as free space */
void              rtg_page_atom_delete           (RtgPageStorage*   storage,
						  RtgPageAtom*      iter);

/* Starting at the given initial page, find space for and allocate
 * a new atom with the given name and value size. Sets 'iter' to point
 * to the new atom. The atom's initial value is undefined.
 */
void              rtg_page_atom_new              (RtgPageStorage*   storage,
						  RtgPageAddress    initial,
						  RtgPageAtom*      iter,
						  const char*       key,
						  int               value_size);

/* Starting at the given initial page, find an atom with the
 * given key. Returns TRUE if the atom was found successfully,
 * and points 'iter' to it.
 */
gboolean          rtg_page_atom_find             (RtgPageStorage*   storage,
						  RtgPageAddress    initial,
						  RtgPageAtom*      iter,
						  const char*       key);

/* A macro to initialize an iter to an atom with the given key. If an existing
 * atom exists, it is found. If not, this evaluates the 'default' expression
 * and creates a new atom of the specified type.
 */
#define           rtg_page_atom_find_or_create(storage, initial, iter, key, type, default) \
    { \
        if (!rtg_page_atom_find(storage, initial, iter, key)) { \
	  rtg_page_atom_new(storage, initial, iter, key, sizeof(type)); \
	  rtg_page_atom_value(storage, iter, type) = (default); \
	} \
    } while (0);

/* Create a new atom list, using the supplied page address.
 * Optionally reserves a number of bytes at the beginning of
 * the page, that the atom list won't touch.
 */
void              rtg_page_atom_init_page        (RtgPageStorage*   storage,
						  RtgPageAddress    initial,
						  int               reserved);

/* Add a new page to an existing atom list, given any page
 * address in that atom list.
 */
void              rtg_page_atom_add_page         (RtgPageStorage*   storage,
						  RtgPageAddress    in_list,
						  RtgPageAddress    new_page,
						  int               reserved);

G_END_DECLS

#endif /* __RTG_PAGE_ATOM_H__ */

/* The End */

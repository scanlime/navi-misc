/*
 * pagestorage.h - Defines a simple way to allocate, free, and
 *                 address fixed-size blocks of data. This data may
 *                 exist in RAM only, or it may be a memory-mapped
 *                 file on disk.
 *
 *                 Much of the front-end functions are common to
 *                 all types of PageStorage, but multiple implementations
 *                 are defined, accessed using different constructors:
 *
 *                   rtg_page_storage_temp_new() creates an empty
 *                   PageStorage that exists only in memory.
 *
 *                   rtg_page_storage_mapped_new() creates a PageStorage
 *                   backed by a memory-mapped file with the given name.
 *                   There are separate implementations of this for
 *                   POSIX and win32 platforms.
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

#ifndef __RTG_PAGE_STORAGE_H__
#define __RTG_PAGE_STORAGE_H__

#include <gtk/gtk.h>
#include "rtgmem.h"

G_BEGIN_DECLS

typedef RtgAddress RtgPageAddress;

/* This can be returned on failure, or used as a list delimiter */
#define RTG_PAGE_NULL     ((RtgPageAddress) -1)

/* The first page is reserved as a header */
#define RTG_PAGE_HEADER   ((RtgPageAddress) 0)

typedef struct _RtgPageStorage  RtgPageStorage;

struct _RtgPageStorage {
    gpointer        base_address;
    gsize           page_size;
    gsize           num_pages;
    RtgPageAddress  page_addr_max;

    /* Extra amount to allocate, relative to what's absolutely necessary,
     * when we need to grow the size of the storage. This is initialized
     * to a default value, but the user may modify it.
     */
    gfloat    grow_margin;

    /* Resize to be at least new_num_pages large. This must fill in num_pages
     * with the actual new size before returning.
     */
    void  (*resize)(RtgPageStorage *self, gsize new_num_pages);
    void  (*close)(RtgPageStorage *self);

    /* Implementation-specific data may be placed afterwards
     * by embedding this structure in an implementation-specific one.
     */
};


/***********************************************************************/
/**************************************************** Public Methods ***/
/***********************************************************************/

/* Converting page addresses into pointers. These pointers should
 * not be stored generally, as some backends may change base addresses
 * during page allocation.
 */
#define           rtg_page_storage_lookup(self, page) \
    (((page) <= (self)->page_addr_max) ? ((self)->base_address + (page)) : NULL)

/* Page allocation */
RtgPageAddress    rtg_page_storage_alloc         (RtgPageStorage*   self);
void              rtg_page_storage_free          (RtgPageStorage*   self,
						  RtgPageAddress    page);

/* Implementation-independent destructor */
void              rtg_page_storage_close         (RtgPageStorage*   self);

/* Implementation-specific constructors. The page size can
 * be set manually, or you can pass zero to use the most efficient
 * size for this platform. Note that when opening an existing mapped
 * storage, the saved page size is always used rather than a specified
 * page size.
 */
RtgPageStorage*   rtg_page_storage_temp_new      (gsize             page_size);
RtgPageStorage*   rtg_page_storage_mapped_new    (const char*       filename,
						  gsize             page_size);

/* The first page in a pagestorage is used internally as a header.
 * It identifies the storage, points to the free block list, and holds
 * the first page in a global atom list used to identify other pages by name.
 *
 * The pageatom.h API can be used to access this list- use the initial
 * page address RTG_PAGE_HEADER.
 */


/***********************************************************************/
/*************************************************** Private Methods ***/
/***********************************************************************/

/* Validate an existing header. Returns nonzero on failure. */
int               rtg_page_storage_header_validate (RtgPageStorage* self);

/* Initialize an empty page storage */
void              rtg_page_storage_init            (RtgPageStorage* self);

/* Internal resize function that determines the actual
 * new size, calls an implementation-specific resize function,
 * and initializes/destroys pages as necessary.
 */
void              rtg_page_storage_resize          (RtgPageStorage* self,
						    gsize           new_num_pages);

/* Used by implementation constructors and resize backends.
 * This doesn't resize anything, it just changes the stored
 * size and maximum address.
 */
void              rtg_page_storage_set_num_pages   (RtgPageStorage* self,
						    gsize           new_num_pages);

static inline
RtgPageAddress    rtg_page_num_to_address          (RtgPageStorage* self,
						    gsize           page_num)
{
  return page_num * self->page_size;
}

G_END_DECLS

#endif /* __RTG_PAGE_STORAGE_H__ */

/* The End */

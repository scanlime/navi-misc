/*
 * pagestorage.c - Defines a simple way to allocate, free, and
 *                 address fixed-size blocks of data. This data may
 *                 exist in RAM only, or it may be a memory-mapped
 *                 file on disk.
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
#include "pagestorage.h"
#include "pageatom.h"

/* The header page begins with this data structure that identifies
 * our implementation and points to the first and last free pages.
 * The rest of the space is used for a named page list.
 *
 * FILE_MAGIC identifies our file format and version.
 * ARCH_MAGIC identifies this architecture's word size and byte order.
 */
#define FILE_MAGIC   "RTGraph Database File v1"
#define ARCH_MAGIC   ((~0x12345678) + G_MEM_ALIGN)
struct header_page_prefix {
    char            file_magic[32];
    int             arch_magic;
    gsize           page_size;

    /* Currently we use a simple linked list LIFO of free pages.
     * This doesn't have any way of favoring lower page addresses
     * in allocation, so our storage will tend to grow if exposed
     * to certain use patterns. I don't expect this to be a problem
     * with rtgraph, so currently this favors simplicity.
     */
    RtgPageAddress  first_free;
};

/* Prefix stored in free pages, for maintaining our free page data structure */
struct free_page_prefix {
    RtgPageAddress  next;
};

#define PAGE_PREFIX(type, self, page) ((type*)rtg_page_storage_lookup(self, page))


/***********************************************************************/
/**************************************************** Public Methods ***/
/***********************************************************************/

RtgPageAddress    rtg_page_storage_alloc         (RtgPageStorage*   self)
{
    struct header_page_prefix* header_p =
	PAGE_PREFIX(struct header_page_prefix, self, RTG_PAGE_HEADER);
    struct free_page_prefix* free_p;
    RtgPageAddress allocated;

    if (header_p->first_free == RTG_PAGE_NULL) {
	/* No pages free. Resize our storage */
	rtg_page_storage_resize(self, self->num_pages + 1);

	/* Get a new header pointer, since the resize just invalidated ours */
	header_p = PAGE_PREFIX(struct header_page_prefix, self, RTG_PAGE_HEADER);
	g_assert(header_p->first_free != RTG_PAGE_NULL);
    }

    /* Allocate from the beginning of the free list */
    allocated = header_p->first_free;
    free_p = PAGE_PREFIX(struct free_page_prefix, self, header_p->first_free);
    header_p->first_free = free_p->next;
    return allocated;
}

void              rtg_page_storage_free          (RtgPageStorage*   self,
						  RtgPageAddress    page)
{
    struct header_page_prefix* header_p =
	PAGE_PREFIX(struct header_page_prefix, self, RTG_PAGE_HEADER);
    struct free_page_prefix* free_p =
	PAGE_PREFIX(struct free_page_prefix, self, page);

    /* Add to our free pages stack */
    free_p->next = header_p->first_free;
    header_p->first_free = page;
}

void              rtg_page_storage_close         (RtgPageStorage*   self)
{
    self->close(self);
}


/***********************************************************************/
/*************************************************** Private Methods ***/
/***********************************************************************/

int               rtg_page_storage_header_validate (RtgPageStorage* self)
{
    struct header_page_prefix* header_p =
	PAGE_PREFIX(struct header_page_prefix, self, RTG_PAGE_HEADER);

    if (strcmp(header_p->file_magic, FILE_MAGIC)) {
	g_warning("File doesn't appear to be an rtgraph database");
	return -1;
    }
    if (header_p->arch_magic != ARCH_MAGIC) {
	g_warning("rtgraph database is for an incompatible architecture");
	return -1;
    }
    self->page_size = header_p->page_size;

    return 0;
}

void              rtg_page_storage_init            (RtgPageStorage* self)
{
    struct header_page_prefix* header_p =
	PAGE_PREFIX(struct header_page_prefix, self, RTG_PAGE_HEADER);

    memset(header_p->file_magic, 0, sizeof(header_p->file_magic));
    strcpy(header_p->file_magic, FILE_MAGIC);
    header_p->arch_magic = ARCH_MAGIC;

    header_p->page_size = self->page_size;
    header_p->first_free = RTG_PAGE_NULL;

    /* Set up the header as the first page in an atom list.
     * The space used by the header itself is reserved, but the rest
     * of the page may hold atoms.
     */
    rtg_page_atom_init_page(self, RTG_PAGE_HEADER, sizeof(struct header_page_prefix));
}

void              rtg_page_storage_resize          (RtgPageStorage* self,
						    gsize           new_num_pages)
{
    gsize old_num_pages = self->num_pages;
    gulong page_num;

    if (new_num_pages > self->num_pages) {
	/* We're increasing the storage size. Give it some extra margin */
	new_num_pages *= 1 + self->grow_margin;
    }
    else if (new_num_pages < self->num_pages) {
	/* We don't support shrinking the storage yet */
	g_assert_not_reached();
    }
    else {
	/* Same size, nothing to do */
	return;
    }

    self->resize(self, new_num_pages);

    /* Mark new pages as free */
    for (page_num=old_num_pages; page_num<self->num_pages; page_num++)
	rtg_page_storage_free(self, rtg_page_num_to_address(self, page_num));
}

void              rtg_page_storage_set_num_pages   (RtgPageStorage* self,
						    gsize           new_num_pages)
{
    new_num_pages = MAX(new_num_pages, 1);
    self->num_pages = new_num_pages;
    self->page_addr_max = (new_num_pages-1) * self->page_size;
}


/***********************************************************************/
/******************************************** In-memory Page Storage ***/
/***********************************************************************/

static void       temp_storage_resize            (RtgPageStorage*   self,
						  gsize             new_num_pages)
{
    rtg_page_storage_set_num_pages(self, new_num_pages);
    self->base_address = g_realloc(self->base_address, self->num_pages * self->page_size);
}

static void       temp_storage_close             (RtgPageStorage*   self)
{
    g_free(self->base_address);
    g_free(self);
}

RtgPageStorage*   rtg_page_storage_temp_new      (gsize             page_size)
{
    RtgPageStorage* self = g_new0(RtgPageStorage, 1);

    /* Just some arbitrary default */
    if (page_size == 0)
      page_size = 1024;

    self->page_size = page_size;
    self->resize = temp_storage_resize;
    self->close = temp_storage_close;

    /* Start out with one page */
    self->base_address = g_malloc(page_size);
    rtg_page_storage_set_num_pages(self, 1);

    self->grow_margin = 0.5;

    rtg_page_storage_init(self);
    return self;
}


/* The End */

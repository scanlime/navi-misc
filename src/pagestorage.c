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

/* Every page used to store the named page list has this suffix.
 * That includes the header page, which also has a header page
 * prefix. This suffix tells us where in the page the list starts,
 * and where the next page is if there's more than one being
 * used to store the page list.
 *
 * Every page in the page list is an anlignment-padded null-terminated
 * string giving the name followed by an alignment-padded page address.
 */
struct named_page_table_suffix {
    int             offset;
    RtgPageAddress  next;
};

/* Prefix stored in free pages, for maintaining our free page data structure */
struct free_page_prefix {
    RtgPageAddress  next;
};

#define PAGE_PREFIX(type, self, page) ((type*)rtg_page_storage_lookup(self, page))


/***********************************************************************/
/************************************************** Name table utils ***/
/***********************************************************************/

struct name_table_iter {
    RtgPageAddress  current_page;
    int             offset;
};

/* These would be in the iter, except that it isn't safe to hold on
 * to pointers within pages. This will just expand to some simple pointer
 * math to regenerate them as necessary.
 */
#define NAME_TABLE_PAGE_ADDR   (rtg_page_storage_lookup(self, iter->current_page))
#define NAME_TABLE_SUFFIX      ((struct named_page_table_suffix*) \
	(NAME_TABLE_PAGE_ADDR + self->page_size - sizeof(struct named_page_table_suffix)))

static int    name_table_iter_check      (RtgPageStorage *self,
					  struct name_table_iter *iter)
{
    /* Check the validity of this iterator. Returns the number
     * of bytes remaining in the current page if the iterator
     * is valid, or a negative number if invalid.
     */
    if (iter->current_page == RTG_PAGE_NULL)
	return -1;
    return self->page_size - sizeof(struct named_page_table_suffix) -
	iter->offset;
}

static gboolean name_table_is_last_page  (RtgPageStorage *self,
					  struct name_table_iter *iter)
{
    return NAME_TABLE_SUFFIX->next == RTG_PAGE_NULL;
}

static void   name_table_iter_init       (RtgPageStorage *self,
					  struct name_table_iter *iter)
{
    /* Reset the whole iterator to the beginning of the header page */
    iter->current_page = RTG_PAGE_HEADER;
    iter->offset = NAME_TABLE_SUFFIX->offset;
}

static void   name_table_iter_next_page  (RtgPageStorage *self,
					  struct name_table_iter *iter)
{
    /* Move to the next page */
    iter->current_page = NAME_TABLE_SUFFIX->next;
    iter->offset = NAME_TABLE_SUFFIX->offset;
}

static char*  name_table_peek_string     (RtgPageStorage *self,
					  struct name_table_iter *iter)
{
    /* Read a string without advancing the offset or validating */
    return (char*) (NAME_TABLE_PAGE_ADDR + iter->offset);
}

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

static char*  name_table_read_string     (RtgPageStorage *self,
					  struct name_table_iter *iter)
{
    /* Read a padded string out of the name table, advancing the
     * offset within our current page.
     */
    char *current = (char*) (NAME_TABLE_PAGE_ADDR + iter->offset);
    int remaining = name_table_iter_check(self, iter);
    int length;

    if (remaining < 1)
	return NULL;
    length = strnlen(current, remaining);
    if (length == remaining)
	return NULL;
    iter->offset = RTG_ALIGN_CEIL(iter->offset + length + 1);
    return current;
}

static void  name_table_write_string     (RtgPageStorage *self,
					  struct name_table_iter *iter,
					  const char *str)
{
    /* Write a string at the current offset, and advance it. The
     * caller is expected to have already ensured there's enough room.
     */
    char *current = (char*) (NAME_TABLE_PAGE_ADDR + iter->offset);
    int length = RTG_ALIGN_CEIL(strlen(str) + 1);

    memset(current, 0, length);
    strcpy(current, str);

    iter->offset = iter->offset + length;
}

static RtgPageAddress*  name_table_read_address (RtgPageStorage *self,
						 struct name_table_iter *iter)
{
    /* Read a padded page address out of the name table, advancing the
     * offset within our current page.
     */
    RtgPageAddress *current = (RtgPageAddress*) (NAME_TABLE_PAGE_ADDR + iter->offset);
    int remaining = name_table_iter_check(self, iter);

    if (remaining < sizeof(RtgPageAddress))
	return NULL;
    iter->offset = RTG_ALIGN_CEIL(iter->offset + sizeof(RtgPageAddress));
    return current;
}

static void             name_table_write_address (RtgPageStorage *self,
						  struct name_table_iter *iter,
						  RtgPageAddress addr)
{
    /* Write a padded page address into the name table, advancing the
     * offset within our current page. The caller is expected to have
     * already ensured there's enough room.
     */
    RtgPageAddress *current = (RtgPageAddress*) (NAME_TABLE_PAGE_ADDR + iter->offset);

    memset(current, 0, RTG_ALIGN_CEIL(sizeof(RtgPageAddress)));
    *current = addr;
    iter->offset = RTG_ALIGN_CEIL(iter->offset + sizeof(RtgPageAddress));
}

static void            name_table_insert_page  (RtgPageStorage *self,
						struct name_table_iter *iter,
						RtgPageAddress newpage)
{
    /* Initialize a new page and append it after the current one.
     * The iterator is advanced to this new page.
     */
    RtgPageAddress next = NAME_TABLE_SUFFIX->next;
    NAME_TABLE_SUFFIX->next = newpage;
    name_table_iter_next_page(self, iter);
    NAME_TABLE_SUFFIX->next = next;
    NAME_TABLE_SUFFIX->offset = 0;
    iter->offset = 0;

    /* Put in a null terminator */
    memset(NAME_TABLE_PAGE_ADDR + iter->offset, 0, RTG_ALIGN_CEIL(1));
}

static void            name_table_init_header  (RtgPageStorage *self)
{
    struct name_table_iter _iter;
    struct name_table_iter* iter = &_iter;
    name_table_iter_init(self, iter);
    NAME_TABLE_SUFFIX->next = RTG_PAGE_NULL;
    NAME_TABLE_SUFFIX->offset = sizeof(struct header_page_prefix);

    /* Put in a null terminator */
    memset(NAME_TABLE_PAGE_ADDR + NAME_TABLE_SUFFIX->offset,
	   0, RTG_ALIGN_CEIL(1));
}

#undef NAME_TABLE_PAGE_ADDR
#undef NAME_TABLE_SUFFIX


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

struct find_name_context {
    const char *name;
    RtgPageAddress result;
};

static int        find_name_predicate            (RtgPageStorage* self,
						  const char* name,
						  RtgPageAddress page,
						  gpointer context)
{
    struct find_name_context* ctx = (struct find_name_context*) context;
    if (!strcmp(ctx->name, name)) {
	ctx->result = page;
	return 1;
    }
    return 0;
}

RtgPageAddress    rtg_page_storage_find_name     (RtgPageStorage*   self,
						  const char*       name)
{
    struct find_name_context ctx;
    ctx.name = name;
    ctx.result = RTG_PAGE_NULL;
    rtg_page_storage_iter_names(self, find_name_predicate, &ctx);
    return ctx.result;
}

void              rtg_page_storage_add_name      (RtgPageStorage*   self,
						  const char*       name,
						  RtgPageAddress    page)
{
    struct name_table_iter iter;
    int remaining, required;
    int name_len = strlen(name);
    name_table_iter_init(self, &iter);

    /* First just find the last name table page */
    while (!name_table_is_last_page(self, &iter))
	name_table_iter_next_page(self, &iter);

    /* Find the end of this page, leaving the iter
     * positioned at the null terminator.
     */
    while (name_table_peek_string(self, &iter)[0] != '\0') {
	name_table_read_string(self, &iter);
	name_table_read_address(self, &iter);
    }

    /* Is there enough room in this page for the new string,
     * address, and null terminator?
     */
    remaining = name_table_iter_check(self, &iter);
    required = RTG_ALIGN_CEIL(name_len + 1) +
	RTG_ALIGN_CEIL(sizeof(RtgPageAddress)) +
	RTG_ALIGN_CEIL(1);
    if (remaining < required)
	name_table_insert_page(self, &iter, rtg_page_storage_alloc(self));

    name_table_write_string(self, &iter, name);
    name_table_write_address(self, &iter, page);
    name_table_write_string(self, &iter, "");
}

void              rtg_page_storage_iter_names    (RtgPageStorage*       self,
						  RtgNamedPagePredicate callback,
						  gpointer              context)
{
    struct name_table_iter iter;
    name_table_iter_init(self, &iter);

    while (name_table_iter_check(self, &iter) > 0) {
	char* name;
	RtgPageAddress* page;

	/* Extract name */
	name = name_table_read_string(self, &iter);
	if (!name)
	    break;
	if (!name[0]) {
	    /* End marker for this page, try the next one */
	    name_table_iter_next_page(self, &iter);
	    continue;
	}

	/* Extract page address */
	page = name_table_read_address(self, &iter);
	if (!page)
	    break;

	if (callback(self, name, *page, context))
	    break;
    }
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

    name_table_init_header(self);
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

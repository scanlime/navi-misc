/*
 * pagestorage-posix.c - Platform-specific page storage backends for POSIX machines
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

#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "pagestorage.h"

/* Use structure embedding to store our private data */
typedef struct _RtgPageStorageMapped RtgPageStorageMapped;
struct _RtgPageStorageMapped {
    RtgPageStorage super;
    int fd;
    size_t map_length;
};

static void       mapped_storage_resize            (RtgPageStorage*   _self,
						    gsize             new_num_pages)
{
    RtgPageStorageMapped* self = (RtgPageStorageMapped*) _self;
    size_t old_length = self->map_length;

    self->map_length = RTG_ROUND_CEIL(new_num_pages * self->super.page_size, getpagesize());
    rtg_page_storage_set_num_pages(self, self->map_length / self->super.page_size);

    ftruncate(self->fd, self->map_length);
    self->super.base_address = mremap(self->super.base_address, old_length, self->map_length, 1);
    if (self->super.base_address == MAP_FAILED) {
	g_error("Memory remap failed");
    }
}

static void       mapped_storage_close             (RtgPageStorage*   _self)
{
    RtgPageStorageMapped* self = (RtgPageStorageMapped*) _self;

    if (self->super.base_address)
	munmap(self->super.base_address, self->map_length);
    if (self->fd > 0)
	close(self->fd);
    g_free(self);
}

RtgPageStorage*   rtg_page_storage_mapped_new      (const char*       filename,
						    gsize             page_size)
{
    RtgPageStorageMapped* self = g_new0(RtgPageStorageMapped, 1);
    struct stat st;
    gboolean new_file;
    int os_page_size = getpagesize();

    /* The page size must be a power of two, for our rounding
     * operators. This will almost certainly be a power of two for
     * hardware reasons, but catch it now if this isn't the case.
     */
    g_assert(RTG_IS_POWER2(os_page_size));

    /* Use mmap()'s preferred page size if possible */
    if (page_size == 0)
	page_size = os_page_size;
    self->super.page_size = page_size;

    self->super.resize = mapped_storage_resize;
    self->super.close = mapped_storage_close;

    /* Does this file already exist? */
    if (stat(filename, &st) < 0) {
	if (errno != ENOENT) {
	    /* Something else went wrong */
	    rtg_page_storage_close(&self->super);
	    g_warning("Can't stat file while opening rtgraph mapped storage: %s", filename);
	    return NULL;
	}
	new_file = TRUE;
    }
    else {
	if (!S_ISREG(st.st_mode)) {
	    rtg_page_storage_close(&self->super);
	    g_warning("rtgraph mapped storage '%s' is not a regular file", filename);
	    return NULL;
	}
	new_file = FALSE;
    }

    self->fd = open(filename, O_CREAT | O_RDWR, 0666);
    if (self->fd <= 0) {
	rtg_page_storage_close(&self->super);
	g_warning("Can't open rtgraph database '%s'", filename);
	return NULL;
    }

    /* Determine the initial mmap() length. For new files, we
     * already know the page size, and we only want one of them.
     * For existing files, map the whole thing.
     */
    if (new_file)
	self->map_length = page_size;
    else
	self->map_length = st.st_size;

    /* Round the map length up to the next full OS page */
    self->map_length = RTG_ROUND_CEIL(self->map_length, os_page_size);

    /* Extend the file size. This creates a sparse file if the
     * underlying filesystem supports it.
     */
    ftruncate(self->fd, self->map_length);

    self->super.base_address = mmap(NULL, self->map_length,
				    PROT_READ | PROT_WRITE, MAP_SHARED, self->fd, 0);
    if (self->super.base_address == MAP_FAILED) {
	rtg_page_storage_close(&self->super);
	g_warning("Can't perform initial memory map of size %d on rtgraph database '%s'",
		  self->map_length, filename);
	return NULL;
    }

    /* Check the header, if this is an old file.
     * If it's a new file, init the header. We need a temporary length
     * of at least one page so we can address the header.
     */
    rtg_page_storage_set_num_pages(self, 1);
    if (new_file) {
	rtg_page_storage_init(&self->super);
    }
    else {
	if (rtg_page_storage_header_validate(&self->super) < 0) {
	    rtg_page_storage_close(&self->super);
	    g_warning("Invalid header on rtgraph database '%s'", filename);
	    return NULL;
	}
    }

    /* Now that we know the page size (either the one set
     * above, or loaded from the file header) we can set the num_pages
     */
    rtg_page_storage_set_num_pages(self, self->map_length / self->super.page_size);

    /* Growing a file shouldn't be a costly operation, use a smallish growth margin */
    self->super.grow_margin = 0.1;

    return (RtgPageStorage*) self;
}

/* The End */

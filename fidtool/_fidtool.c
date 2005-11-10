/* -*- Mode: C; c-basic-offset: 4; indent-tabs-mode: nil -*-
 *
 * fidtool - C backend for working with Fast Interval Databases
 * Copyright (C) 2005 Micah Dowty <micah@navi.cx>
 *
 * The FID format is effectively a three-level skip list, with
 * good space-efficiency. Appends and interval queries are designed
 * to be very I/O efficient. The format is technically O(N) for
 * both queries and appends, however the coefficient involved is
 * very small. A typical FID requires 2 blocks of disk I/O for
 * a query on approximately 1 million samples. The asymptotic
 * time could be improved by using a variable number of skiplist
 * levels, but for the typical workloads FID was designed for, this
 * could actually hurt efficiency.
 *
 * This example shows a FID with 20 samples. Each "*" is a sample,
 * and the [] represent disk page boundaries. For simplicity,
 * each L0 page in this example holds exactly 4 samples, and
 * each L1 page holds 4 L0 pages plus its reverse-header.
 *
 *   L2 [                                           *             ...
 *   L1 [       *           *           *           * ] [       * ...
 *   L0 [ * * * * ] [ * * * * ] [ * * * * ] [ * * * * ] [ * * * * ]
 *        0 1 2 3     4 5 6 7     8 9 1 1     1 1 1 1     1 1 1 1    
 *                                    0 1     2 3 4 5     6 7 8 9
 *
 * Conceptually, this looks just like a skiplist. Each sample is actually
 * stored as a time delta from the previous sample, so it's important to
 * keep the three lists conceptually separate.
 *
 * The L0 skiplist is stored in a sequence of L0 pages. L0 pages store
 * only time deltas, encoded as variable-length integers. The first L0
 * page would begin with the delta from time zero to sample 0. It would
 * then encode the difference between sample 0 and 1, then the difference
 * between sample 1 and 2, and so on. The second L0 page would begin with
 * the difference between sample 3 and 4. The first L1 page would store a
 * delta between time zero and sample 3, then sample 3 to sample 7, and
 * so on.
 *
 * The L1 skiplist and L2 skiplist are both encoded in a sequence of L1
 * pages. The file starts out with a single L1 page, and a new L1 page is
 * inserted any time the previous one fills up. The on-disk sequence
 * for this example may be:
 *
 *   L1 L0 L0 L0 L0 L1 ...
 *
 * Most of the space in an L1 page is devoted to storing samples from
 * the L1 skiplist, but a small 'reverse-header' written backward from
 * the end of each L1 page stores a single sample from the L2 skiplist.
 *
 * While samples from the L0 skiplist only include time deltas, samples
 * from the L1 skiplist include both time deltas and sample counts. It's
 * important for the L1 list to indicate how many L0 samples are between
 * each pair of L1 samples, for performing interval queries.
 * 
 * L2 samples include a time delta, a count of L0 samples, and a count
 * of L1 samples. The L1 sample count, equal to a count of L0 pages, is
 * necessary in order to calculate the location on disk where the next
 * L1 page is to be found.
 *
 * Complete vs Incomplete pages:
 *
 *   The format described above applies to 'complete' pages. The last
 *   L0 page and the last L1 page in a file will always be considered
 *   'incomplete'.
 *
 *   An incomplete L0 page is not yet padded to a full PAGE_SIZE on disk.
 *   It may or may not have room for another sample. Indeed, a page may
 *   still be incomplete when it is full to exactly PAGE_SIZE with samples.
 *   An L0 page is only marked 'complete' once an append causes a new page
 *   to be created after it.
 *
 *   Incomplete L0 pages do not have an entry in the L1 skiplist yet.
 *
 *   Incomplete L1 pages do not yet have an entry in the L2 skiplist,
 *   meaning that they don't yet have their reverse-header. When an L0
 *   page becomes complete and it's time to append to the L1 skiplist,
 *   an L1 page becomes complete if it is out of space. (The definition
 *   of 'out of space' is somewhat implementation-dependent and tricky)
 *
 *   L1 pages are only ever completed when a new L0 page is about to be
 *   appended. After the reverse-header is written to the L1 page,
 *   signalling that it's complete, a new L1 page and a new L0 page
 *   (in that order) are opened.
 *
 * Integer encoding:
 *
 *   The variable length integers are formatted as follows, shown
 *   in binary:
 *
 *     x < 0x80              1xxxxxxx
 *     x < 0x4000            01xxxxxx xxxxxxxx
 *     x < 0x200000          001xxxxx xxxxxxxx xxxxxxxx
 *     x < 0x10000000        0001xxxx xxxxxxxx xxxxxxxx xxxxxxxx
 *     ...
 *     End-of-page mark      00000000
 *
 *   The largest integer length that can be represented is 56-bits,
 *   which will be prefixed by 0x01.
 *
 * Graphing:
 *
 *   This format is designed from the ground up for fast interactive
 *   graphing. More specifically, this implementation should be friendly
 *   to an AJAX-style graphing frontend in which Javascript code provides
 *   interactivity and renders graph scales, whereas the body of the graph
 *   is rendered by this library in small tiles.
 *
 *   To keep our graph rendering operations fast and produce the smallest
 *   output possible, we directly render to palettized PNGs. This library
 *   only knows how to render a background grid and graph data with simple
 *   built-in oversampling. Other graph elements must be composited separately
 *   by client-side code.
 *
 * License:
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <Python.h>

#define DEBUG

//#define PAGE_SHIFT 12
#define PAGE_SHIFT 5
#define PAGE_SIZE  (1 << PAGE_SHIFT)
#define PAGE_MASK  (PAGE_SIZE - 1)

#ifdef DEBUG
#define DBG(args...) fprintf(stderr, args)
#else
#define DBG(args...)
#endif

#if __GNUC__ >= 3
# define inline		inline __attribute__ ((always_inline))
# define __const	__attribute__ ((const))
# define likely(x)	__builtin_expect (!!(x), 1)
# define unlikely(x)	__builtin_expect (!!(x), 0)
#else
# define inline
# define __const
# define likely(x)	(x)
# define unlikely(x)	(x)
#endif

/************************************************************************/
/***************************************** Variable-length Integers *****/
/************************************************************************/

/* Valid samples can be up to 56 bits long.
 *
 * Negative sample values are only valid as special return codes
 * from our variable-integer layer. SAMPLE_INF is a general-purpose
 * 'infinity' value, typically used for seeking to the end of the FID.
 *
 * SAMPLE_NEED_RESET is for conveinience in initializing cursors.
 * It must be greater than SAMPLE_INF.
 */
typedef long long sample_t;
#define END_MARKER          ((sample_t) -1)
#define HIT_FENCE           ((sample_t) -2)
#define SAMPLE_INF          0x7FFFFFFFFFFFFFFELL
#define SAMPLE_NEED_RESET   0x7FFFFFFFFFFFFFFFLL

/* Read a sample forward, incrementing 'p' to point just past the end
 * of the sample on a successful read. If any byte in the sample would
 * have been read from 'fence', this does not change p and returns HIT_FENCE.
 * Memory will never be read from addresses greater than or equal to 'fence'.
 */
static inline sample_t
sample_read(unsigned char **p, unsigned char *fence)
{
    unsigned char *cp = *p;
    unsigned char c;

    if (cp >= fence)
        return HIT_FENCE;
    c = *cp;

    if (c & 0x80) {
        *p = cp + 1;
        return c & 0x7F;
    }

    if (c & 0x40) {
        if (cp + 1 >= fence)
            return HIT_FENCE;
        *p = cp + 2;
        return ((c & 0x3F) << 8) | cp[1];
    }

    if (c == 0)
        return END_MARKER;

    if (c & 0x20) {
        if (cp + 2 >= fence)
            return HIT_FENCE;
        *p = cp + 3;
        return ((c & 0x1F) << 16) | (cp[1] << 8) | cp[2];
    }

    if (c & 0x10) {
        if (cp + 3 >= fence)
            return HIT_FENCE;
        *p = cp + 4;
        return ((c & 0x0F) << 24) | (cp[1] << 16) | (cp[2] << 8) | cp[3];
    }

    if (c & 0x08) {
        if (cp + 4 >= fence)
            return HIT_FENCE;
        *p = cp + 5;
        return (((sample_t) c & 0x07) << 32) | (cp[1] << 24) |
            (cp[2] << 16) | (cp[3] << 8) | cp[4];
    }

    if (c & 0x04) {
        if (cp + 5 >= fence)
            return HIT_FENCE;
        *p = cp + 6;
        return (((sample_t) c & 0x03) << 40) | (((sample_t) cp[1]) << 32) |
            (cp[2] << 24) | (cp[3] << 16) | (cp[4] << 8) | cp[5];
    }

    if (c & 0x02) {
        if (cp + 6 >= fence)
            return HIT_FENCE;
        *p = cp + 7;
        return (((sample_t) c & 0x01) << 48) | (((sample_t) cp[1]) << 40) |
            (((sample_t) cp[2]) << 32) | (cp[3] << 24) | (cp[4] << 16) |
            (cp[5] << 8) | cp[6];
    }

    if (cp + 7 >= fence)
        return HIT_FENCE;
    *p = cp + 8;
    return (((sample_t) cp[1]) << 48) | (((sample_t) cp[2]) << 40) |
        (((sample_t) cp[3]) << 32) | (cp[4] << 24) | (cp[5] << 16) |
        (cp[6] << 8) | cp[7];
}

/* A reversed version of sample_read, where memory addresses move
 * downward rather than upward.
 */
static inline sample_t
sample_read_r(unsigned char **p, unsigned char *fence)
{
    unsigned char *cp = *p;
    unsigned char c;

    if (cp <= fence)
        return HIT_FENCE;
    c = *cp;

    if (c & 0x80) {
        *p = cp - 1;
        return c & 0x7F;
    }

    if (c & 0x40) {
        if (cp - 1 <= fence)
            return HIT_FENCE;
        *p = cp - 2;
        return ((c & 0x3F) << 8) | cp[-1];
    }

    if (c == 0)
        return END_MARKER;

    if (c & 0x20) {
        if (cp - 2 <= fence)
            return HIT_FENCE;
        *p = cp - 3;
        return ((c & 0x1F) << 16) | (cp[-1] << 8) | cp[-2];
    }

    if (c & 0x10) {
        if (cp - 3 <= fence)
            return HIT_FENCE;
        *p = cp - 4;
        return ((c & 0x0F) << 24) | (cp[-1] << 16) | (cp[-2] << 8) | cp[-3];
    }

    if (c & 0x08) {
        if (cp - 4 <= fence)
            return HIT_FENCE;
        *p = cp - 5;
        return (((sample_t) c & 0x07) << 32) | (cp[-1] << 24) |
            (cp[-2] << 16) | (cp[-3] << 8) | cp[-4];
    }

    if (c & 0x04) {
        if (cp - 5 <= fence)
            return HIT_FENCE;
        *p = cp - 6;
        return (((sample_t) c & 0x03) << 40) | (((sample_t) cp[-1]) << 32) |
            (cp[-2] << 24) | (cp[-3] << 16) | (cp[-4] << 8) | cp[-5];
    }

    if (c & 0x02) {
        if (cp - 6 <= fence)
            return HIT_FENCE;
        *p = cp - 7;
        return (((sample_t) c & 0x01) << 48) | (((sample_t) cp[-1]) << 40) |
            (((sample_t) cp[-2]) << 32) | (cp[-3] << 24) | (cp[-4] << 16) |
            (cp[-5] << 8) | cp[-6];
    }

    if (cp - 7 <= fence)
        return HIT_FENCE;
    *p = cp - 8;
    return (((sample_t) cp[-1]) << 48) | (((sample_t) cp[-2]) << 40) |
        (((sample_t) cp[-3]) << 32) | (cp[-4] << 24) | (cp[-5] << 16) |
        (cp[-6] << 8) | cp[-7];
}

/* Return the length, in bytes, necessary to store a sample.
 * This assumes a sample fits in our 56-bit limit.
 */
static inline __const int
sample_len(sample_t s)
{
    if (s < 0x80) return 1; /* This case also works for END_MARKER */
    if (s < 0x4000) return 2;
    if (s < 0x200000) return 3;
    if (s < 0x10000000) return 4;
    if (s < 0x0800000000LL) return 5;
    if (s < 0x040000000000LL) return 6;
    if (s < 0x02000000000000LL) return 7;
    return 8;
}

/* Write a sample at the provided address. This does not increment
 * the pointer, or perform any EOF checking. The sample may not
 * be END_MARKER.
 */
static inline void
sample_write(sample_t s, unsigned char *p)
{
    if (s < 0x80) {
        p[0] = 0x80 | s;
    }
    else if (s < 0x4000) {
        p[0] = 0x40 | (s >> 8);
        p[1] = s;
    }
    else if (s < 0x200000) {
        p[0] = 0x20 | (s >> 16);
        p[1] = s >> 8;
        p[2] = s;
    }
    else if (s < 0x10000000) {
        p[0] = 0x10 | (s >> 24);
        p[1] = s >> 16;
        p[2] = s >> 8;
        p[3] = s;
    }
    else if (s < 0x0800000000LL) {
        p[0] = 0x08 | (s >> 32);
        p[1] = s >> 24;
        p[2] = s >> 16;
        p[3] = s >> 8;
        p[4] = s;
    }
    else if (s < 0x040000000000LL) {
        p[0] = 0x04 | (s >> 40);
        p[1] = s >> 32;
        p[2] = s >> 24;
        p[3] = s >> 16;
        p[3] = s >> 8;
        p[4] = s;
    }
    else if (s < 0x02000000000000LL) {
        p[0] = 0x02 | (s >> 48);
        p[1] = s >> 40;
        p[2] = s >> 32;
        p[3] = s >> 24;
        p[4] = s >> 16;
        p[5] = s >> 8;
        p[6] = s;
    }
    else {
        p[0] = 0x01;
        p[1] = s >> 48;
        p[2] = s >> 40;
        p[3] = s >> 32;
        p[4] = s >> 24;
        p[5] = s >> 16;
        p[6] = s >> 8;
        p[7] = s;
    }
}

/* A reversed version of sample_write */
static inline void
sample_write_r(sample_t s, unsigned char *p)
{
    if (s < 0x80) {
        p[0] = 0x80 | s;
    }
    else if (s < 0x4000) {
        p[0] = 0x40 | (s >> 8);
        p[-1] = s;
    }
    else if (s < 0x200000) {
        p[0] = 0x20 | (s >> 16);
        p[-1] = s >> 8;
        p[-2] = s;
    }
    else if (s < 0x10000000) {
        p[0] = 0x10 | (s >> 24);
        p[-1] = s >> 16;
        p[-2] = s >> 8;
        p[-3] = s;
    }
    else if (s < 0x0800000000LL) {
        p[0] = 0x08 | (s >> 32);
        p[-1] = s >> 24;
        p[-2] = s >> 16;
        p[-3] = s >> 8;
        p[-4] = s;
    }
    else if (s < 0x040000000000LL) {
        p[0] = 0x04 | (s >> 40);
        p[-1] = s >> 32;
        p[-2] = s >> 24;
        p[-3] = s >> 16;
        p[-3] = s >> 8;
        p[-4] = s;
    }
    else if (s < 0x02000000000000LL) {
        p[0] = 0x02 | (s >> 48);
        p[-1] = s >> 40;
        p[-2] = s >> 32;
        p[-3] = s >> 24;
        p[-4] = s >> 16;
        p[-5] = s >> 8;
        p[-6] = s;
    }
    else {
        p[0] = 0x01;
        p[-1] = s >> 48;
        p[-2] = s >> 40;
        p[-3] = s >> 32;
        p[-4] = s >> 24;
        p[-5] = s >> 16;
        p[-6] = s >> 8;
        p[-7] = s;
    }
}

/************************************************************************/
/************************************************* Cached I/O Layer *****/
/************************************************************************/

typedef struct {
    unsigned char data[PAGE_SIZE];
    off_t offset;
    ssize_t size;
    int need_write;
    int need_read;
} fid_page;

typedef struct {
    int fd;
    off_t size;
    off_t offset;
} fid_file;

static int
fid_file_init(fid_file *self, int fd)
{
    self->fd = fd;
    self->offset = self->size = lseek(fd, 0, SEEK_END);
    if (self->offset == (off_t)-1) {
        PyErr_SetFromErrno(PyExc_IOError);
        return -1;
    }
    return 0;
}

static int
fid_file_seek(fid_file *self, off_t offset)
{
    if (self->offset != offset) {
        if (lseek(self->fd, offset, SEEK_SET) == (off_t)-1) {
            PyErr_SetFromErrno(PyExc_IOError);
            return -1;
        }
    }
    return 0;
}

static void
fid_page_init(fid_page *self)
{
    self->offset = 0;
    self->size = 0;
    self->need_read = 1;
    self->need_write = 0;
}

static int
fid_page_read(fid_page *self, fid_file *file)
{
    if (!self->need_read)
        return 0;

    if (fid_file_seek(file, self->offset) < 0)
        return -1;

    DBG("reading page at 0x%016llx\n", self->offset);

    self->size = read(file->fd, self->data, PAGE_SIZE);
    if (self->size < 0) {
        PyErr_SetFromErrno(PyExc_IOError);
        return -1;
    }
    file->offset += self->size;

    /* Zero out anything not present in the file yet */
    memset(self->data + self->size, 0, PAGE_SIZE - self->size);

    self->need_read = 0;
    return 0;
}

static int
fid_page_write(fid_page *self, fid_file *file)
{
    if (!self->need_write)
        return 0;

    if (fid_file_seek(file, self->offset) < 0)
        return -1;

    DBG("flushing page at 0x%016llx\n", self->offset);

    if (write(file->fd, self->data, self->size) != self->size) {
        PyErr_SetFromErrno(PyExc_IOError);
        return -1;
    }
    file->offset += self->size;

    self->need_write = 0;
    return 0;
}

/* Change a page's offset, dirtying and/or flushing if appropriate */
static int
fid_page_seek(fid_page *self, fid_file *file, off_t offset)
{
    if (offset != self->offset) {
        if (fid_page_write(self, file) < 0)
            return -1;

        self->offset = offset;
        self->size = 0;
        self->need_read = 1;
        self->need_write = 0;
    }
    return 0;
}

static void
fid_page_grow(fid_page *self, unsigned char *p)
{
    int size = p - self->data;
    if (size > self->size) {
        self->size = size;
        self->need_write = 1;
    }
}

/************************************************************************/
/******************************************** Skiplist Cursor Layer *****/
/************************************************************************/

typedef struct {
    sample_t sample;
    unsigned long sample_number;
} fid_list_cursor;

typedef struct {
    sample_t time_delta;
    unsigned long n_samples;
} fid_list_delta;

static void
fid_list_cursor_reset(fid_list_cursor *self)
{
    self->sample = 0;
    self->sample_number = 0;
}

/* Apply a fid_list_delta to the cursor, moving it forward */
static void
fid_list_cursor_advance(fid_list_cursor *self, fid_list_delta *delta)
{
    self->sample += delta->time_delta;
    self->sample_number += delta->n_samples;
}

/* Test whether advancing the cursor would cause it to cross our search key.
 *
 * Returns zero if the new sample would be greater than or equal to the
 * search key. At L1 and higher this means we don't advance, at L0 it's
 * a stopping condition indicating we should advance once then bail out.
 */
static int
fid_list_cursor_next(fid_list_cursor *self, fid_list_delta *delta, sample_t key)
{
    return (self->sample + delta->time_delta < key);
}


/************************************************************************/
/************************************************* FID Cursor Layer *****/
/************************************************************************/

typedef struct {
    fid_file file;
    
    /* Logical positions in all three skiplists */
    fid_list_cursor l2_cursor;
    fid_list_cursor l1_cursor;
    fid_list_cursor l0_cursor;

    fid_page l1_page;   /* Doubles as the current sample in the L2 list */
    fid_page l0_page;

    /* These both point to the first byte after the current sample.
     * Pointing to the beginning on one page has the same meaning as pointing
     * past the last sample on the previous page.
     */
    unsigned char *l1_sample; 
    unsigned char *l0_sample;
} fid_cursor;

static int
fid_cursor_init(fid_cursor *self, int fd)
{
    fid_page_init(&self->l0_page);
    fid_page_init(&self->l1_page);

    self->l0_cursor.sample = SAMPLE_NEED_RESET;
    self->l1_cursor.sample = SAMPLE_NEED_RESET;
    self->l2_cursor.sample = SAMPLE_NEED_RESET;

    return fid_file_init(&self->file, fd);
}

static int
fid_cursor_flush(fid_cursor *self)
{
    if (fid_page_write(&self->l0_page, &self->file) < 0)
        return -1;
    if (fid_page_write(&self->l1_page, &self->file) < 0)
        return -1;
    return 0;
}

static int
fid_cursor_seek_l2(fid_cursor *self, sample_t key)
{
    fid_list_delta l2_delta;

    /* If the seek is backwards, reset to the very beginning.
     * Note that a key less than our current sample isn't
     * guaranteed to return a sample before this one- but since
     * it isn't guaranteed *not* to, we have to reset the
     * search to be on the safe side. If this is ever a performance
     * problem we could cache the previous sample on each cursor,
     * but our usage patterns are intended not to seek backwards
     * during normal operation.
     */
    if (key < self->l2_cursor.sample) {
        DBG("L2 cursor reset\n");

        fid_list_cursor_reset(&self->l2_cursor);
        if (fid_page_seek(&self->l1_page, &self->file, 0) < 0)
            return -1;
        self->l1_cursor.sample = SAMPLE_NEED_RESET;
    }

    /* Moving the L2 cursor is tricky, since the cursor position
     * and the current sample (L1 page number) correspond in an
     * unintuitive way.
     *
     * After resetting a cursor, we're pointing to the first L1
     * page, and we're pointing before the first L2 sample.
     * Each iteration through this loop is a test to check whether
     * we can move forward.
     *
     * If we can't move forward (the first L2 sample, stored in
     * L1 page zero, is too big) we keep pointing at the first
     * L1 page and *before* the first L2 sample. Our L2 cursor
     * is still at time zero.
     *
     * However if we can move forward, we seek our L2 cursor
     * forward to the first L2 sample. The sample itself,
     * conceptually, lies inside the first L1 page. Our L2
     * sample pointer, however, will be set to the second
     * L1 page.
     */
    while (1) {
        unsigned char *p = &self->l1_page.data[PAGE_SIZE - 1];
        unsigned char *fence = self->l1_page.data;
        int n_pages;

        if (fid_page_read(&self->l1_page, &self->file) < 0)
            return -1;

        /* If this is an incomplete page, stop the seek */
        if (*p == 0x00)
            break;

        /* Read the reverse-header */
        l2_delta.time_delta = sample_read_r(&p, fence);
        l2_delta.n_samples = sample_read_r(&p, fence);
        n_pages = sample_read_r(&p, fence);

        if (fid_list_cursor_next(&self->l2_cursor, &l2_delta, key)) {
            /* Seek the L2 cursor ahead by n_pages L1 pages */
            fid_list_cursor_advance(&self->l2_cursor, &l2_delta);
            if (fid_page_seek(&self->l1_page, &self->file,
                              self->l1_page.offset + ((1 + n_pages) << PAGE_SHIFT)) < 0)
                return -1;

            /* Force the L1 cursor to sync to the L2 cursor */
            self->l1_cursor.sample = SAMPLE_NEED_RESET;
        }
        else
            break;
    }
    return 0;
}

static int
fid_cursor_seek_l1(fid_cursor *self, sample_t key)
{
    fid_list_delta l1_delta;

    if (fid_page_read(&self->l1_page, &self->file) < 0)
        return -1;

    /* If the seek is backwards, reset to the L2 cursor */
    if (key < self->l1_cursor.sample) {
        DBG("L1 cursor reset\n");

        /* Synchronize the L1 cursor to the current L2 sample (beginning of current page) */
        self->l1_cursor = self->l2_cursor;
        self->l1_sample = self->l1_page.data;

        /* Force the L0 cursor to sync up with this one */
        self->l0_cursor.sample = SAMPLE_NEED_RESET;
    }

    while (1) {
        unsigned char *p = self->l1_sample;

        l1_delta.time_delta = sample_read(&p, self->l1_page.data + PAGE_SIZE);
        l1_delta.n_samples = sample_read(&p, self->l1_page.data + PAGE_SIZE);

        /* Stop the seek if we hit the end of the page. This should only happen
         * on incomplete pages, as on complete pages we would have skipped to the
         * next L1 during the L2 seek.
         */
        if (l1_delta.time_delta < 0)
            break;

        if (fid_list_cursor_next(&self->l1_cursor, &l1_delta, key)) {
            /* Seek the L1 cursor forward */
            fid_list_cursor_advance(&self->l1_cursor, &l1_delta);
            self->l1_sample = p;

            /* Force the L0 cursor to sync to the L1 cursor */
            self->l0_cursor.sample = SAMPLE_NEED_RESET;
        }
        else
            break;
    }
    return 0;
}

static int
fid_cursor_seek_l0(fid_cursor *self, sample_t key)
{
    int next;
    fid_list_delta l0_delta;
    l0_delta.n_samples = 1;

    if (fid_page_read(&self->l0_page, &self->file) < 0)
        return -1;

    /* If the seek is backwards, reset to the L1 cursor */
    if (key < self->l0_cursor.sample) {
        DBG("L0 cursor reset\n");

        /* Calculate the proper L0 page offset by looking at how many
         * samples we've covered in the current L1 page since branching
         * off from the L2 cursor.
         */
        self->l0_cursor = self->l1_cursor;
        if (fid_page_seek(&self->l0_page, &self->file,
                          self->l1_page.offset +
                          ((self->l1_cursor.sample_number -
                            self->l2_cursor.sample_number + 1) << PAGE_SHIFT)) < 0)
            return -1;
        self->l0_sample = self->l0_page.data;
    }

    DBG("L0 cursor at (%lld, %lu)\n",
        self->l0_cursor.sample, self->l0_cursor.sample_number);

    do {
        unsigned char *p = self->l0_sample;

        l0_delta.time_delta = sample_read(&p, self->l0_page.data + self->l0_page.size);

        /* Stop the seek if we hit the end of the page. This should only 
         * happen on incomplete pages, for the same reason noted in
         * fid_cursor_seek_l1.
         */
        if (l0_delta.time_delta < 0) {
            DBG("Hit the end\n");
            break;
        }

        DBG("L0 hit a delta of %lld\n", l0_delta.time_delta);

        /* At the L0 level, it's OK to skip just past 'key'
         * as long as we're sure we hit the first sample greater
         * than or equal to our search key.
         */
        next = fid_list_cursor_next(&self->l0_cursor, &l0_delta, key);

        fid_list_cursor_advance(&self->l0_cursor, &l0_delta);
        self->l0_sample = p;

        DBG("L0 advanced to (%lld, %lu) key: %lld\n",
            self->l0_cursor.sample, self->l0_cursor.sample_number, key);
    } while (next);
    return 0;
}

/* Seek a fid_cursor to the first sample equal to or greater
 * than the supplied key. The key SAMPLE_INF can be used to seek
 * to the end of the file, where no sample exists yet.
 */
static int fid_cursor_seek(fid_cursor *self, sample_t key)
{
    if (fid_cursor_seek_l2(self, key) < 0)
        return -1;
    if (fid_cursor_seek_l1(self, key) < 0)
        return -1;
    if (fid_cursor_seek_l0(self, key) < 0)
        return -1;

    return 0;
}

/* Append a new sample. The fid_cursor must already be seeked
 * to SAMPLE_INF. It will still be seeked to SAMPLE_INF after
 * this executes.
 */
static int fid_cursor_append(fid_cursor *self, sample_t sample)
{
    fid_list_delta delta = {sample - self->l0_cursor.sample, 1};

    if (delta.time_delta < 0) {
        PyErr_SetString(PyExc_ValueError,
           "Sample is not greater than or equal to the previous sample");
        return -1;
    }

    /* If there's room in this L0 page, we have very little to do.
     * This is the most common case when appending many samples.
     */
    if (self->l0_sample + sample_len(delta.time_delta) <=
        self->l0_page.data + PAGE_SIZE) {

        sample_write(delta.time_delta, self->l0_sample);
        fid_list_cursor_advance(&self->l0_cursor, &delta);
        self->l0_sample += sample_len(delta.time_delta);
        fid_page_grow(&self->l0_page, self->l0_sample);
        return 0;
    }

    abort();
    return 0;
}


/************************************************************************/
/************************************** High-Level Python Interface *****/
/************************************************************************/

/* Appends a list of new samples to a fid file */
static PyObject* fid_append_samples(PyObject *self, PyObject *args) {
    PyObject *sequence, *item, *iterator = NULL;
    int fd;
    fid_cursor cursor;

    if (!PyArg_ParseTuple(args, "iO", &fd, &sequence))
        goto error;
    iterator = PyObject_GetIter(sequence);
    if (!iterator)
        goto error;

    if (fid_cursor_init(&cursor, fd) < 0)
        goto error;
    if (fid_cursor_seek(&cursor, SAMPLE_INF) < 0)
        goto error;

    /* For each item in our sequence... */
    while ((item = PyIter_Next(iterator))) {
        /* Safely decode as a sample_t */
        sample_t sample = PyLong_AsLongLong(item);
        Py_DECREF(item);
        if (sample == -1 && PyErr_Occurred())
            goto append_error;

        if (fid_cursor_append(&cursor, sample) < 0)
            goto append_error;
    }
    if (fid_cursor_flush(&cursor) < 0)
        goto error;

    Py_DECREF(iterator);
    Py_RETURN_NONE;

append_error:
    fid_cursor_flush(&cursor);
error:
    Py_XDECREF(iterator);
    return NULL;
}

/* Seek to each sample in the provided sequence, returning the associated index.
 */
static PyObject* fid_query_samples(PyObject *self, PyObject *args) {
    PyObject *sequence, *item, *iterator = NULL, *results = PyList_New(0);
    int fd;
    fid_cursor cursor;

    if (!PyArg_ParseTuple(args, "iO", &fd, &sequence))
        goto error;
    iterator = PyObject_GetIter(sequence);
    if (!iterator)
        goto error;

    if (fid_cursor_init(&cursor, fd) < 0)
        goto error;

    /* For each item in our sequence... */
    while ((item = PyIter_Next(iterator))) {
        /* Safely decode as a sample_t */
        sample_t sample = PyLong_AsLongLong(item);
        Py_DECREF(item);
        if (sample == -1 && PyErr_Occurred())
            goto error;

        if (fid_cursor_seek(&cursor, sample) < 0)
            goto error;

        item = PyInt_FromLong(cursor.l0_cursor.sample_number);
        PyList_Append(results, item);
        Py_DECREF(item);
    }

    Py_DECREF(iterator);
    return results;
error:
    Py_XDECREF(iterator);
    Py_XDECREF(results);
    return NULL;
}

static PyMethodDef module_methods[] = {
    { "append_samples",  (PyCFunction) fid_append_samples,  METH_VARARGS },
    { "query_samples",   (PyCFunction) fid_query_samples,   METH_VARARGS },
    {0}
};

PyMODINIT_FUNC init_fidtool(void)
{
    Py_InitModule("_fidtool", module_methods);
}

/* The End */

/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * avi-writer.c - A simple interface for writing uncompressed RGB frames to
 *                a Microsoft AVI video file.
 *
 * Fyre - rendering and interactive exploration of chaotic functions
 * Copyright (C) 2004-2007 David Trowbridge and Micah Dowty
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#include "avi-writer.h"
#include <string.h>

static void avi_writer_class_init           (AviWriterClass *klass);
static void avi_writer_init                 (AviWriter *self);

static void avi_writer_write_header_list    (AviWriter *self);
static void avi_writer_write_main_header    (AviWriter *self);
static void avi_writer_write_stream_header  (AviWriter *self);
static void avi_writer_write_stream_format  (AviWriter *self);
static void avi_writer_write_index          (AviWriter *self);

static void write_fourcc                    (FILE *f, const char *fourcc);
static void write_int32                     (FILE *f, gint32 i);
static void write_int16                     (FILE *f, gint16 i);

static void avi_writer_push_chunk           (AviWriter *self, const char *fourcc);
static void avi_writer_pop_chunk            (AviWriter *self);
static void avi_writer_push_header          (AviWriter *self, const char *fileType);
static void avi_writer_push_list            (AviWriter *self, const char *listType);
static void avi_writer_pop_chunk_with_index (AviWriter *self, int index_flags);

typedef struct {
    char fourcc[5];   /* The chunk's FOURCC code */
    long size_field;  /* The file offset of the beginning of the chunk header's size field */
    long data_start;  /* The file offset where data starts, for measuring chunk size */
} ChunkStackEntry;

typedef struct {
    char fourcc[5];   /* The chunk's FOURCC code */
    int flags;        /* AVIIF_* flags */
    long offset;      /* File offset- number of bytes between the end of "movi" and the end
		       * of the chunk's FOURCC code. The documentation is fuzzy, but this
		       * is how mencoder seems to treat the field.
		       */
    long size;        /* Size of the indexed chunk- should match the size stored in the
		       * chunk's header after it's fixed up.
		       */
} IndexQueueEntry;


/* Scale factor for video stream length and frame rate */
#define RATE_SCALE 1000

/* Header flags */
#define AVIF_HASINDEX           0x00000010
#define AVIF_MUSTUSEINDEX       0x00000020
#define AVIF_ISINTERLEAVED      0x00000100
#define AVIF_TRUSTCKTYPE        0x00000800
#define AVIF_WASCAPTUREFILE     0x00010000
#define AVIF_COPYRIGHTED        0x00020000

/* Index flags */
#define AVIIF_LIST              0x00000001
#define AVIIF_KEYFRAME          0x00000010
#define AVIIF_NOTIME            0x00000100
#define AVIIF_COMPUSE           0x0FFF0000


/************************************************************************************/
/**************************************************** Initialization / Finalization */
/************************************************************************************/

GType avi_writer_get_type(void) {
    static GType dj_type = 0;

    if (!dj_type) {
	static const GTypeInfo dj_info = {
	    sizeof(AviWriterClass),
	    NULL, /* base_init */
	    NULL, /* base_finalize */
	    (GClassInitFunc) avi_writer_class_init,
	    NULL, /* class_finalize */
	    NULL, /* class_data */
	    sizeof(AviWriter),
	    0,
	    (GInstanceInitFunc) avi_writer_init,
	};

	dj_type = g_type_register_static(G_TYPE_OBJECT, "AviWriter", &dj_info, 0);
    }

    return dj_type;
}

static void avi_writer_class_init(AviWriterClass *klass) {
}

static void avi_writer_init(AviWriter *self) {
}

AviWriter*  avi_writer_new(FILE       *file,
			   guint       width,
			   guint       height,
			   float       frame_rate) {
    AviWriter *self = AVI_WRITER(g_object_new(avi_writer_get_type(), NULL));

    self->file = file;
    self->width = width;
    self->height = height;
    self->frame_rate = frame_rate;

    /* Write out everything we need to before the movie data can start... */
    avi_writer_push_header(self, "AVI ");
    avi_writer_write_header_list(self);
    avi_writer_push_list(self, "movi");

    /* Right here, at the beginning of the 'movi' list, is where index
     * offsets are measured from.
     */
    self->index_queue = g_queue_new();
    self->index_origin_offset = ftell(self->file);

    return self;
}


/************************************************************************************/
/******************************************************************* Public Methods */
/************************************************************************************/

void avi_writer_append_frame (AviWriter *self, const GdkPixbuf *frame) {
    int x, y;
    guchar bgr[3];
    guchar *pixels = gdk_pixbuf_get_pixels(frame);
    guchar *row, *pixel;
    int width = gdk_pixbuf_get_width(frame);
    int height = gdk_pixbuf_get_height(frame);
    int rowstride = gdk_pixbuf_get_rowstride(frame);
    int n_channels = gdk_pixbuf_get_n_channels(frame);
    int padding = 0;

    g_assert(width == self->width);
    g_assert(height == self->height);

    /* Start an uncompressed video frame */
    avi_writer_push_chunk(self, "00db");

    /* Write out our image data bottom-up in BGR order. Yuck.
     * Each row is padded to the next 4-byte boundary.
     */
    row = pixels + rowstride * (height-1);
    for (y=self->height-1; y>=0; y--) {
	pixel = row;
	padding = 0;

	for (x=0; x<self->width; x++) {
	    bgr[2] = pixel[0];
	    bgr[1] = pixel[1];
	    bgr[0] = pixel[2];

	    fwrite(bgr, 1, 3, self->file);
	    pixel += n_channels;
	    padding = (padding - 3) & 3;
	}

	if (padding) {
	    bgr[0] = bgr[1] = bgr[2] = 0;
	    fwrite(bgr, 1, padding, self->file);
	}
	row -= rowstride;
    }

    avi_writer_pop_chunk_with_index(self, AVIIF_KEYFRAME);
    self->frame_count++;
}

void avi_writer_close (AviWriter *self) {
    avi_writer_pop_chunk(self);     /* Close the "movi" list */
    avi_writer_write_index(self);
    avi_writer_pop_chunk(self);     /* Close the "AVI" chunk */

    /* At this point, there should be no more open chunks */
    g_assert(self->chunk_stack == NULL);

    /* Fix up frame count and stream length */
    fseek(self->file, self->frame_count_offset, SEEK_SET);
    write_int32(self->file, self->frame_count);
    fseek(self->file, self->length_offset, SEEK_SET);
    write_int32(self->file, self->frame_count * self->frame_rate * RATE_SCALE);

    fclose(self->file);
    g_queue_free(self->index_queue);
}


/************************************************************************************/
/********************************************************************** AVI Headers */
/************************************************************************************/

static void avi_writer_write_header_list(AviWriter *self) {
    avi_writer_push_list(self, "hdrl");

    avi_writer_write_main_header(self);

    avi_writer_push_list(self, "strl");
    avi_writer_write_stream_header(self);
    avi_writer_write_stream_format(self);
    avi_writer_pop_chunk(self);

    avi_writer_pop_chunk(self);
}

static void avi_writer_write_main_header(AviWriter *self) {
    avi_writer_push_chunk(self, "avih");

    /* microseconds per frame */
    write_int32(self->file, 1000000 / self->frame_rate);

    /* max bytes per second */
    write_int32(self->file, 0);

    /* padding granularity */
    write_int32(self->file, 0);

    /* flags (AVIF_* constants) */
    write_int32(self->file, AVIF_HASINDEX);

    /* total frames (we fill this in later) */
    self->frame_count_offset = ftell(self->file);
    write_int32(self->file, 0);

    /* inital frames */
    write_int32(self->file, 0);

    /* number of streams */
    write_int32(self->file, 1);

    /* suggested buffer size */
    write_int32(self->file, self->width * self->height * 3 + 1024);

    /* width and height */
    write_int32(self->file, self->width);
    write_int32(self->file, self->height);

    /* reserved (4) */
    write_int32(self->file, 0);
    write_int32(self->file, 0);
    write_int32(self->file, 0);
    write_int32(self->file, 0);

    avi_writer_pop_chunk(self);
}

static void avi_writer_write_stream_header(AviWriter *self) {
    avi_writer_push_chunk(self, "strh");

    /* data type: video stream */
    write_fourcc(self->file, "vids");

    /* data handler (video codec) */
    write_fourcc(self->file, "DIB ");

    /* flags */
    write_int32(self->file, 0);

    /* priority */
    write_int16(self->file, 1);

    /* language */
    write_int16(self->file, 0);

    /* initial frames */
    write_int32(self->file, 0);

    /* scale followed by rate. For video streams, (rate/scale) is the frame rate. */
    write_int32(self->file, RATE_SCALE);
    write_int32(self->file, self->frame_rate * RATE_SCALE);

    /* start */
    write_int32(self->file, 0);

    /* length (we fill this in later) */
    self->length_offset = ftell(self->file);
    write_int32(self->file, 0);

    /* suggested buffer size */
    write_int32(self->file, self->width * self->height * 3 + 1024);

    /* quality */
    write_int32(self->file, 10000);

    /* sample size */
    write_int32(self->file, 0);

    /* frame position and size (left, top, right, bottom) */
    write_int16(self->file, 0);
    write_int16(self->file, 0);
    write_int16(self->file, self->width - 1);
    write_int16(self->file, self->height - 1);

    avi_writer_pop_chunk(self);
}

static void avi_writer_write_stream_format(AviWriter *self) {
    avi_writer_push_chunk(self, "strf");
    /* This is a BITMAPINFO structure for video streams */

    /* BITMAPINFOHEADER size */
    write_int32(self->file, 0x28);

    /* width and height */
    write_int32(self->file, self->width);
    write_int32(self->file, self->height);

    /* Number of planes, "must be set to 1" */
    write_int16(self->file, 1);

    /* bits per pixel */
    write_int16(self->file, 24);

    /* compression */
    write_int32(self->file, 0);

    /* size, in bytes, of the image */
    write_int32(self->file, self->width * self->height * 3);

    /* horizontal/vertical pixels per meter (75 dpi) */
    write_int32(self->file, 2952);
    write_int32(self->file, 2952);

    /* Color table size and number of important colors */
    write_int32(self->file, 0);
    write_int32(self->file, 0);

    avi_writer_pop_chunk(self);
}


/************************************************************************************/
/********************************************************************** RIFF Chunks */
/************************************************************************************/

static void write_fourcc(FILE *f, const char *fourcc) {
    fwrite(fourcc, 1, 4, f);
}

static void write_int32(FILE *f, gint32 i) {
    i = GINT32_TO_LE(i);
    fwrite(&i, 1, 4, f);
}

static void write_int16(FILE *f, gint16 i) {
    i = GINT16_TO_LE(i);
    fwrite(&i, 1, 2, f);
}

static void avi_writer_push_chunk(AviWriter *self, const char *fourcc) {
    /* Write a chunk's RIFF header, with a dummy size, and add
     * it to the chunk_stack. The file pointer will be positioned
     * at the beginning of the chunk's data.
     */
    ChunkStackEntry *new_chunk = g_new(ChunkStackEntry, 1);

    write_fourcc(self->file, fourcc);
    strncpy(new_chunk->fourcc, fourcc, 4);
    new_chunk->fourcc[4] = '\0';

    /* Write a dummy size field, pointing to it for later updating */
    new_chunk->size_field = ftell(self->file);
    write_int32(self->file, 0);

    /* Point to the beginning of the chunk's data */
    new_chunk->data_start = ftell(self->file);

    self->chunk_stack = g_slist_prepend(self->chunk_stack, new_chunk);
}

static void avi_writer_pop_chunk(AviWriter *self) {
    /* Pop a chunk off of the chunk_stack, updating the chunk's size.
     * Assumes that the file pointer is placed just after the chunk data.
     */
    long after_data = ftell(self->file);
    ChunkStackEntry *old_chunk;
    GSList *removed_link;

    /* Pop this chunk off our stack */
    removed_link = self->chunk_stack;
    self->chunk_stack = self->chunk_stack->next;
    old_chunk = (ChunkStackEntry*) removed_link->data;
    g_slist_free_1(removed_link);

    /* Update its header's size */
    fseek(self->file, old_chunk->size_field, SEEK_SET);
    write_int32(self->file, after_data - old_chunk->data_start);

    fseek(self->file, after_data, SEEK_SET);
    g_free(old_chunk);
}

static void avi_writer_push_header(AviWriter *self, const char *fileType) {
    avi_writer_push_chunk(self, "RIFF");
    write_fourcc(self->file, fileType);
}

static void avi_writer_push_list(AviWriter *self, const char *listType) {
    avi_writer_push_chunk(self, "LIST");
    write_fourcc(self->file, listType);
}


/************************************************************************************/
/************************************************************************ AVI Index */
/************************************************************************************/

/* Write the "idx1" list, using the queued chunk positions from our index FIFO */
static void avi_writer_write_index(AviWriter *self)
{
    IndexQueueEntry *current_entry;

    avi_writer_push_chunk(self, "idx1");

    /* Write all IndexQueueEntry nodes, freeing them as we go */
    while ((current_entry = g_queue_pop_head(self->index_queue))) {

	write_fourcc(self->file, current_entry->fourcc);
	write_int32(self->file, current_entry->flags);
	write_int32(self->file, current_entry->offset);
	write_int32(self->file, current_entry->size);

	g_free(current_entry);
    }

    avi_writer_pop_chunk(self);
}

/* This is a version of avi_writer_pop_chunk() that adds the finished chunk
 * to this AVI file's index, using the provided flags.
 */
static void avi_writer_pop_chunk_with_index (AviWriter *self, int index_flags)
{
    IndexQueueEntry *new_entry = g_new(IndexQueueEntry, 1);
    ChunkStackEntry *current_chunk = self->chunk_stack->data;
    long current_offset = ftell(self->file);

    memcpy(new_entry->fourcc, current_chunk->fourcc, 5);
    new_entry->flags = index_flags;
    new_entry->offset = current_chunk->data_start - self->index_origin_offset - 4;
    new_entry->size = current_offset - current_chunk->data_start;

    g_queue_push_tail(self->index_queue, new_entry);
    avi_writer_pop_chunk(self);
}

/* The End */

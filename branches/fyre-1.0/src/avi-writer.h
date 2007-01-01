/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * avi-writer.h - A simple interface for writing uncompressed RGB frames to
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

#ifndef __AVI_WRITER_H__
#define __AVI_WRITER_H__

#include <stdio.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define AVI_WRITER_TYPE            (avi_writer_get_type ())
#define AVI_WRITER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), AVI_WRITER_TYPE, AviWriter))
#define AVI_WRITER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), AVI_WRITER_TYPE, AviWriterClass))
#define IS_AVI_WRITER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AVI_WRITER_TYPE))
#define IS_AVI_WRITER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), AVI_WRITER_TYPE))

typedef struct _AviWriter      AviWriter;
typedef struct _AviWriterClass AviWriterClass;

struct _AviWriter {
    GObject object;

    FILE *file;

    guint width, height;
    float frame_rate;
    guint32 frame_count;

    /* A stack of RIFF chunks that need their sizes fixed */
    GSList *chunk_stack;

    /* A FIFO of index chunks to write later */
    GQueue *index_queue;
    long index_origin_offset;

    /* Offsets of particular things we need to fix later */
    long frame_count_offset;
    long length_offset;
};

struct _AviWriterClass {
    GObjectClass parent_class;
};


/************************************************************************************/
/******************************************************************* Public Methods */
/************************************************************************************/

GType       avi_writer_get_type           ();
AviWriter*  avi_writer_new                (FILE *file, guint width, guint height, float frame_rate);
void        avi_writer_append_frame       (AviWriter *self, const GdkPixbuf *frame);
void        avi_writer_close              (AviWriter *self);

G_END_DECLS

#endif /* __AVI_WRITER_H__ */

/* The End */

/*
 * chunked-file.h - A simple interface for reading and writing binary files
 *                  consisting of PNG-style chunks with 4-character identifiers.
 *
 * de Jong Explorer - interactive exploration of the Peter de Jong attractor
 * Copyright (C) 2004 David Trowbridge and Micah Dowty
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

#ifndef __CHUNKED_FILE_H__
#define __CHUNKED_FILE_H__

#include <stdio.h>
#include <glib-object.h>

G_BEGIN_DECLS

/* Convert 4 characters representing the type code into a number which, in network
 * byte order, is represented by the same bytes. This makes chunk types
 * easier to work with. In particular, they can be used as switch statement cases.
 */
typedef guint32 ChunkType;
#define CHUNK_TYPE(a,b,c,d)   ((guint32)(((a) << 24) | ((b) << 16) | ((c) << 8) | (d)))


void      chunked_file_write_signature(FILE* self, const gchar* signature);
gboolean  chunked_file_read_signature(FILE* self, const gchar* signature);

void      chunked_file_write_chunk(FILE* self, ChunkType type, gsize length, const guchar* data);
gboolean  chunked_file_read_chunk(FILE* self, ChunkType *type, gsize *length, guchar** data);
void      chunked_file_warn_unknown_type(ChunkType type);

gchar*    chunk_type_to_string(ChunkType type);

G_END_DECLS

#endif /* __CHUNKED_FILE_H__ */

/* The End */

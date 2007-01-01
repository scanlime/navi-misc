/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * chunked-file.c - A simple interface for reading and writing binary files
 *                  consisting of PNG-style chunks with 4-character identifiers.
 *
 *                  As with PNG images, the file consists of a fixed signature
 *                  followed by any number of chunks. Each chunk consists of
 *                  a 32-bit length, 4-byte chunk type, data, and a CRC.
 *                  The chunk format and CRC used here is compatible with PNG, but
 *                  this module does not specify the format of the chunk type
 *                  codes or of the file signature.
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

#include <string.h>
#include "chunked-file.h"

/************************************************************************************/
/*************************************************************** CRC Implementation */
/************************************************************************************/

/* This implementation is a modified version of the one included in
 * the PNG specification's appendix. It has been altered to use GLib data types.
 */

/* Table of CRCs of all 8-bit messages. */
static guint32 crc_table[256];

/* Flag: has the table been computed? Initially false. */
static gboolean crc_table_computed = 0;

/* Make the table for a fast CRC. */
static void make_crc_table() {
    guint32 c;
    int n, k;

    for (n = 0; n < 256; n++) {
	c = (guint32) n;
	for (k = 0; k < 8; k++) {
	    if (c & 1)
		c = 0xedb88320L ^ (c >> 1);
	    else
		c = c >> 1;
	}
	crc_table[n] = c;
    }
    crc_table_computed = TRUE;
}

/* Update a running CRC with the bytes buf[0..len-1]--the CRC
 * should be initialized to all 1's, and the transmitted value
 * is the 1's complement of the final running CRC (see the
 * crc() routine below)).
 */
static guint32 update_crc(guint32 crc, const guchar *buf, guint len) {
    guint32 c = crc;
    guint n;

    if (!crc_table_computed)
	make_crc_table();
    for (n = 0; n < len; n++) {
	c = crc_table[(c ^ buf[n]) & 0xff] ^ (c >> 8);
    }
    return c;
}

/* Return the CRC of a data field and a type field */
static guint32 chunk_crc(ChunkType type, gsize length, const guchar* data) {
    guint32 c = 0xffffffffL;
    guint32 word;

    word = GUINT32_TO_BE(type);
    c = update_crc(c, (const guchar*) &word, sizeof(word));

    c = update_crc(c, data, length);

    return c ^ 0xffffffffL;
}


/************************************************************************************/
/******************************************************************* Public Methods */
/************************************************************************************/

void chunked_file_write_signature(FILE* self, const gchar* signature) {
    fwrite((void *) signature, strlen((void *) signature), 1, self);
}

gboolean chunked_file_read_signature(FILE* self, const gchar* signature) {
    /* Read a signature from the file, returning TRUE on success and FALSE on failure
     */
    int expected_size = strlen((void *) signature);
    gchar read_sig[expected_size];

    fseek(self, 0, SEEK_SET);
    if (fread(read_sig, 1, expected_size, self) != expected_size)
	return FALSE;

    if (memcmp(read_sig, (void *) signature, expected_size))
	return FALSE;

    return TRUE;
}

void chunked_file_write_chunk(FILE* self, ChunkType type, gsize length, const guchar* data) {
    guint32 word;

    word = GUINT32_TO_BE(length);
    fwrite(&word, sizeof(word), 1, self);

    word = GUINT32_TO_BE(type);
    fwrite(&word, sizeof(word), 1, self);

    fwrite(data, length, 1, self);

    word = GUINT32_TO_BE(chunk_crc(type, length, data));
    fwrite(&word, sizeof(word), 1, self);
}

gboolean chunked_file_read_chunk(FILE* self, ChunkType *type, gsize *length, guchar** data) {
    /* Try to read the next chunk from the file. Returns FALSE on EOF or
     * unrecoverable error. Returns TRUE on success, and sets type, length, and data
     * to hold the chunk contents. If this returns TRUE, the caller must free the
     * data buffer.
     */
    guint32 word;

    /* Loop until we get a valid chunk, we hit the end of file, or we hit an unrecoverable error */
    while (1) {

	/* Read length */
	if (fread(&word, 1, sizeof(word), self) != sizeof(word))
	    return FALSE;
	*length = GUINT32_FROM_BE(word);

	/* Read type */
	if (fread(&word, 1, sizeof(word), self) != sizeof(word)) {
	    g_log(G_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		  "Unexpected EOF trying to read chunk type");
	    return FALSE;
	}
	*type = GUINT32_FROM_BE(word);

	/* Read data, allocating a new buffer for it */
	*data = g_malloc(*length);
	if (fread(*data, 1, *length, self) != *length) {
	    gchar *type_string = chunk_type_to_string(*type);
	    g_log(G_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		  "Unexpected EOF trying to read data for chunk of type %s", type_string);
	    g_free(*data);
	    g_free(type_string);
	    return FALSE;
	}

	/* Read and validate the CRC. If it passes,
	 * return successfully, otherwise issue a warning
	 * and ignore the corrupted chunk.
	 */
	if (fread(&word, 1, sizeof(word), self) != sizeof(word)) {
	    g_log(G_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		  "Unexpected EOF trying to read chunk CRC");
	    g_free(*data);
	    return FALSE;
	}
	if (chunk_crc(*type, *length, *data) == GUINT32_FROM_BE(word)) {
	    return TRUE;
	}
	else {
	    gchar *type_string = chunk_type_to_string(*type);
	    g_log(G_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
		  "Ignoring corrupted chunk of type %s", type_string);
	    g_free(*data);
	    g_free(type_string);
	}
    }
}

void chunked_file_read_all(FILE* self, ChunkCallback callback, gpointer user_data) {
    ChunkType type;
    gsize length;
    guchar* data;

    while (chunked_file_read_chunk(self, &type, &length, &data)) {
	callback(user_data, type, length, data);
	g_free(data);
    }
}

void chunked_file_warn_unknown_type(ChunkType type) {
    gchar *type_string = chunk_type_to_string(type);
    g_log(G_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	  "Ignoring unrecognized chunk of type %s", type_string);
    g_free(type_string);
}

gchar* chunk_type_to_string(ChunkType type) {
    return g_strdup_printf("'%c%c%c%c'",
			   (type >> 24) & 0xFF,
			   (type >> 16) & 0xFF,
			   (type >> 8) & 0xFF,
			   type & 0xFF);
}

/* The End */

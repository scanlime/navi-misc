/*
 * bit_file.h - An abstraction for the Xilinx .bit file format,
 *              supports loading the bitstream data itself and
 *              associated metadata from a .bit file.
 *
 *              Thanks to Dave Sullins and his bitinfo utility for
 *              giving a good introduction to the header format.
 *
 * Universal Controller Emulator project
 * Copyright (C) 2004 Micah Dowty
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

#ifndef _H_BIT_FILE
#define _H_BIT_FILE

#include <stdio.h>

struct bitfile {
  /* Only present if the file hasn't been read in completely yet */
  FILE *file;

  /* Metadata */
  char *ncd_filename;
  char *part_number;
  char *date;
  char *time;

  /* The bitstream itself. The length is read at the same time
   * as the rest of the header, but the data itself may be NULL
   * if it hasn't yet been read in.
   */
  int length;
  unsigned char *data;
};

/* Create a bitfile instance from a FILE* or a file name.
 * The FILE* will be closed by the bitfile instance when it's
 * deleted or the content has been read. Returns NULL on error.
 */
struct bitfile*      bitfile_new_from_file     (FILE* f);
struct bitfile*      bitfile_new_from_path     (const char *path);
void                 bitfile_delete            (struct bitfile *self);

/* The bit file's header is read immediately, but content is not read
 * until this function is called. The application may want to skip the
 * file entirely if the metadata reveals problems, or it may want to
 * read the content using an alternative method.
 *
 * After a bit file has been opened, its file pointer will always be
 * pointing to the beginning of the content.
 *
 * Returns the number of bytes read if the entire content was
 * read successfully, or <= 0 on error.
 */
int                  bitfile_read_content      (struct bitfile *self);

#endif /* _H_BIT_FILE */

/* The End */

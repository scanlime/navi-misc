/* -*- mode: c; c-basic-offset: 4; -*-
 *
 * image-fu.h - Imaging utilities for manipulating GdkPixbufs
 *
 * Fyre - rendering and interactive exploration of chaotic functions
 * Copyright (C) 2004-2005 David Trowbridge and Micah Dowty
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

#ifndef __IMAGE_FU_H__
#define __IMAGE_FU_H__

#include <gtk/gtk.h>

/* This must be a power of two. It's hardcoded in image_add_checkerboard
 * for speed, but it might be needed for calculations elsewhere.
 */
#define CHECKERBOARD_TILE_SIZE 8

/* Do an in-place composite on a GdkPixbuf to render it in front of a checkerboard pattern */
void  image_add_checkerboard(GdkPixbuf *img);

/* Modify an image in-place to include a thin frame */
void  image_add_thumbnail_frame(GdkPixbuf *img);

/* Adjust an image's levels automatically, to make faint images more visible */
void  image_adjust_levels(GdkPixbuf *img);

#endif /* __IMAGE_FU_H__ */

/* The End */
